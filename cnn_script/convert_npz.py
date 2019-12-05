
# python3 convert_npz.py

import glob
import traceback

import numpy as np
import sys
from PIL import Image


photo_count = 2006

# 画像データ
X = []
# ラベル
Y = []

# path = '/home/amsl/Pictures/CNN/dataset-2019-09-25/'
path = '/home/amsl/Pictures/CNN/2018-09-09dataset/'

image_shape = (21, 21, 3)

def convert_image_to_npz(folder, label, count):
    images = glob.glob(path + folder + '/*.png')

    for i, image in enumerate(images):
        if i > count:
            break

        # 読み取りモードで読み取り
        image = Image.open(image, 'r')
        # RGBへ変換
        image = image.convert("RGB")

        length = min(image.size)
        crop = image.crop((0, 0, length, length))
        resized = crop.resize(image_shape[:2])  # use width x height
        img = np.array(resized).astype("float32")
        
        dataset = np.asarray(img)
        dataset = dataset / 256
        X.append(dataset)
        Y.append(label)

def convert_flag(num):
    flag_ = True;
    if num: 
        flag_ = False;
    return flag_;

if __name__ == '__main__':
    
    print(path, "下に置いているディレクトリを引数に取る")
    print(image_shape)

    if len(sys.argv) < 2:
        sys.exit("Usage: %s convert-npz.py <keyword1> <keyword2>" % sys.argv[0])

    try:
        output_file = '../model/learning-image'

        for i, key in enumerate(sys.argv[1:]):
            # convert_image_to_npz(key, i, photo_count)
            
            flag = convert_flag(i);
            print(flag ,":",key)
            convert_image_to_npz(key, flag, photo_count)

        X = np.array(X, dtype=np.float32)
        np.savez(output_file, x=X, y=Y)
    
        print(output_file, "に保存")

    except Exception as e:
        traceback.print_exc()
