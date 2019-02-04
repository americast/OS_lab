import numpy as np
import matplotlib.pyplot as plt
import json
import os
import pudb

os.system("rm result")
os.system("g++ a3_42.cpp && ./a.out < in")

f = open("result", "r")

a_10 = np.array([0.0, 0.0, 0.0, 0.0, 0.0])
a_50 = np.array([0.0, 0.0, 0.0, 0.0, 0.0])
a_100 = np.array([0.0, 0.0, 0.0, 0.0, 0.0])

for i in xrange(10):
	line = f.readline()
	print line
	a_10 += np.array(json.loads(line))


for i in xrange(10):
	line = f.readline()
	print line
	a_50 += np.array(json.loads(line))


for i in xrange(10):
	line = f.readline()
	print line
	a_100 += np.array(json.loads(line))

a_10 /= 10
a_50 /= 10
a_100 /= 10

y_0 = [a_10[0], a_50[0], a_100[0]] 
y_1 = [a_10[1], a_50[1], a_100[1]] 
y_2 = [a_10[2], a_50[2], a_100[2]] 
y_3 = [a_10[3], a_50[3], a_100[3]] 
y_4 = [a_10[4], a_50[4], a_100[4]] 

x = [10, 50, 100] 

plt.plot(x,y_0, label="FCFS")
plt.plot(x,y_1, label="SJF")
plt.plot(x,y_2, label="P-SJF")
plt.plot(x,y_3, label="RR")
plt.plot(x,y_4, label="HRN")
plt.xlabel("Number of processes")
plt.ylabel("Average turnout times")
plt.legend()
plt.xticks(x)
plt.yticks(np.append(y_0, [y_1, y_2, y_3, y_4]))
plt.gca().yaxis.grid(True)
plt.gca().xaxis.grid(True)
# pu.db
# import matplotlib
# matplotlib.rc('xtick', labelsize=20) 
plt.rc('ytick', labelsize=3)

plt.show()
