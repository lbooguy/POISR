import numpy as np
import matplotlib.pyplot as plt


file_y = open("vector_lenght.txt", "r")
Y = []
while True:
    line = file_y.readline()
    if not line:
        break
    Y.append(float(eval(line)))
file_y.close


X = [i for i in range(len(Y))]

plt.title("Смещение")
#plt.legend()
plt.plot(X, Y)
plt.show()
