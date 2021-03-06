/***************************************************************************************************************************
 * px4_multidrone_pos_estimator_pure_vision.cpp
 *
 * Author: Longhao Qian
 *
 * Update Time: 2019.8.31
 *
 * 说明: mavros位置估计程序 (optitrack only)
 *      1. subscribe position and velocity data of drone and payload from ground station via Mocap topic
 *      2. for multi drone implmentation. auto add UAV number to pub and sub topics
 *      3. 订阅飞控发布的位置、速度及欧拉角信息，作对比用
 *      4. 存储飞行数据，实验分析及作图使用
 *
 ***************************************************************************************************************************/
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TransformStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/Vector3Stamped.h>
#include <math_utils.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <nav_msgs/Odometry.h>
#include <px4_command/DroneState.h>
#include <px4_command/Mocap.h>
#include <ros/ros.h>
#include <sensor_msgs/BatteryState.h>
#include <sensor_msgs/Imu.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Float64.h>
#include <std_msgs/UInt16.h>
#include <tf2_msgs/TFMessage.h>
#include <Eigen/Eigen>
#include <iostream>

using namespace std;
px4_command::DroneState _DroneState;
px4_command::Mocap UAV_motion;
px4_command::Mocap Payload_motion;
bool UAVsubFlag;
bool PaylaodsubFlag;
bool MocapOK;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>   Callbacks   <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void GetUAVBattery(const sensor_msgs::BatteryState::ConstPtr& msg) {
  _DroneState.battery_voltage = msg->voltage;
  _DroneState.battery_remaining = msg->percentage;
}
void GetUAVState(const px4_command::Mocap::ConstPtr& msg) {
  UAV_motion = *msg;
  for (int i = 0; i < 3; i++) {
    _DroneState.position[i] = UAV_motion.position[i];
    _DroneState.velocity[i] = UAV_motion.velocity[i];
  }
  UAVsubFlag = true;
}
void GetPayloadState(const px4_command::Mocap::ConstPtr& msg) {
  Payload_motion = *msg;
  for (int i = 0; i < 3; i++) {
    _DroneState.payload_vel[i] = Payload_motion.velocity[i];
    _DroneState.payload_pos[i] = Payload_motion.position[i];
    _DroneState.payload_angular_vel[i] = Payload_motion.angular_velocity[i];
  }

  for (int i = 0; i < 4; i++) {
    _DroneState.payload_quaternion[i] = Payload_motion.quaternion[i];
  }

  PaylaodsubFlag = true;
}
void GetMavrosState(const mavros_msgs::State::ConstPtr& msg) {
  _DroneState.connected = msg->connected;
  _DroneState.armed = msg->armed;
  _DroneState.mode = msg->mode;
}
void GetAttitude(const sensor_msgs::Imu::ConstPtr& msg) {
  Eigen::Quaterniond q_fcu =
      Eigen::Quaterniond(msg->orientation.w, msg->orientation.x, msg->orientation.y, msg->orientation.z);
  // Transform the Quaternion to euler Angles
  Eigen::Vector3d euler_fcu = quaternion_to_euler(q_fcu);
  _DroneState.attitude_q.w = q_fcu.w();
  _DroneState.attitude_q.x = q_fcu.x();
  _DroneState.attitude_q.y = q_fcu.y();
  _DroneState.attitude_q.z = q_fcu.z();
  _DroneState.attitude[0] = euler_fcu[0];
  _DroneState.attitude[1] = euler_fcu[1];
  _DroneState.attitude[2] = euler_fcu[2];
  _DroneState.attitude_rate[0] = msg->angular_velocity.x;
  _DroneState.attitude_rate[1] = msg->angular_velocity.y;
  _DroneState.attitude_rate[2] = msg->angular_velocity.z;
  _DroneState.acceleration[0] = msg->linear_acceleration.x;
  _DroneState.acceleration[1] = msg->linear_acceleration.y;
  _DroneState.acceleration[2] = msg->linear_acceleration.z;
}
int main(int argc, char** argv) {
  ros::init(argc, argv, "px4_multidrone_pos_estimator_pure_vision");
  ros::NodeHandle nh("~");
  ros::Rate rate(50.0);

  /*----------- determine the  ID of the drone -----------------------------*/
  std::string mocap_UAV;
  std::string mavros_state;
  std::string mavros_imu_data;
  std::string mavros_battery;
  std::string px4_command_drone_state;
  std::string mavros_vision_pose_pose;
  // add preflex: (mavros and px4_command use lower case uav, while mocap use upper case UAV)
  mocap_UAV = "/mocap/UAV";
  mavros_state = "/uav";
  mavros_imu_data = "/uav";
  px4_command_drone_state = "/uav";
  mavros_vision_pose_pose = "/uav";
  mavros_battery = "/uav";
  if (argc > 1) {
    // if ID is specified as the second argument
    mocap_UAV += argv[1];
    mavros_state += argv[1];
    mavros_imu_data += argv[1];
    mavros_battery += argv[1];
    px4_command_drone_state += argv[1];
    mavros_vision_pose_pose += argv[1];
    ROS_INFO("UAV ID specified as: UAV%s", argv[1]);
  } else {
    // if ID is not specified, then set the drone to UAV0
    mocap_UAV += "0";
    mavros_state += "0";
    mavros_imu_data += "0";
    px4_command_drone_state += "0";
    mavros_vision_pose_pose += "0";
    mavros_battery += "0";
    ROS_WARN("NO UAV ID specified, set ID to 0.");
  }

  mavros_state += "/mavros/state";
  mavros_imu_data += "/mavros/imu/data";
  px4_command_drone_state += "/px4_command/drone_state";
  mavros_vision_pose_pose += "/mavros/vision_pose/pose";
  mavros_battery += "/mavros/battery";

  ROS_INFO("Subscribe uav Mocap from: %s", mocap_UAV.c_str());
  ROS_INFO("Subscribe payload from: /mocap/Payload");
  ROS_INFO("Subscribe mavros_msgs::State from: %s", mavros_state.c_str());
  ROS_INFO("Subscribe IMU from: %s", mavros_imu_data.c_str());
  ROS_INFO("Subscribe battery info from: %s", mavros_battery.c_str());
  ROS_INFO("Publish DroneState to: %s", px4_command_drone_state.c_str());
  ROS_INFO("Publish PoseStamped to: %s", mavros_vision_pose_pose.c_str());

  // subscriber
  ros::Subscriber UAV_motion_sub = nh.subscribe<px4_command::Mocap>(mocap_UAV, 1000, GetUAVState);
  ros::Subscriber MavrosBattery_sub = nh.subscribe<sensor_msgs::BatteryState>(mavros_battery, 1000, GetUAVBattery);
  ros::Subscriber Payload_motion_sub = nh.subscribe<px4_command::Mocap>("/mocap/Payload", 1000, GetPayloadState);
  ros::Subscriber MavrosState_sub = nh.subscribe<mavros_msgs::State>(mavros_state, 100, GetMavrosState);
  ros::Subscriber Attitude_sub = nh.subscribe<sensor_msgs::Imu>(mavros_imu_data, 100, GetAttitude);
  // publisher
  ros::Publisher drone_state_pub = nh.advertise<px4_command::DroneState>(px4_command_drone_state, 100);
  /*【发布】无人机位置和偏航角 坐标系 ENU系
  本话题要发送飞控(通过mavros_extras/src/plugins/vision_pose_estimate.cpp发送),
  对应Mavlink消息为VISION_POSITION_ESTIMATE(#??), 对应的飞控中的uORB消息为vehicle_vision_position.msg
  及 vehicle_vision_attitude.msg */
  ros::Publisher vision_pub = nh.advertise<geometry_msgs::PoseStamped>(mavros_vision_pose_pose, 100);
  geometry_msgs::PoseStamped vision;  // vision data

  const int MocapTolerance = 2;
  int NoFeedBackCounter = 0;
  ROS_INFO("Start the estimator...");
  while (ros::ok()) {
    //回调一次 更新传感器状态
    ros::spinOnce();
    vision.pose.position.x = UAV_motion.position[0];
    vision.pose.position.y = UAV_motion.position[1];
    vision.pose.position.z = UAV_motion.position[2];
    vision.pose.orientation.w = UAV_motion.quaternion[0];
    vision.pose.orientation.x = UAV_motion.quaternion[1];
    vision.pose.orientation.y = UAV_motion.quaternion[2];
    vision.pose.orientation.z = UAV_motion.quaternion[3];
    vision.header.stamp = ros::Time::now();
    vision_pub.publish(vision);  // send vision measurment to fcu

    _DroneState.header.stamp = ros::Time::now();
    MocapOK = true;  // reset the mocap flag to false
    if (!UAVsubFlag) {
      MocapOK = false;
    }
    // reset the sub flag to false
    UAVsubFlag = false;
    // determine whether the mocap feedback is normal
    _DroneState.mocapOK = true;
    if (MocapOK) {
      NoFeedBackCounter = 0;
      _DroneState.mocapOK = true;
    } else {
      NoFeedBackCounter++;
      if (NoFeedBackCounter >= MocapTolerance) {
        _DroneState.mocapOK = false;
      } else {
        _DroneState.mocapOK = true;
      }
    }

    drone_state_pub.publish(_DroneState);
    rate.sleep();
  }
  return 0;
}