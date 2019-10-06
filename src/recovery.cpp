

#include"recovery.hpp"
Recovery::Recovery(ros::NodeHandle n,ros::NodeHandle priv_nh)
{
	rebest_pub = n.advertise<std_msgs::Int32>("/score/best/itst", 10);
	
	better_sub = n.subscribe<std_msgs::Int32MultiArray>("/score/better/itst", 1, &Recovery::scoreCallback, this);

	process_sub = n.subscribe<std_msgs::Empty>("/process", 1, &Recovery::processCallback, this);
}

void
Recovery::scoreCallback(const std_msgs::Int32MultiArrayConstPtr &msgs){	

	better_score_num = *msgs;
	
}

void
Recovery::processCallback(const std_msgs::EmptyConstPtr &msgs){

	std_msgs::Int32 rebest_num;
	rebest_num.data = better_score_num.data[0];

	better_score_num.data.erase(better_score_num.data.begin());

	std::cout<<"\033[1;32m Node Number:"<<rebest_num.data<<" publish \033[0m"<<std::endl;

	rebest_pub.publish(rebest_num);

}
