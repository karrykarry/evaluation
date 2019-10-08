
# python3 image_learning.py


import numpy as np
from keras.layers import Dense
from keras.models import Sequential
from sklearn.model_selection import train_test_split
from keras.layers import Dense, Dropout, Flatten, Conv2D, MaxPooling2D
from keras.optimizers import RMSprop
from keras.utils import to_categorical

classes = 2
data_size = 75 * 75 * 3
 
 
def model_training(x, y):
    # model = Sequential()
    # # 隠れ層:64、入力層:データサイズ、活性化関数:Relu
    # model.add(Dense(units=64, activation='relu', input_dim=(data_size)))
    # # 出力層:分類するクラス数、活性化関数:Softmax
    # model.add(Dense(units=classes, activation='softmax'))
    # # モデルをコンパイル
    # model.compile(loss='sparse_categorical_crossentropy',
    #               optimizer='sgd',
    #               metrics=['accuracy'])
    # model.fit(x, y, epochs=60)



    model = Sequential()
    model.add(Conv2D(32, kernel_size=(3, 3), activation='relu', input_shape=(75, 75, 3)))
    model.add(Conv2D(64, (3, 3), activation='relu'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Dropout(0.25))
    model.add(Conv2D(64, (3, 3), activation='relu'))
    model.add(Conv2D(64, (3, 3), activation='relu'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Flatten())
    model.add(Dense(256, activation='relu'))
    model.add(Dense(128, activation='relu'))
    model.add(Dropout(0.5))
    model.add(Dense(2, activation='softmax'))
    # モデルをコンパイル
    model.compile(loss='categorical_crossentropy',
                  optimizer=RMSprop(),
                  metrics=['accuracy'])
    model.fit(x, y, batch_size=128, epochs=15, verbose=1)
    
    return model
 
 
def model_evaluation(model, x_test, y_test):
    score = model.evaluate(x_test, y_test)
    print(score)
    print('Loss = ', score[0])
    print('Accuracy = ', score[1])
 
 
if __name__ == '__main__':
 
    try:
        # データの読み込み
        data_set = np.load('./learning-image-300.npz')
        X = data_set["x"]
        Y = data_set["y"]
        # 2次元に変換
        # X = np.reshape(X, (-1, data_size))
        X_train, X_test, Y_train, Y_test = train_test_split(X, Y, train_size=0.9)
        
        print(X_train.shape)
        print(Y_train.shape)
        # Y_train = Y_train.reshape(len(Y_train.data), 2)
        Y_train = to_categorical(Y_train)
        print(Y_train.shape)
        Y_test = to_categorical(Y_test)
        print(Y_test.shape)
    
        model = model_training(X_train, Y_train)
        model.save('../model/learning-image.h5')
        model_evaluation(model, X_test, Y_test)

        print("Xsize :", X_test.shape)
        predicted = model.predict_classes(X_test)
        
    except Exception as e:
        traceback.print_exc()



