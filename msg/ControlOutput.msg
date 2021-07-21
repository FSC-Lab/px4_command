std_msgs/Header header

## 位置控制器输出： 期望角度、期望角加速度
float64[3] u_l                 ## [0-1] 惯性系下的油门量
float64[3] u_d                 ## [rad]

float64[3] NE                  ## [rad]
  
float64[3] Thrust              ## [rad]  
float64[3] Throttle