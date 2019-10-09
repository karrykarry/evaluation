#!/usr/bin/env python
# python3 predict.py

# from pathlib import Path
# import numpy as np
# from PIL import Image
# from keras.models import load_model

# import sys
# sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')




# from __future__ import print_function

#Image_checker
import roslib
# roslib.load_manifest('my_package')
import sys
import rospy
from std_msgs.msg import String
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError
import numpy as np

#Cnn_classifier
from keras.models import load_model

#cv2
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')
import cv2

image_shape = (280, 280, 3)


def image_to_numpy(msg):
    if not msg.encoding == "bgr8":
        raise TypeError('Unrecognized encoding {}'.format(msg.encoding))
    
    dtype_class = np.uint8
    channels = 3
    dtype = np.dtype(dtype_class)
    dtype = dtype.newbyteorder('>' if msg.is_bigendian else '<')
    shape = (msg.height, msg.width, channels)
    
    data = np.fromstring(msg.data, dtype=dtype).reshape(shape)
    data.strides = (
            msg.step,
            dtype.itemsize * channels,
            dtype.itemsize
    )
    
    if channels == 1:
        data = data[...,0]
    return data



class Cnn_classifier:
    def __init__(self):
        print("a")
        model_path = "../model/learning-image.h5"
        model = load_model(model_path)

class Image_checker:
    def __init__(self):
        self.msg_sub = rospy.Subscriber("/camera/rgb/resized_image", Image, self.callback)
        self.bridge = CvBridge()
        Cnn_classifier();

    def callback(self, data):

        image_np = image_to_numpy(data)
        
        # try:
        #     cv_image  = self.bridge.imgmsg_to_cv2(data, "bgr8")
        # except CvBridgeError as e:
        #     print (e)

        # img = self.crop_resized(cv_image)
        # cv2.imshow("Image window", img)
        # cv2.waitKey(1)

    # def crop_resized(self, image):
        # dst = cv2.resize(image, dsize=image_shape[:2])
        # image_np = np.asarray(dst)
        print(image_np.shape)
        
        # return dst


def main(args):
    ic = Image_checker()
    rospy.init_node('node_estimate_checker.py', anonymous=True)
    print("node_estimate_checker")
    try:
        rospy.spin()

    except KeyboardInterrupt:
        print("Shut down")
    cv2.destroyAllwindows()


if __name__ == '__main__':
    main(sys.argv)

