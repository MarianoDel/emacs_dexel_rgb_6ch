# -*- coding: utf-8 -*-
#usar python3
import numpy as np
import matplotlib.pyplot as plt
from sympy import *
# import math
from scipy.signal import lti, bode, lsim, TransferFunction, step, step2
from scipy.signal import cont2discrete, dbode



#########################################################
# Transfer Function equation for the output voltage and #
# the output current.                                   #
#########################################################
# b_param = 0.02
# a_param = 0.98
b_param = 0.1
a_param = 0.9
# b_param = 0.8
# a_param = 0.2

Fsampling = 24e3
td = 1 / Fsampling

################################################
# Respuesta escalon de la planta punto a punto #
# entrando con Duty propuesto como escalon     #
################################################
tiempo_de_simulacion = 0.010
print('td:')
print (td)
t = np.arange(0, tiempo_de_simulacion, td)

########################
# Armo la senial input #
########################
vin = np.ones(t.size) * 2000
vin[:int(t.size/10)] = 0
vin[t.size-int(t.size/10):] = 0
vout = np.zeros(t.size)

for i in range(1, len(vin)):
    ######################################
    # aplico la transferencia del filtro #
    ######################################
    vout[i] = b_param*vin[i] \
              + a_param*vout[i-1]

        
fig, ax = plt.subplots()
ax.set_title('IIR Respuesta del Filtro')
ax.set_ylabel('Vout')
ax.set_xlabel('Tiempo en muestras')
ax.grid()
ax.plot(t, vin, 'y')
ax.plot(t, vout, 'b')
# ax.stem(t, vout, 'b')
plt.tight_layout()
plt.show()


# for i in range(15, len(vin)):
#     ######################################
#     # aplico la transferencia del filtro #
#     ######################################
#     vout[i] = vin[i] + vin[i-1] + vin[i-2] + vin[i-3] \
#               + vin[i-4] + vin[i-5] + vin[i-6] + vin[i-7] \
#               + vin[i-8] + vin[i-9] + vin[i-10] + vin[i-11] \
#               + vin[i-12] + vin[i-13] + vin[i-14] + vin[i-15]              

#     vout[i] = vout[i] / 16.0

        
# fig, ax = plt.subplots()
# ax.set_title('MA Respuesta del Filtro')
# ax.set_ylabel('Vout')
# ax.set_xlabel('Tiempo en muestras')
# ax.grid()
# ax.plot(t, vin, 'y')
# ax.plot(t, vout, 'b')
# # ax.stem(t, vout, 'b')
# plt.tight_layout()
# plt.show()





