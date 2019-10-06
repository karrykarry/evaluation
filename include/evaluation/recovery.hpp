#ifndef _RECOVERY_HPP_
#define _RECOVERY_HPP_
#include<iostream>
#include<vector>
#include<sstream>
#include<iomanip>
#include<fstream>

#include<ros/ros.h>
#include<std_msgs/Int32.h>
#include<std_msgs/Int32MultiArray.h>
#include<std_msgs/Empty.h>
#include<sensor_msgs/PointCloud2.h>

class Recovery{	
	
	public:
		Recovery(ros::NodeHandle n,ros::NodeHandle priv_nh);
		
		void scoreCallback(const std_msgs::Int32MultiArrayConstPtr &msgs);
		void processCallback(const std_msgs::EmptyConstPtr &msgs);

	private:
		ros::Publisher rebest_pub;

		ros::Subscriber better_sub;
		ros::Subscriber process_sub;
		
		std_msgs::Int32MultiArray better_score_num;
		sensor_msgs::PointCloud2 buffer_pc;
};

#endif




