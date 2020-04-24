#include<ros/ros.h>
#include<mavros_msgs/OverrideRCIn.h>
#include<geometry_msgs/Twist.h>

// Move motor and armed
// rostopic pub /mavros/rc/override mavros_msgs/OverrideRCIn "channels: [1650, 982, 1822, 1494, 982, 982, 2006, 2006]"
// Stop motor and unarmed
// rostopic pub /mavros/rc/override mavros_msgs/OverrideRCIn "channels: [1498, 982, 1493, 1494, 982, 982, 2006, 982]"


// [1498, 982, 1493, 1494, 982, 982, 2006, 982]
// 1498, 982, 1493, 1494, 982, 982, 2006, 982, 1729, 1494, 1494, 1494, 1494, 1494, 1494, 1815


// Channel 1 (Left / Right)  2000 right 1500 middle 1000 lelf
// Channel 2 (Unuse) 1500
// Channel 3 (Throttle)      1000 reverse_max 1500 middle 2000 forward max
// Channel 4 (Unuse)  1500
// Channel 5 (Mode)   1100 Manual 1500 Acro 1900 Auto
// Channel 6 (Unuse)  1500
// Channel 7 (Arm/Disarm) 1900 / 1100
// Channel 8 (Unuse)  982 or 1500 

#define MAX_STEER 2000
#define MIDDLE_STEER 1500
#define MIN_STEER 1000

#define MAX_TWIST_z 1
#define MIN_TWIST_z -1

#define MAX_THROTTLE 2000
#define BRAKE_THROTLE 1500
#define MIN_THROTTLE 1000

#define MAX_TWIST_x 1
#define MIN_TWIST_x -1

#define MANUAL 1100
#define Acro 1500
#define Auto 1900

#define ARMED 1900
#define DISARMED 1500

#define UNUSE 1500



ros::Publisher overdrive;
ros::Subscriber cmd;

int map(double x, double in_min, double in_max, int out_min, int out_max) 
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
double saturate (double value, double min, double max)
{
    if(value > max)
    {
        value = max;
    }
    if(value < min)
    {
        value = min;
    }
    return value;
}


void callback(const geometry_msgs::Twist::ConstPtr &msg)
{

    mavros_msgs::OverrideRCIn send;
    send.channels[0] = map(saturate(msg->angular.z,MIN_TWIST_z,MAX_TWIST_z), MIN_TWIST_z, MAX_TWIST_z, MIN_STEER, MAX_STEER);
    send.channels[1] = UNUSE;
    send.channels[2] = map(saturate(msg->linear.x,MIN_TWIST_z,MAX_TWIST_z), MIN_TWIST_x, MAX_TWIST_x, MIN_THROTTLE, MAX_THROTTLE);
    send.channels[3] = UNUSE;
    send.channels[4] = MANUAL;
    send.channels[5] = UNUSE;
    send.channels[6] = ARMED;
    send.channels[7] = UNUSE;
    
    overdrive.publish(send);
    return;
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "mavros_rc_overdrive");
    ros::NodeHandle n;

    overdrive = n.advertise<mavros_msgs::OverrideRCIn>("/mavros/rc/override", 10);
    cmd = n.subscribe("/cmd_vel",10, callback);

    ros::spin();
    return 0;


}
