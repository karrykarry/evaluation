/* evaluation_function.cpp
 *
 * 2019.12.05
 *
 * author : R.Kusakari
 *
*/ 
#include<ros/ros.h>
#include"evaluation_function.hpp"

int main(int argc, char* argv[])
{
    ros::init(argc, argv, "recovery");
	ros::NodeHandle n;
	ros::NodeHandle priv_nh("~");

    ROS_INFO("\033[1;32m---->\033[0m Evaluation Function Started.");
	
	Eval eval(n,priv_nh);	

	ros::spin();
 
	return 0;
}

       






