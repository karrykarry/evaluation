#!/usr/bin/env python
# -*- coding: utf-8 -*-
import rospy
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import math

from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d import axes3d

from std_msgs.msg import Float64MultiArray
from std_msgs.msg import Empty 
    

def viewer(score_msg):

    plt.clf()
    plt.cla()
        
    x = range(0, len(score_msg.data))
        
    plt.title("score of PR ")
    plt.xlabel("place")
    plt.ylabel("Score")
    # plt.xlim(xmax = 30, xmin = 0)    
    # plt.ylim(ymax = 100, ymin = 0)    
    plt.bar(x, score_msg.data) 

    plt.draw()
    plt.pause(0.01)
    # print("draw")



class Score_viewer:
    def __init__(self):
        rospy.loginfo(rospy.get_caller_id())
        self.msg_sub = rospy.Subscriber("/score/vis/context", Float64MultiArray, self.callback)
        self.fig = plt.figure()
        plt.show()

    def callback(self, msg):
        # viewer(msg)
        side_len = np.sqrt(len(msg.data))
        self.value_x = np.arange(math.ceil((1-side_len)*(0.5)), math.ceil((side_len-1)*(0.5))+1)
        self.value_y = np.arange(math.ceil((1-side_len)*(0.5)), math.ceil((side_len-1)*(0.5))+1)
        data_list = list(msg.data)
        print ("success subscribed")
        self.graph_vis(data_list)

    def graph_vis(self, data):

        ax = self.fig.add_subplot(111, projection='3d')
        cs = ['y'] * len(self.value_y)

        
        num = 0;
        for x in self.value_x:
            height =  data[num : num+len(self.value_x)]
            # height = np.arange(num, num+len(self.value_x))
            # print height

            ax.bar(self.value_y, height, x, zdir='x', color=cs, alpha=0.8)
            num += len(self.value_x);

        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_zlabel('Z')

        plt.draw()
        plt.pause(0.1)


def main():
    rospy.init_node('score_view', anonymous=True)
    Score_viewer()
# while not rospy.is_shutdown():


    rospy.spin()




if __name__ == '__main__':
    main()


