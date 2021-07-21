std_msgs/Header header

## the estimated disturbances
float64  W_x    ## [N]
float64  W_y    ## [N]
float64  W_z    ## [N] 

## quadrotor attitude
float64  q_0     ## 
float64  q_1     ##
float64  q_2     ##
float64  q_3     ##

## cable motion
float64  r_x    ## [m]
float64  r_y    ## [m]

float64  v_x    ## [m]
float64  v_y    ## [m]

## tracking errors
float64  pos_error_x ## [m]
float64  pos_error_y ## [m]
float64  pos_error_z ## [m]

float64  vel_error_x  ## [m/s]
float64  vel_error_y  ## [m/s]
float64  vel_error_z  ## [m/s]

## payload position measurement from mocap
float64  Lm_x    ## [m]
float64  Lm_y    ## [m]
float64  Lm_z    ## [m]

## payload position measurement from mocap
float64  Vpm_x   ## [m/s]
float64  Vpm_y   ## [m/s]
float64  Vpm_z   ## [m/s]

## the  command lift 
float64  fL_x     ##
float64  fL_y     ##
float64  fL_z     ##

## the quadrotor inerital acceleration
float64  acc_x  ##
float64  acc_y  ##
float64  acc_z  ##