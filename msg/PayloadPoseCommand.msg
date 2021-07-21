std_msgs/Header header

## Command_ID for safety, the command ID should increase with every command sent.
uint32 Command_ID

## Payload Pose Setpoint
float64    x_ref         ## [m]
float64    y_ref         ## [m]
float64    z_ref         ## [m]
float64    roll_ref      ## [deg]
float64    pitch_ref     ## [deg]
float64    yaw_ref       ## [deg]


