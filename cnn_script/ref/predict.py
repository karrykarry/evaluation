
# python3 predict.py

from pathlib import Path
import numpy as np
from PIL import Image
from keras.models import load_model

import sys
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')

import cv2


model_path = "../model/mnist_model.h5"
images_folder = "/home/amsl/Pictures/sample/"

# load model
model = load_model(model_path)
image_shape = (28, 28, 1)


# load images
def crop_resize(image_path):
    # img_gbr = cv2.imread(image_path)
    # img_gray = cv2.cvtColor(img_gbr, cv2.COLOR_BGR2GRAY)

    image_ = Image.open(image_path)
    img_gray = image_.convert('L')
    length = min(img_gray.size)
    crop = img_gray.crop((0, 0, length, length))
    resized = crop.resize(image_shape[:2])  # use width x height
    img = np.array(resized).astype("float32")
    print(length)
    print(image_.size)
    print(img.shape)
    # img = np.array(img_gray).astype("float32")
    img /= 255
    img = img.reshape(28, 28, 1)
    return img


folder = Path(images_folder)
image_paths = [str(f) for f in folder.glob("*.jpg")]
images = [crop_resize(p) for p in image_paths]
images = np.asarray(images)

print("a",images.shape)

predicted = model.predict_classes(images)

print("\n")
print("-----result-----\n")
for image_path, predict in zip(image_paths, predicted):
    print(image_path,  predict)

