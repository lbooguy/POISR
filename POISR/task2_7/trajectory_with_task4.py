import numpy as np
import matplotlib.pyplot as plt


def get_coord(X_path, Y_path):
    file_x = open(X_path, "r")
    file_y = open(Y_path, "r")

    X = []
    Y = []

    while True:
        line = file_x.readline()
        if not line:
            break
        X.append(float(eval(line)))
    file_x.close

    while True:
        line = file_y.readline()
        if not line:
            break
        Y.append(float(eval(line)))
    file_y.close

    return X,Y


    #file_x = open("X_7.txt", "r")
    #file_y = open("Y_7.txt", "r")

X1, Y1 = get_coord("X_7.txt", "Y_7.txt")
X2, Y2 = get_coord("X_4.txt", "Y_4.txt")

plt.title("Наложение траекторий")
#plt.legend()
plt.scatter(X1, Y1)
plt.scatter(X2, Y2)
plt.show()
