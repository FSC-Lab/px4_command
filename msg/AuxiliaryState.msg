std_msgs/Header header

## quadrotor attitude

float64  q_0     ##
float64  q_1     ##
float64  q_2     ##
float64  q_3     ##

float64  r_jx    ## [m]
float64  r_jy    ## [m]

float64  v_jx    ## [m]
float64  v_jy    ## [m]

float64  L_measured ## [m] measured L from mocap system

## payload pos error

float64  pos_error_x ## [m]
float64  pos_error_y ## [m]
float64  pos_error_z ## [m]

float64  angle_error_x  ## 
float64  angle_error_y  ## 
float64  angle_error_z  ## 

## quadrotor euler angle

float64  Euler_roll     ##
float64  Euler_pitch    ##
float64  Euler_yaw      ##

float64  fLj_x  ## [N]
float64  fLj_y  ## [N]
float64  fLj_z  ## [N]

float64  Delta_jp_x ## [N]
float64  Delta_jp_y ## [N]
float64  Delta_jp_z ## [N]

float64  acc_x  ##
float64  acc_y  ##
float64  acc_z  ##

float64  rd_jx  ## 
float64  rd_jy  ##