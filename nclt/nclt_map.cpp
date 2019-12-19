#include <iostream>
#include <ros/ros.h>
#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud_conversion.h>

#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>

#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl_conversions/pcl_conversions.h>
 #include <pcl_ros/transforms.h>     


class MapPossess{
	private:
	ros::Publisher map_pub;
	ros::Subscriber lidar_sub;
	std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> pc_buffer;
	pcl::PointCloud<pcl::PointXYZI>::Ptr map_cloud;
	pcl::VoxelGrid<pcl::PointXYZI> vg;
	
	sensor_msgs::PointCloud2 transform_pc;
	sensor_msgs::PointCloud buffer_point;
		
	tf::TransformListener listener;
    Eigen::Affine3d affine_transform;
		
	static constexpr int BUFFER_SIZE = 2;
	
	public:
	MapPossess(ros::NodeHandle n, ros::NodeHandle priv_nh);
	~MapPossess();
	void lidarCallback(const sensor_msgs::PointCloud2ConstPtr&);
	bool listen_tf(sensor_msgs::PointCloud buffer_point, std::string Child_id, std::string Parent_id);

};

MapPossess::MapPossess(ros::NodeHandle n, ros::NodeHandle priv_nh):
	map_cloud (new pcl::PointCloud<pcl::PointXYZI>)
{
    map_cloud.reset(new pcl::PointCloud<pcl::PointXYZI>());
	map_pub = n.advertise<sensor_msgs::PointCloud2>("cloud_pcd", 1);
    lidar_sub = n.subscribe("velodyne_points", 1, &MapPossess::lidarCallback, this);
    
	vg.setLeafSize(1.0, 1.0, 1.0); // for global map visualization
	
}

MapPossess::~MapPossess(){

	std::cout<<"save now"<<std::endl;
	pcl::io::savePCDFileASCII ("/home/amsl/NCLT/map/nclt.pcd", *map_cloud);

}


bool
MapPossess::listen_tf(sensor_msgs::PointCloud buffer_point, std::string Child_id, std::string Parent_id){
	bool flag = false;
	sensor_msgs::PointCloud save_point;
	try{
		ros::Time time_now = buffer_point.header.stamp;
		listener.waitForTransform(Child_id, Parent_id, time_now, ros::Duration(0.5));
		//
		// // listener.lookupTransform(Child_id, Parent_id,  
		// // 		time_now, buffer_transform);
		// listener.transformPointCloud(Child_id, time_now, buffer_point, Parent_id, save_point);
		// sensor_msgs::convertPointCloudToPointCloud2(save_point, transform_pc);
	


		tf::StampedTransform stamped_transform;

		listener.lookupTransform(Child_id, Parent_id, time_now, stamped_transform);
		Eigen::Translation<double, 3> t(stamped_transform.getOrigin().x(), stamped_transform.getOrigin().y(), stamped_transform.getOrigin().z());
		Eigen::Quaterniond q(stamped_transform.getRotation().w(), stamped_transform.getRotation().x(), stamped_transform.getRotation().y(), stamped_transform.getRotation().z());
		affine_transform = t * q;
		std::cout << affine_transform.translation() << std::endl;
		std::cout << affine_transform.rotation() << std::endl;
		std::cout << time_now << std::endl;
		
		flag  = true;
		return flag;
	}
	catch (tf::TransformException ex){
		ROS_ERROR("%s",ex.what());
		ros::Duration(1.0).sleep();
		return flag;
	}
	return flag;
}



void 
MapPossess::lidarCallback(const sensor_msgs::PointCloud2ConstPtr& msgs)
{
	static int cnt_=1;
	
	if(cnt_==10){
	sensor_msgs::convertPointCloud2ToPointCloud(*msgs, buffer_point);

	if(listen_tf(buffer_point, "/world_", "/ground_truth_link_")){
	// if(listen_tf(buffer_point, "/ground_truth_link_", "/world_")){
	// if(listen_tf(buffer_point, "/ground_truth_link_", "/world")){

		pcl::PointCloud<pcl::PointXYZI>::Ptr input_cloud (new pcl::PointCloud<pcl::PointXYZI>);

		// pcl::fromROSMsg (transform_pc, *input_cloud);
		pcl::fromROSMsg (*msgs, *input_cloud);

		pcl::transformPointCloud(*input_cloud, *input_cloud, affine_transform);
		*map_cloud += *input_cloud;

		pcl::PointCloud<pcl::PointXYZI>::Ptr vc_map(new pcl::PointCloud<pcl::PointXYZI>());
		vg.setInputCloud(map_cloud);
		vg.filter(*vc_map);
		map_cloud = vc_map;

		sensor_msgs::PointCloud2 vis_laser_voxel;
		pcl::toROSMsg(*vc_map, vis_laser_voxel);           
		vis_laser_voxel.header.frame_id = "/world_"; 
		vis_laser_voxel.header.stamp = msgs->header.stamp; 
		map_pub.publish(vis_laser_voxel);
	}
	cnt_ = 1;
	}

	cnt_++;

}


int main (int argc, char** argv)
{
    	
	ros::init(argc, argv, "nclt_map"); 
    ros::NodeHandle n;
	ros::NodeHandle priv_nh("~");

	std::cout<<"-------nclt_map--------"<<std::endl;

	MapPossess maposess(n,priv_nh);
	
	ros::spin();

	return (0);
}


