/*
 * rm_ground from pcd
 * &
 * pcd to occupancy grid 
 *
 */
#include <stdio.h>
#include <iostream>
#include <ros/ros.h>

#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <geometry_msgs/Point.h>
#include <nav_msgs/OccupancyGrid.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <tf/tf.h>

class Pf_eva{
	public:
		Pf_eva(ros::NodeHandle n, ros::NodeHandle private_nh_);
	
	private:
		ros::Publisher grid_pub;

		void grid()
};

Pf_eva::Pf_eva(ros::NodeHandle n, ros::NodeHandle private_nh_):
{
	grid_pub = n.advertise<nav_msgs::OccupancyGrid>("/occupancy_grid", 1, true);

}



void
Pf_eva::grid(){
  

	nav_msgs::OccupancyGrid grid;
	// grid.header.frame_id = "/map";
	// grid.header.stamp = ros::Time::now();
	// grid.info.resolution = m_per_cell_;
	// grid.info.width = x_dim_;
	// grid.info.height = y_dim_;
	// grid.info.origin.position.x = x_val.min;
	// grid.info.origin.position.y = y_val.min;
	// grid.info.origin.orientation = tf::createQuaternionMsgFromYaw(0);



	grid_pub.publish(grid);
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

