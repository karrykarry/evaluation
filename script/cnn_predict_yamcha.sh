#!/bin/bash

Path="/home/amsl/ros_catkin_ws/src/master_thsis/evaluation/cnn_script/"

ssh -t -t yamcha<<EOF
export ROS_MASTER_URI="http://192.168.0.224:11311"
sleep 1s

cd $Path

/usr/bin/python3 ros_image_predict.py

sleep 1s
exit
EOF
