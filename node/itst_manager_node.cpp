/* make_pf_image_dataset.cpp
 *
 * 2019.10.11
 *
 * author : R.Kusakari
 *
*/ 
#include<ros/ros.h>
#include"itst_manager.hpp"

int main(int argc, char* argv[])
{
    ros::init(argc, argv, "itst_manager");
	ros::NodeHandle n;
	ros::NodeHandle priv_nh("~");
	// ros::Rate loop(10);

    ROS_INFO("\033[1;32m---->\033[0m Dataset prepared Started.");
	
	Dataset_itst dataset_itst(n,priv_nh);	

	ros::spin();
 
	return 0;
}

       






