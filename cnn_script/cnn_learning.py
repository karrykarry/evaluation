
# python3 cnn_learning.py


import numpy as np
from keras.layers import Dense
from keras.models import Sequential
from sklearn.model_selection import train_test_split
from keras.layers import Dense, Dropout, Flatten, Conv2D, MaxPooling2D
from keras.optimizers import RMSprop
from keras.utils import to_categorical
from keras.callbacks import Callback, CSVLogger
from matplotlib import pyplot as plt

classes = 2
# data_size = 75 * 75 * 3
image_shape = (21, 21, 3)
data_load = '../model/learning-image.npz'


class PlotLosses(Callback):
    '''
    学習中のlossについてlive plotする
    '''

    def on_train_begin(self, logs={}):
        '''
        訓練開始時に実施
        '''
        self.epoch_cnt = 0      # epochの回数を初期化
        plt.axis([0, self.epochs, 0, 0.25])
        plt.ion()               # pyplotをinteractive modeにする

    def on_train_end(self, logs={}):
        '''
        訓練修了時に実施
        '''
        plt.ioff()              # pyplotのinteractive modeをoffにする
        plt.legend(['loss', 'val_loss'], loc='best')
        plt.show()

    def on_epoch_end(self, epoch, logs={}):
        '''
        epochごとに実行する処理
        '''
        loss = logs.get('loss')
        val_loss = logs.get('val_loss')
        x = self.epoch_cnt
        # epochごとのlossとval_lossをplotする
        plt.scatter(x, loss, c='b', label='loss')
        plt.scatter(x, val_loss, c='r', label='val_loss')
        plt.pause(0.05)
        # epoch回数をcount up
        self.epoch_cnt += 1


def plot_result(history):
    '''
    plot result
    全ての学習が終了した後に、historyを参照して、accuracyとlossをそれぞれplotする
    '''

    # accuracy
    plt.figure()
    plt.plot(history.history['acc'], label='acc', marker='.')
    plt.plot(history.history['val_acc'], label='val_acc', marker='.')
    plt.grid()
    plt.legend(loc='best')
    plt.title('accuracy')
    plt.savefig('./data/graph_accuracy.png')
    plt.show()

    # loss
    plt.figure()
    plt.plot(history.history['loss'], label='loss', marker='.')
    plt.plot(history.history['val_loss'], label='val_loss', marker='.')
    plt.grid()
    plt.legend(loc='best')
    plt.title('loss')
    plt.savefig('./data/graph_loss.png')
    plt.show()



def model_training(x, y, x_valid, y_valid, epochs, batch_size):
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
    # callback function
    plot_losses = PlotLosses()      # グラフ表示(live plot)
    plot_losses.epochs = epochs
    csv_logger = CSVLogger('../model/trainlog.csv')



    model = Sequential()
    model.add(Conv2D(32, kernel_size=(3, 3), activation='relu', input_shape=image_shape))
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
    model.add(Dense(classes, activation='softmax'))
    # モデルをコンパイル
    model.compile(loss='categorical_crossentropy',
                  optimizer=RMSprop(),
                  metrics=['accuracy'])
    
    print(model.summary())

    history = model.fit(x, y, 
            batch_size=batch_size, 
            epochs=epochs, 
            verbose=1, 
            validation_data=(x_valid, y_valid), 
            callbacks=[plot_losses, csv_logger])


    plot_result(history)

    return model
 
 
def model_evaluation(model, x_test, y_test):
    score = model.evaluate(x_test, y_test)
    print(score)
    print('Loss = ', score[0])
    print('Accuracy = ', score[1])
 
 
if __name__ == '__main__':
 
    try:
        # データの読み込み
        data_set = np.load(data_load)

        print(data_load, "を用いる")
        X = data_set["x"]
        Y = data_set["y"]
        # 2次元に変換
        # X = np.reshape(X, (-1, data_size))
        # print(X.shape) e.g. 162
        X_train, X_test, Y_train, Y_test = train_test_split(X, Y, train_size=0.9)
        # print(X_train.shape) e.g.145
        X_train1, X_valid, Y_train1, Y_valid = train_test_split(X_train, Y_train, test_size=0.175)
        # print(X_train1.shape) e.g. 119
        
        # Y_train = Y_train.reshape(len(Y_train.data), 2)
        Y_train1 = to_categorical(Y_train1)
        Y_test = to_categorical(Y_test)
        Y_valid = to_categorical(Y_valid)
        
        epochs = 20
        batch_size = 128

        print('epoch:', epochs)
        print('batch_size:', batch_size)

        model = model_training(X_train1, Y_train1, X_valid, Y_valid, epochs, batch_size)
        model_path = '../model/learning-image.h5'
        model.save(model_path)
        model_evaluation(model, X_test, Y_test)
    
        score = model.evaluate(X_test, Y_test, verbose=0)
        print('Test loss: {0}'.format(score[0]))
        print('Test accuracy: {0}'.format(score[1]))
        
        print(model_path, "にmodelを保存")
        
    except Exception as e:
        traceback.print_exc()



