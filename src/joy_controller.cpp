#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include "ros/console.h"

class JoyTeleop{
    public:
        JoyTeleop();
    private:
        ros::NodeHandle nh_;
        ros::Publisher vel_pub_;
        ros::Subscriber joy_sub_;

        int linear_,angular_,changer_;
        double f_maxvel_,b_maxvel_,r_l_maxvel_,r_r_maxvel_;
        double l_scale_,a_scale_;
        bool control_mode_;

        void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);
        void publish();  
};


JoyTeleop::JoyTeleop()
{
nh_ = ros::NodeHandle();
nh_.param("axis_linear", linear_,1);
nh_.param("axis_angular", angular_,3);
nh_.param("button_changer", changer_,10);
nh_.param("forward_maxvel",f_maxvel_,1.0);
nh_.param("back_maxvel",b_maxvel_,-1.0);
nh_.param("rotate_R_maxvel",r_r_maxvel_,1.0);
nh_.param("rotate_L_maxvel",r_l_maxvel_,-1.0);

nh_.param("linear_scale",l_scale_,0.3);
nh_.param("angular_scale",a_scale_,0.5);

control_mode_ = true;
vel_pub_ = nh_.advertise<geometry_msgs::Twist>("/cmd_vel", 1, true);
joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("/joy", 10, &JoyTeleop::joyCallback, this);

}



void JoyTeleop::joyCallback(const sensor_msgs::Joy::ConstPtr& joy){
    geometry_msgs::Twist vel;
  
    vel.angular.z = a_scale_*joy->axes[angular_];
    vel.linear.x = l_scale_*joy->axes[linear_];
    
    if(vel.angular.z > r_r_maxvel_){
        vel.angular.z = r_r_maxvel_;
        ROS_INFO("angular.z value learger than r_r_maxvel_");
    }else if(vel.angular.z < r_l_maxvel_){
        vel.angular.z = r_l_maxvel_;
        ROS_INFO("angular.z value smaller than r_l_maxvel_");
    }
    
    if(vel.linear.x > f_maxvel_){
        vel.linear.x = f_maxvel_;
        ROS_INFO("linear.x value learger than f_maxvel_");
    }else if(vel.linear.x < b_maxvel_){
        vel.linear.x = b_maxvel_;
        ROS_INFO("linear.x value smaller than b_maxvel_");
    }

    if(joy->buttons[changer_]){
        control_mode_ = !control_mode_ ;
        vel.angular.z = 0;
        vel.linear.x = 0;
        vel_pub_.publish(vel);
        ROS_INFO("change mode");
    }

    if(control_mode_){
        vel_pub_.publish(vel);
        
    }
}

int main(int argc,char** argv){
    //ROSノードの初期化
    ros::init(argc,argv,"joy_controller_node");
    //インスタンスを作成
    JoyTeleop joy_custom;
    //callBackの待機処理を実行
    ros::spin();

}