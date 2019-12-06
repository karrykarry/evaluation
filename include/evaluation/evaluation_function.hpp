#ifndef _EVALUATION_FUNCTION_HPP_
#define	_EVALUATION_FUNCTION_HPP_ 
#include<iostream>
#include<vector>
#include<sstream>
#include<iomanip>
#include<fstream>

#include<ros/ros.h>
#include<std_msgs/Int32.h>
#include<std_msgs/Int32MultiArray.h>
#include<std_msgs/Float64.h>
#include<std_msgs/Float64MultiArray.h>
#include<std_msgs/Empty.h>
#include<std_msgs/Bool.h>
#include<sensor_msgs/PointCloud2.h>
#include<geometry_msgs/Pose.h>
#include<geometry_msgs/PoseStamped.h>

#include<tf/tf.h>
#include<tf/transform_datatypes.h>
#include<tf/transform_broadcaster.h>

class Eval{	
	
	public:
		Eval(ros::NodeHandle n,ros::NodeHandle private_nh_);
		~Eval();
		
		void scoreCallback(const std_msgs::Int32MultiArrayConstPtr &msgs);
		void itstCallback(const std_msgs::Float64MultiArrayConstPtr &msgs);	
		void contextCallback(const std_msgs::Float64ConstPtr &msgs);
		void cnnCallback(const std_msgs::Float64ConstPtr &msgs);
		void poseCallback(const geometry_msgs::PoseStampedConstPtr &msgs);

	private:
		ros::Publisher rebest_pub;
		ros::Publisher answer_pub;
		ros::Publisher go_pub;

		ros::Subscriber node_sub;
		ros::Subscriber itst_sub;
		ros::Subscriber context_sub;
		ros::Subscriber cnn_sub;
		ros::Subscriber pose_sub;
		
		std_msgs::Int32MultiArray better_score_num;
		std_msgs::Float64MultiArray buffer_itst;
		int cnt_;
		int NUM_CANDIDATE;
		
		std::multimap<double, int, std::greater<double>> sorted_map;
		
		
		std::vector<double> buffer_context;
		std::vector<double> buffer_cnn;
		std::vector<geometry_msgs::PoseStamped> buffer_pose;
		
		tf::TransformBroadcaster br;
		tf::Transform transform; 
	
		void tf_pub(geometry_msgs::PoseStamped now_pose);
};

#endif


