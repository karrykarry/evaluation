
# python3 image_pre.py

import glob
import traceback

import numpy as np
import sys
from PIL import Image


photo_count = 80

# 画像データ
X = []
# ラベル
Y = []

image_shape = (75, 75, 3)

def convert_image_to_npz(folder, label, count):
    images = glob.glob('/home/amsl/Pictures/sample/' + folder + '/*.jpg')

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


if __name__ == '__main__':

    if len(sys.argv) < 2:
        sys.exit("Usage: %s convert-npz.py <keyword1> <keyword2>" % sys.argv[0])

    try:
        output_file = 'learning-image-' + str(photo_count)

        for i, key in enumerate(sys.argv[1:]):
            print(i ,":",key)
            convert_image_to_npz(key, i, photo_count)

        X = np.array(X, dtype=np.float32)
        np.savez(output_file, x=X, y=Y)

    except Exception as e:
        traceback.print_exc()
