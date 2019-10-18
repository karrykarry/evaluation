
# python3 predict.py

from pathlib import Path
import numpy as np
from PIL import Image
from keras.models import load_model

import sys
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')

import cv2


model_path = "../model/learning-image.h5"
# images_folder = "/home/amsl/Pictures/dataset/1/"
images_folder = "/home/amsl/Pictures/2018/2/"

# load model
model = load_model(model_path)
# image_shape = (28, 28, 1)
image_shape = (21, 21, 3)
data_size = 75 * 75 * 3


# load images
def crop_resize(image_path):
    # img_gbr = cv2.imread(image_path)
    # img_gray = cv2.cvtColor(img_gbr, cv2.COLOR_BGR2GRAY)

    img_gray = Image.open(image_path)
    # img_gray = image_.convert('L')
    length = min(img_gray.size)
    crop = img_gray.crop((0, 0, length, length))
    resized = crop.resize(image_shape[:2])  # use width x height
    img = np.array(resized).astype("float32")
    # print(image_.size)
    # print(img.shape)
    # img = np.array(img_gray).astype("float32")
    img /= 255
    # img_ = img.reshape(data_size)
    img = img.reshape(image_shape)
    return img


folder = Path(images_folder)
image_paths = [str(f) for f in folder.glob("*.png")]
images = [crop_resize(p) for p in image_paths]
images = np.asarray(images)
# print(images[0])
predicted = model.predict_classes(images)
probas = model.predict_proba(images)

print("\n")
print("-----result-----\n")
sum = 0;
# print(predicted)
for image_path, predict, proba in zip(image_paths, predicted, probas):
    sum += predict
    print(image_path,  predict, proba, max(proba))

print("sum:", sum)

