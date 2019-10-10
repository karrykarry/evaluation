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
from sensor_msgs.msg import Image

#Cnn_classifier
import numpy as np
import tensorflow as tf
from keras.models import load_model


#cv2
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')
import cv2



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
    data = data / 255.0
    return data


class Image_checker:
    def __init__(self):
        self.msg_sub = rospy.Subscriber("/pf_score/image", Image, self.callback)
        # 事前に適当に呼び出しておく
        X = np.zeros((1, 21, 21, 3))
        model_path = "../model/learning-image.h5"
        self.model = load_model(model_path)
        
        self.model.predict(X)
        # print("a",model)
        print('image_predict start')

    def callback(self, data):

        image_np = image_to_numpy(data)
        image_np = image_np[:, :, ::-1]
        image_np = [image_np]
        image_np = np.asarray(image_np)
        predicted = self.model.predict_classes(image_np)
        
        for predict in predicted:
            print(predict)
    
        # print(image_np.shape)
        
    # def image_to_numpy(self, msg):
    #     if not msg.encoding == "bgr8":
    #         raise TypeError('Unrecognized encoding {}'.format(msg.encoding))
    
    #     dtype_class = np.uint8
    #     channels = 3
    #     dtype = np.dtype(dtype_class)
    #     dtype = dtype.newbyteorder('>' if msg.is_bigendian else '<')
    #     shape = (msg.height, msg.width, channels)
        
        
    #     data = np.fromstring(msg.data, dtype=dtype).reshape(shape)
    #     data.strides = (
    #             msg.step,
    #             dtype.itemsize * channels,
    #             dtype.itemsize
    #     )
    
    #     if channels == 1:
    #         data = data[...,0]
    #     return data


def main(args):
    ic = Image_checker()
    rospy.init_node('node_estimate_checker.py', anonymous=True)
    print("node_estimate_checker")
    try:
        rospy.spin()

    except KeyboardInterrupt:
        print("Shut down")

if __name__ == '__main__':
    main(sys.argv)

