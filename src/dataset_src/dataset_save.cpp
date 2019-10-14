/*
 * dataset用の画像を保存するもの
 *
 */
#include<stdio.h>
#include<iostream>
#include<string>

#include<ros/ros.h>
#include<std_msgs/Int32.h>
#include<std_msgs/Float64MultiArray.h>
#include<sensor_msgs/PointCloud.h>
#include<sensor_msgs/PointCloud2.h>
#include<geometry_msgs/Point.h>
#include<nav_msgs/OccupancyGrid.h>

#include<tf/tf.h>

#include<cv_bridge/cv_bridge.h>
#include<image_transport/image_transport.h>
#include<opencv2/opencv.hpp>
#include<opencv2/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/imgcodecs.hpp>

class Dataset_save{
	public:
		Dataset_save(ros::NodeHandle n, ros::NodeHandle private_nh_);
		~Dataset_save();
		image_transport::ImageTransport it;
		void scoreCallback(const std_msgs::Float64MultiArrayConstPtr& msgs);
		void processCallback(const std_msgs::Int32ConstPtr& msgs);
	
	private:
		ros::Publisher grid_pub;
		ros::Subscriber score_sub;
		ros::Subscriber process_sub;

		image_transport::Publisher image_pub;

		std_msgs::Float64MultiArray buffer_array;

		double P_RANGE, P_INTER, GRID_WIDTH;
		std::string FRAME_ID;
		std::string FILE_PATH;
		std::string file_dir;
		std::ofstream writing_file0;
		std::ofstream writing_file1;
		std::ofstream writing_file2;
		std::vector<double> min0;
		std::vector<double> max0;
		std::vector<double> min1;
		std::vector<double> max1;
		std::vector<double> min2;
		std::vector<double> max2;

		template<typename T> T MAX_(T val_1, T val_2);
		template<typename T> T MIN_(T val_1, T val_2);

};

Dataset_save::Dataset_save(ros::NodeHandle n, ros::NodeHandle private_nh_):
	it(private_nh_)
{
	grid_pub = n.advertise<nav_msgs::OccupancyGrid>("/score_grid", 1);
	
	score_sub = n.subscribe("/score/vis/context", 10, &Dataset_save::scoreCallback, this);
	process_sub = n.subscribe("/score/process", 10, &Dataset_save::processCallback, this);

	image_pub = it.advertise("/pf_score/image",10);
	
	private_nh_.param("PF_RANGE", P_RANGE, {5.0});	//パーティクルの範囲
	private_nh_.param("PF_INTER", P_INTER, {0.5});	//間引く距離
	private_nh_.param("FRAME_ID", FRAME_ID, {"/context_estimate"});
	private_nh_.param("image_save_path", FILE_PATH, {"/home/amsl/m2_result/image"});

	std::cout<<"FILE_PATH:"<<FILE_PATH<<std::endl;

    GRID_WIDTH = (P_RANGE / P_INTER)*2+1;
	
	writing_file0.open(FILE_PATH+"/0/context.csv", std::ios::out);
	writing_file1.open(FILE_PATH+"/1/context.csv", std::ios::out);
	writing_file2.open(FILE_PATH+"/2/context.csv", std::ios::out);
}


template<typename T>
T Dataset_save::MAX_(T val_1, T val_2){
	return val_1 > val_2 ? val_1 : val_2;
}

template<typename T>
T Dataset_save::MIN_(T val_1, T val_2){
	return val_1 < val_2 ? val_1 : val_2;
}

Dataset_save::~Dataset_save(){
	std::cout<<"----save now----"<<std::endl;

	for(auto min : min0){
		writing_file0 << min <<"," <<std::flush;
	}
	writing_file0 << std::endl;
	for(auto max : max0){
		writing_file0 << max <<"," <<std::flush;
	}

	for(auto min : min1){
		writing_file1 << min <<"," <<std::flush;
	}
	writing_file1 << std::endl;
	for(auto max : max1){
		writing_file1 << max <<"," <<std::flush;
	}

	for(auto min : min2){
		writing_file2 << min <<"," <<std::flush;
	}
	writing_file2 << std::endl;
	for(auto max : max2){
		writing_file2 << max <<"," <<std::flush;
	}


}


void
Dataset_save::scoreCallback(const std_msgs::Float64MultiArrayConstPtr& msgs){

	buffer_array = *msgs;
}

void
Dataset_save::processCallback(const std_msgs::Int32ConstPtr& msg){
	
	
	cv::Mat color_image;
	cv::Mat mono_image = cv::Mat::zeros(sqrt(buffer_array.data.size()), sqrt(buffer_array.data.size()), CV_8UC1);


	std::vector<double> states(buffer_array.data.size(), -1);
	double max_v = buffer_array.data[0];
	double min_v = buffer_array.data[0];
	
	for(auto buffer_data : buffer_array.data){
		max_v = MAX_((double)buffer_data,max_v);
		min_v = MIN_((double)buffer_data,min_v);
	}

	for(int i=0; i<GRID_WIDTH; i++){
		for(int j=0; j<GRID_WIDTH; j++){
			double score = (1- (buffer_array.data[i*GRID_WIDTH+j] - min_v) / (max_v - min_v))*100;
			states[i+j*GRID_WIDTH] = score;
			
			mono_image.at<uchar>(i, GRID_WIDTH-j) = score * (255.0/100.0);
		}
	}

	std::cout<<"\033[1;32mmax"<<max_v<<" min:"<<min_v<<"\033[0m"<<std::endl;


	static int image_num = 0;
	
	std::ostringstream oss;
	oss << std::setfill( '0' ) << std::setw( 4 ) << image_num++;

	cv::applyColorMap(mono_image, color_image, cv::COLORMAP_JET);
	
	if(msg->data == 0){
		min0.push_back(min_v);
		max0.push_back(max_v);
		file_dir = FILE_PATH+"/0";
	}
	else if(msg->data == 1){
		min1.push_back(min_v);
		max1.push_back(max_v);	
		file_dir = FILE_PATH+"/1";
	}
	else{
		min2.push_back(min_v);
		max2.push_back(max_v);	
		file_dir = FILE_PATH+"/2";
	}
	std::string image_file_dir = file_dir + "/" + oss.str()+"_"+std::to_string(min_v)+"_"+std::to_string(max_v);
	cv::imwrite(image_file_dir + ".png", color_image);


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

	sensor_msgs::ImagePtr img_msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", color_image).toImageMsg();
	image_pub.publish(img_msg);
	
}


int main(int argc, char** argv)
{
	ros::init(argc, argv ,"pf_evaluation");
	ros::NodeHandle n;
	ros::NodeHandle private_nh_("~");
    ROS_INFO("\033[1;32m---->\033[0m pf_evaluation Started.");

	Dataset_save dataset_save(n, private_nh_);

	ros::spin();

	return 0;
}

