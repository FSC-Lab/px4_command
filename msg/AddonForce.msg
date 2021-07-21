std_msgs/Header header

##the total disturbance estimation

float64  delta_Tx    ## [N]
float64  delta_Ty    ## [N]
float64  delta_Tz    ## [N] 

float64  delta_Rx    ## [N]
float64  delta_Ry    ## [N]
float64  delta_Rz    ## [N] 

## the following terms are used for different purpose for TCST and JGCD method
## for TCST, they are the cross feeding terms
## for JGCD, they are the additive mpc command

float64  R_1x        ## [N]
float64  R_1y        ## [N]
float64  R_1z        ## [N]

float64  R_2x        ## [N]
float64  R_2y        ## [N]
float64  R_2z        ## [N]

bool emergency        ## flag for sending out emergency landing signal
bool perform_action   ## flag for performing action