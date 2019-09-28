# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import numpy as np

data_hessian = np.loadtxt("hessian_time_lm2")   #将文件中数据加载到data数组里
data_solve = np.loadtxt("solve_time_lm2")



epoches_hessian=[i for i in range(len(data_hessian))]
epoches_solve=[i for i in range(len(data_solve))]

print(np.mean(data_hessian))
print(np.mean(data_solve))

ln1, = plt.plot(epoches_hessian, data_hessian,linestyle='dashed',linewidth=0.5,color='red',)
ln2, = plt.plot(epoches_solve, data_solve,linestyle='dashed',linewidth=0.5,color='blue',)

# plt.plot(epoches_solve, data_solve,linestyle='dashed',linewidth=0.5,color='blue',marker='o',)

plt.xlabel("epoches")
plt.ylabel("time_ms")

# plt.title("Lamda change with iteration")

plt.legend(handles=[ln1, ln2], labels=['makehessian_time', 'solve_time'], loc='lower right')

plt.show()

