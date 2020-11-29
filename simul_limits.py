# -*- coding: utf-8 -*-
#usar python3
import numpy as np
import matplotlib.pyplot as plt
# import sys
# import math


################################################
# Open the data file with the vectors to graph #
################################################
file = open ('data.txt', 'r')

###################
# Get the vectors #
###################
#readlines reads the individual line into a list
fl =file.readlines()
v1 = fl[0]
v1 = v1.strip('\n')

str_data = str(fl[1])
# print(str_data[0])
# print(len(str_data))
# print(type(str_data))
# print(sys.getsizeof(str_data))
# print(f"vector: {v1} size: {len(str_data)} first element: {str_data[0]}")

dmx_data = np.fromstring(str_data, dtype=np.uint16, sep=' ')
# print(dmx_data.size)
# print(dmx_data.dtype)
# print(dmx_data)
print(f"vector: {v1} numpy array size: {dmx_data.size} first element: {dmx_data[0]}")


# print(f"the name of the vector: {fl[2]}")
v2 = fl[2]
v2 = v2.strip('\n')

str_data = str(fl[3])
# print(str_data[0])
# print(len(str_data))
# print(type(str_data))
# print(sys.getsizeof(str_data))
pwm1_data = np.fromstring(str_data, dtype=np.uint16, sep=' ')
# print(pwm1_data.size)
# print(pwm1_data.dtype)
# print(pwm1_data)
print(f"vector: {v2} numpy array size: {pwm1_data.size} first element: {pwm1_data[0]}")

d3 = fl[4]
d3 = d3.strip('\n')
max_power = int(fl[5])
print(f"data: {d3} value: {max_power}")
close(file)
###########################
# Armo la senial temporal #
###########################
t = np.linspace(0, dmx_data.size, num=dmx_data.size)

fig, ax = plt.subplots()
ax.set_title('Mapeo de PWM como f(dmx_data)')
ax.set_ylabel('PWM')
ax.set_xlabel('Tiempo en muestras [ms]')
ax.grid()
ax.plot(t, dmx_data, 'y')
ax.plot(t, pwm1_data, 'b')
plt.tight_layout()
plt.show()


fig, ax = plt.subplots()
ax.set_title('Mapeo de PWM como f(dmx_data * 10)')
ax.set_ylabel('PWM')
ax.set_xlabel('Tiempo en muestras [ms]')
ax.grid()
ax.plot(t, dmx_data * 10, 'y')
ax.plot(t, pwm1_data, 'b')
plt.tight_layout()
plt.show()
