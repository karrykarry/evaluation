#!/bin/bash
if [ $# != 1 ]; then
    echo "Please set pc_sub name"
    exit
fi


ssh -t -t yamcha<<EOF
export ROS_MASTER_URI="http://192.168.0.224:11311"
sleep 1s

echo 'sub_name : ' $1
/opt/ros/kinetic/bin/roslaunch human_detection kari_human_detector.launch pc_sub:=$1

sleep 1s
exit
EOF
