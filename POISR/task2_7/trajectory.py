import numpy as np
import matplotlib.pyplot as plt


file_x = open("X_7.txt", "r")
file_y = open("Y_7.txt", "r")

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
plt.title("Траектория")
#plt.legend()
plt.scatter(X, Y)
plt.show()
