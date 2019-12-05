/*
 * /result/negative の判定されたらrecoverする
 *
 */


#include"evaluation_function.hpp"
Eval::Eval(ros::NodeHandle n,ros::NodeHandle private_nh_)
{
	rebest_pub = n.advertise<std_msgs::Int32>("/score/best/itst", 10);	
	answer_pub = n.advertise<geometry_msgs::Pose>("/map2context_result",10);
	
	node_sub = n.subscribe<std_msgs::Int32MultiArray>("/score/better/itst", 1, &Eval::scoreCallback, this);

	itst_sub = n.subscribe<std_msgs::Float64MultiArray>("/eval_score/itst", 1, &Eval::itstCallback, this);

	context_sub = n.subscribe<std_msgs::Float64>("/eval_score/context", 1, &Eval::contextCallback, this);
	cnn_sub = n.subscribe<std_msgs::Float64>("/eval_score/cnn", 1, &Eval::cnnCallback, this);
	pose_sub = n.subscribe<geometry_msgs::Pose>("/map2context_result_", 1, &Eval::poseCallback, this);

	private_nh_.param("Number_of_candidate", NUM_CANDIDATE, {5});

	buffer_cnn.resize(NUM_CANDIDATE);
	buffer_context.resize(NUM_CANDIDATE);
	buffer_pose.resize(NUM_CANDIDATE);
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
	sorted_map.clear();
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
Eval::poseCallback(const geometry_msgs::PoseConstPtr &msgs){	
	buffer_pose[cnt_] = *msgs;
}

void
Eval::cnnCallback(const std_msgs::Float64ConstPtr &msgs){	
	buffer_cnn[cnt_] = msgs->data;
	
	sorted_map.insert(std::make_pair(buffer_context[cnt_] + buffer_cnn[cnt_] + buffer_itst.data[cnt_], cnt_));

	if(cnt_==(NUM_CANDIDATE-1)){

		int best_node = sorted_map.begin()->second;
		answer_pub.publish(buffer_pose[best_node]);

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
	
	}
	else{
		std_msgs::Int32 rebest_num;
		rebest_num.data = better_score_num.data[cnt_+1];

		rebest_pub.publish(rebest_num);
	}
		
	cnt_++;
}

