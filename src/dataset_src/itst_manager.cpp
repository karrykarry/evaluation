/*
 * cnnのdatasetをつくるためのソース
 *
 */

#include"itst_manager.hpp"

Dataset_itst::Dataset_itst(ros::NodeHandle n,ros::NodeHandle priv_nh)
{
	rebest_pub = n.advertise<std_msgs::Int32>("/score/best/itst", 10);
	go_pub = n.advertise<std_msgs::Bool>("/next_pcd",10);
	
	better_sub = n.subscribe<std_msgs::Int32MultiArray>("/score/better/itst", 1, &Dataset_itst::scoreCallback, this);

	process_sub = n.subscribe<std_msgs::Empty>("/process", 1, &Dataset_itst::processCallback, this);
}

void
Dataset_itst::scoreCallback(const std_msgs::Int32MultiArrayConstPtr &msgs){	

	better_score_num = *msgs;
	buffer_num = better_score_num.data[0];
}

void
Dataset_itst::processCallback(const std_msgs::EmptyConstPtr &msgs){
	static int cnt_ = 0;

	std_msgs::Int32 rebest_num;
	do{
		better_score_num.data.erase(better_score_num.data.begin());
		if(!better_score_num.data.size()) break;
		rebest_num.data = better_score_num.data[0];

		// better_score_num.data.erase(better_score_num.data.begin());
		cnt_++;
	}while(fabs(rebest_num.data - buffer_num) < 10);

	
	if(!better_score_num.data.size()){
		std::cout<<"next pcd"<<std::endl;
		std_msgs::Bool em;
		cnt_=0;
		em.data = true;
		go_pub.publish(em);
	}
	else{
		std::cout<<"\033[1;32m"<<cnt_<<"th Number:"<<rebest_num.data<<" publish \033[0m"<<std::endl;
		rebest_pub.publish(rebest_num);
	}
}

