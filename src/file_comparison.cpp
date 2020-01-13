//g++11 -o file_comparison file_comparison.cpp -isystem  /home/amsl/include/eigen



#include<iostream>
#include<fstream>
#include<sstream>
#include<string.h>
#include<vector>
#include<Eigen/Geometry> 

struct Pose{
    double x;
    double y;
    double yaw;
};

std::vector<std::string> split(const std::string &str, char sep)
{
	std::vector<std::string> v;
	std::stringstream ss(str);
	std::string buffer;
	while( getline(ss, buffer, sep) ) {
		v.push_back(buffer);
	}
	return v;
}



void file2vec(const std::string PR_list_filename, const int yaw_num, std::vector<Pose>& vec){

	std::ifstream reading_file;
	std::string reading_line_buffer;

	reading_file.open(PR_list_filename, std::ios::in);

	if(reading_file.fail()) {
		std::cerr<<"\033[1;32m File could not be opened "<< PR_list_filename << "\033[0m"<<std::endl;
		exit(1);
	}


	std::vector<std::string> v;	
	std::cout<<"------read "<<PR_list_filename<< "--------"<<std::endl;
	
	while (!reading_file.eof())
	{
		// read by line
		std::getline(reading_file, reading_line_buffer);
		if(!reading_line_buffer.empty()){
			v = split(reading_line_buffer,',');
			/* std::cout<<atof(v[0].c_str())<<","<<atof(v[1].c_str())<<","<<atof(v[yaw_num].c_str())<<std::endl; */

            Pose pose;
			pose.x = atof(v[0].c_str());
			pose.y = atof(v[1].c_str());
			pose.yaw = atof(v[yaw_num].c_str());	//yaw

			vec.push_back(pose);
		}
	}
	std::cout<<"\033[1;31minput --> "<< PR_list_filename <<" \033[0m"<<std::endl;

}

inline Eigen::Vector2f th2q(double yaw){
	Eigen::Vector2f output;
	output << cos(yaw), sin(yaw);
	/* std::cout<<"th2q :"<<output<<std::endl; */
	return output;
}


//角度の差を取る
double qdev(Eigen::Vector2f input1, Eigen::Vector2f input2){	
	
	double yaw1 = atan2(input1.y(), input1.x());
	double yaw2 = atan2(input2.y(), input2.x());
	
	Eigen::Rotation2Df rot((-1)*yaw2);

	Eigen::Vector2f input;
	input << cos(yaw1), sin(yaw1);
	
	Eigen::Vector2f ans = rot*input;
	//q2th
	/* std::cout<<"|theta1-theta2|: "<< fabs(atan2(ans.y(), ans.x())) * 180.0 / M_PI <<std::endl; */

    return  fabs(atan2(ans.y(), ans.x())) * 180.0 / M_PI; 
}

int main(void){

    std::string proposed_file;
    std::cout<<"Propose file name ?:"<<std::endl;
    std::cin >> proposed_file;
    std::vector<Pose> proposed_vec;
    
    file2vec(proposed_file + ".txt", 2, proposed_vec);

    std::string ndt_file;
    std::cout<<"NDT file name ?:"<<std::endl;
    std::cin >> ndt_file;
    std::vector<Pose> ndt_vec;
    
    file2vec(ndt_file + ".txt", 5, ndt_vec);



	std::ofstream writing_file;
	writing_file.open("./" + proposed_file + ".csv", std::ios::out);

    int file_size = proposed_vec.size();
    double yaw_diff;

    std::cout << "----- save now -----" << std::endl;
    for(int i=0;i<file_size;i++){
        writing_file << fabs(proposed_vec[i].x - ndt_vec[i].x)<<","<<std::flush;
    }
	writing_file <<std::endl;
    for(int i=0;i<file_size;i++){
        writing_file << fabs(proposed_vec[i].y - ndt_vec[i].y)<<","<<std::flush;
    }
	writing_file <<std::endl;


    for(int i=0;i<file_size;i++){
   
        Eigen::Vector2f qs, qe, qave;
        qs = th2q(proposed_vec[i].yaw);
        qe = th2q(ndt_vec[i].yaw);
        writing_file << qdev(qs, qe);
        writing_file<<","<<std::flush;
    }
    writing_file << std::endl;


}
