/*
 * /result/negative の判定されたらrecoverする
 *
 */


#include"recovery.hpp"
Recovery::Recovery(ros::NodeHandle n,ros::NodeHandle priv_nh):
	out_num(0)
{
	rebest_pub = n.advertise<std_msgs::Int32>("/score/best/itst", 10);
	go_pub = n.advertise<std_msgs::Bool>("/next_pcd",10);
	
	better_sub = n.subscribe<std_msgs::Int32MultiArray>("/score/better/itst", 1, &Recovery::scoreCallback, this);

	process_sub = n.subscribe<std_msgs::Empty>("/result/negative", 1, &Recovery::processCallback, this);
}

Recovery::~Recovery(){

	std::cout<<"\033[1;32m The number od out is "<<out_num<<" \033[0m"<<std::endl;	

}

void
Recovery::scoreCallback(const std_msgs::Int32MultiArrayConstPtr &msgs){	

	better_score_num = *msgs;
	better_score_num.data.erase(better_score_num.data.begin());
	
	cnt_ = 1;
}




void
Recovery::processCallback(const std_msgs::EmptyConstPtr &msgs){

	if(!(better_score_num.data.size())){
		std::cout<<"\033[1;32m out!!!! \033[0m"<<std::endl;	
		std_msgs::Bool em;
		em.data = true;
		go_pub.publish(em);
		out_num++;
	}

	else{
		std_msgs::Int32 rebest_num;
		rebest_num.data = better_score_num.data[0];

		better_score_num.data.erase(better_score_num.data.begin());

		std::cout<<"\033[1;32m"<<cnt_<<"th Number:"<<rebest_num.data<<" publish \033[0m"<<std::endl;

		rebest_pub.publish(rebest_num);
		cnt_++;
	}	
}	
