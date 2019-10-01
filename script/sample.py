#!/usr/bin/env python
import rospy
import numpy as np
import matplotlib
import matplotlib.pyplot as plt 

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

def callback(msg):
    viewer(msg)


def listen():
    print("listen")
    # rospy.Subscriber("/message", Float64MultiArray, callback)
    rospy.Subscriber("/score/pf_vis", Float64MultiArray, callback)
    print("subscribed")
    rospy.spin()

def main():
    rospy.init_node('score_viewer', anonymous=True)
    listen()


if __name__ == '__main__':
    main()

