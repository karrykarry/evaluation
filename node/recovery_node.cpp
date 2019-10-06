/* similarity_score.cpp
 *
 * 2019.10.03
 *
 * author : R.Kusakari
 *
*/ 
#include<ros/ros.h>
#include"recovery.hpp"

int main(int argc, char* argv[])
{
    ros::init(argc, argv, "recovery");
	ros::NodeHandle n;
	ros::NodeHandle priv_nh("~");
	// ros::Rate loop(10);

    ROS_INFO("\033[1;32m---->\033[0m recovery Started.");
	
	Recovery recovery(n,priv_nh);	

	ros::spin();
 
	return 0;
}

       





