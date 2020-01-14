/*
 * /result/negative の判定されたらrecoverする
 *
 */


#include"evaluation_function.hpp"
Eval::Eval(ros::NodeHandle n,ros::NodeHandle private_nh_)
{
	// image_pub1 = n.advertise<sensor_msgs::Image>("/image/1", 1);	
	// image_pub2 = n.advertise<sensor_msgs::Image>("/image/2", 1);	
	// image_pub3 = n.advertise<sensor_msgs::Image>("/image/3", 1);	
	// image_pub4 = n.advertise<sensor_msgs::Image>("/image/4", 1);	
	// image_pub5 = n.advertise<sensor_msgs::Image>("/image/5", 1);	
	// image_sub = n.subscribe<sensor_msgs::Image>("/pf_score/image", 1, &Eval::imageCallback, this);
	
	
	
	rebest_pub = n.advertise<std_msgs::Int32>("/score/best/itst", 10);	
	answer_pub = n.advertise<geometry_msgs::Pose>("/map2context_result",10);
	
	node_sub = n.subscribe<std_msgs::Int32MultiArray>("/score/better/itst", 1, &Eval::scoreCallback, this);

	itst_sub = n.subscribe<std_msgs::Float64MultiArray>("/eval_score/itst", 1, &Eval::itstCallback, this);

	context_sub = n.subscribe<std_msgs::Float64>("/eval_score/context", 1, &Eval::contextCallback, this);
	cnn_sub = n.subscribe<std_msgs::Float64>("/eval_score/cnn", 1, &Eval::cnnCallback, this);
	pose_sub = n.subscribe<geometry_msgs::PoseStamped>("/map2context_result_", 1, &Eval::poseCallback, this);


	private_nh_.param("Number_of_candidate", NUM_CANDIDATE, {5});

	buffer_cnn.resize(NUM_CANDIDATE);
	buffer_context.resize(NUM_CANDIDATE);
	buffer_pose.resize(NUM_CANDIDATE);
	buffer_image.resize(NUM_CANDIDATE);


	writing_file.open("/home/amsl/m2_result/all.txt", std::ios::out);
	writing_file1.open("/home/amsl/m2_result/context.txt", std::ios::out);
	writing_file2.open("/home/amsl/m2_result/cnn.txt", std::ios::out);
	writing_file3.open("/home/amsl/m2_result/itst.txt", std::ios::out);

}

Eval::~Eval(){


}

void
Eval::scoreCallback(const std_msgs::Int32MultiArrayConstPtr &msgs){	

	better_score_num = *msgs;
	// better_score_num.data.erase(better_score_num.data.begin());
	
	cnt_ = 0;

	buffer_cnn.resize(NUM_CANDIDATE);
	buffer_context.resize(NUM_CANDIDATE);
	buffer_pose.resize(NUM_CANDIDATE);
	buffer_image.resize(NUM_CANDIDATE);
	sorted_map.clear();


	test_map1.clear();
	test_map2.clear();
	test_map3.clear();

}


void
Eval::itstCallback(const std_msgs::Float64MultiArrayConstPtr &msgs){	
	buffer_itst = *msgs;	
}

void
Eval::contextCallback(const std_msgs::Float64ConstPtr &msgs){
	buffer_context[cnt_] = 1.0-msgs->data;	
}

void
Eval::poseCallback(const geometry_msgs::PoseStampedConstPtr &msgs){	
	buffer_pose[cnt_] = *msgs;
}

void
Eval::cnnCallback(const std_msgs::Float64ConstPtr &msgs){	
	buffer_cnn[cnt_] = msgs->data;
	
	sorted_map.insert(std::make_pair(buffer_context[cnt_] + buffer_cnn[cnt_] + buffer_itst.data[cnt_], cnt_));
	// sorted_map.insert(std::make_pair(buffer_context[cnt_]*0 + buffer_cnn[cnt_]*0 + buffer_itst.data[cnt_], cnt_));

	test_map1.insert(std::make_pair(buffer_context[cnt_], cnt_));
	test_map2.insert(std::make_pair(buffer_cnn[cnt_], cnt_));
	test_map3.insert(std::make_pair(buffer_itst.data[cnt_], cnt_));


	if(cnt_==(NUM_CANDIDATE-1)){

		// image_pub1.publish(buffer_image[0]);
		// image_pub2.publish(buffer_image[1]);
		// image_pub3.publish(buffer_image[2]);
		// image_pub4.publish(buffer_image[3]);
		// image_pub5.publish(buffer_image[4]);
		test();

		int best_node = sorted_map.begin()->second;
		tf_pub(buffer_pose[best_node]);

		geometry_msgs::Pose p;
		p.position.x = buffer_pose[best_node].pose.position.x;
		p.position.y = buffer_pose[best_node].pose.position.y;
		p.orientation.z = buffer_pose[best_node].pose.orientation.z;
		
		answer_pub.publish(p);

		int i=0;	
		for(auto const& entry: sorted_map){	
			int node = entry.second;
			std::cout << i << "th Node Number:" << better_score_num.data[node] <<std::flush;
			std::cout<<"  "<<buffer_itst.data[node]<<" "<<buffer_context[node]<<" "<<buffer_cnn[node]<<std::endl;
			i++;
		}
		std::cout<<std::endl;

		buffer_cnn.resize(NUM_CANDIDATE);
		buffer_context.resize(NUM_CANDIDATE);
		buffer_pose.resize(NUM_CANDIDATE);
		sorted_map.clear();



		test_map1.clear();
		test_map2.clear();
		test_map3.clear();

	}
	else{
		std_msgs::Int32 rebest_num;
		rebest_num.data = better_score_num.data[cnt_+1];

		rebest_pub.publish(rebest_num);
	}
		
	cnt_++;
}


void
Eval::imageCallback(const sensor_msgs::ImageConstPtr &msgs){
	buffer_image[cnt_] = *msgs;
	// if(cnt_==0) image_pub1.publish(*msgs);
 	// else if(cnt_==1) image_pub2.publish(*msgs);
 	// else if(cnt_==2) image_pub3.publish(*msgs);
	// else if(cnt_==3) image_pub4.publish(*msgs);
	// else image_pub5.publish(*msgs);
}



void
Eval::tf_pub(geometry_msgs::PoseStamped now_pose){
	
	transform.setOrigin( tf::Vector3( 
				now_pose.pose.position.x, now_pose.pose.position.y, now_pose.pose.position.z) );
	tf::Quaternion q;
	q.setRPY(0, 0, now_pose.pose.orientation.z);	//roll, pitch, yaw

	transform.setRotation(q);
	br.sendTransform(tf::StampedTransform(
				transform, now_pose.header.stamp, "/map" , "/context_estimate"));

}

void 
Eval::test(){

	int best_node = sorted_map.begin()->second;

	writing_file
		<<buffer_pose[best_node].pose.position.x<<","
		<<buffer_pose[best_node].pose.position.y<<","
		<<buffer_pose[best_node].pose.orientation.z
		<<std::endl;


	best_node = test_map1.begin()->second;

	writing_file1
		<<buffer_pose[best_node].pose.position.x<<","
		<<buffer_pose[best_node].pose.position.y<<","
		<<buffer_pose[best_node].pose.orientation.z
		<<std::endl;


	best_node = test_map2.begin()->second;

	writing_file2
		<<buffer_pose[best_node].pose.position.x<<","
		<<buffer_pose[best_node].pose.position.y<<","
		<<buffer_pose[best_node].pose.orientation.z
		<<std::endl;


	best_node = test_map3.begin()->second;

	writing_file3
		<<buffer_pose[best_node].pose.position.x<<","
		<<buffer_pose[best_node].pose.position.y<<","
		<<buffer_pose[best_node].pose.orientation.z
		<<std::endl;

}
