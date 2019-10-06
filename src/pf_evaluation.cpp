/*
 *
 */
#include <stdio.h>
#include <iostream>
#include <ros/ros.h>

#include <std_msgs/Float64MultiArray.h>
#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <geometry_msgs/Point.h>
#include <nav_msgs/OccupancyGrid.h>

#include <tf/tf.h>

class Pf_eva{
	public:
		Pf_eva(ros::NodeHandle n, ros::NodeHandle private_nh_);
		void scoreCallback(const std_msgs::Float64MultiArrayConstPtr& msgs);
	
	private:
		ros::Publisher grid_pub;
		ros::Publisher grid_pub_;
		ros::Subscriber score_sub;

		double P_RANGE, P_INTER, GRID_WIDTH;
		std::string FRAME_ID;
		template<typename T> T MAX_(T val_1, T val_2);
		template<typename T> T MIN_(T val_1, T val_2);

};

Pf_eva::Pf_eva(ros::NodeHandle n, ros::NodeHandle private_nh_)
{
	grid_pub = n.advertise<nav_msgs::OccupancyGrid>("/score_grid", 1);
	grid_pub_ = n.advertise<nav_msgs::OccupancyGrid>("/score_grid/vis", 1);
	
	score_sub = n.subscribe("/score/vis/context", 10, &Pf_eva::scoreCallback, this);
	
	private_nh_.param("PF_RANGE", P_RANGE, {5.0});	//パーティクルの範囲
	private_nh_.param("PF_INTER", P_INTER, {0.5});	//間引く距離
	private_nh_.param("FRAME_ID", FRAME_ID, {"/context_estimate"});	//間引く距離

    GRID_WIDTH = (P_RANGE / P_INTER)*2+1;
}


template<typename T>
T Pf_eva::MAX_(T val_1, T val_2){
	return val_1 > val_2 ? val_1 : val_2;
}

template<typename T>
T Pf_eva::MIN_(T val_1, T val_2){
	return val_1 < val_2 ? val_1 : val_2;
}



void
Pf_eva::scoreCallback(const std_msgs::Float64MultiArrayConstPtr& msgs){
	
	std::vector<double> states(msgs->data.size(), -1);
	double max_v = msgs->data[0];
	double min_v = msgs->data[0];
	
	for(auto msg : msgs->data){
		max_v = MAX_((double)msg,max_v);
		min_v = MIN_((double)msg,min_v);
	}

	for(int i=0; i<GRID_WIDTH; i++){
		for(int j=0; j<GRID_WIDTH; j++){
			double score = (1- (msgs->data[i*GRID_WIDTH+j] - min_v) / (max_v - min_v))*100;
			states[i+j*GRID_WIDTH] = score;
		}
	}

	std::cout<<"\033[1;32mmax"<<max_v<<" min:"<<min_v<<"\033[0m"<<std::endl;

	nav_msgs::OccupancyGrid grid;
	grid.data.clear();
	
	grid.header.frame_id = FRAME_ID;
	grid.header.stamp = ros::Time(0);
	grid.info.resolution = P_INTER;
	grid.info.width = GRID_WIDTH;
	grid.info.height = GRID_WIDTH;
	grid.info.origin.position.x = -P_RANGE;
	grid.info.origin.position.y = -P_RANGE;
	grid.info.origin.orientation = tf::createQuaternionMsgFromYaw(0);

	for(auto state : states){
		grid.data.push_back(state);
	}

	grid_pub.publish(grid);
	
	// nav_msgs::OccupancyGrid grid_;
	//
	// grid_.header.frame_id = "/context_estimate";
	// grid_.header.stamp = ros::Time(0);
	// grid_.info.resolution = 1;
	// grid_.info.width = 100;
	// grid_.info.height = 0;
	// grid_.info.origin.position.x = -50;
	// grid_.info.origin.position.y = 0;
	// grid_.info.origin.orientation = tf::createQuaternionMsgFromYaw(0);
    //
    //
	// // int cnt_=0;
	// for(int cnt__=0;cnt__<200;cnt__++){
	// 	// if(cnt_>255) cnt_=3;
	// 	grid_.data.push_back(cnt__);
	// 	cnt__++;
	// }
    //
    //
	// grid_pub_.publish(grid_);
    //
}


int main(int argc, char** argv)
{
	ros::init(argc, argv ,"pf_evaluation");
	ros::NodeHandle n;
	ros::NodeHandle private_nh_("~");
    ROS_INFO("\033[1;32m---->\033[0m pf_evaluation Started.");

	Pf_eva pf_eva(n, private_nh_);

	ros::spin();

	return 0;
}

