#!/usr/bin/env python
import roslib
# roslib.load_manifest('my_package')
import sys
import rospy
import cv2
from std_msgs.msg import String
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError


image_shape = (21, 21, 1)

class Image_checker:
    def __init__(self):
        self.msg_sub = rospy.Subscriber("/pf_score/image", Image, self.callback)
        self.bridge = CvBridge()

    def callback(self, data):
        try:
            cv_image  = self.bridge.imgmsg_to_cv2(data, "bgr8")
        except CvBridgeError as e:
            print (e)

        img = self.crop_resized(cv_image)
        cv2.imshow("Image window", img)
        cv2.waitKey(1)

    def crop_resized(self, image):
        # length = min(image.size)
        # crop = image.crop((0, 0, length, length))
        # resized = crop.resize(image_shape[:2])  # use width x height
        dst = cv2.resize(image, dsize=image_shape[:2])
        # img = np.array(resized).astype("float32")
        # print(img.shape)
        
        return dst
        


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

