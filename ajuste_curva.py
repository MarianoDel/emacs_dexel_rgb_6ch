# usar python3
import numpy as np
import matplotlib.pyplot as plt


"""
        Ajuste de curva corriente medida en puntos pwm = f(dmx)
"""

def Search_segment (dmx_sgm, new_value):
    # busco el segmento
    for i in range(np.size(dmx_sgm)):
        if new_value <= dmx_sgm[i]:
            return i

    return 15

        
def Mapping_each_sgm (pwm_sgm, dmx_sgm, min_value, new_value):
    # busco el segmento
    curr_sgm = Search_segment(dmx_sgm, new_value)

    # para el primer segmento tengo en cuenta los minimos
    if curr_sgm == 0:
        if new_value == 0:
            return 0

        # deltas
        delta_y = pwm_sgm[curr_sgm] - min_value
        delta_x = dmx_sgm[curr_sgm]

        res = delta_y * new_value
        res = res / delta_x
        res = res + min_value
        return int(res)
        
    else:
        # delta y
        delta_y = pwm_sgm[curr_sgm] - pwm_sgm[curr_sgm - 1]

        # delta x siempre 16, llevo el punto al eje
        delta_x = dmx_sgm[curr_sgm] - dmx_sgm[curr_sgm - 1]

        # nuevo punto al eje
        new_value = new_value - dmx_sgm[curr_sgm - 1]

        res = delta_y * new_value
        res = res / delta_x
        res = res + pwm_sgm[curr_sgm - 1]
        return int(res)


def Mapping_change_mode (pwm_sgm, dmx_sgm, ch_mode_sgm, min_value, new_value):
    # busco el segmento
    curr_sgm = Search_segment(dmx_sgm, new_value)

    # todos los segmentos debajo del ch_mode_sgm
    if curr_sgm < ch_mode_sgm:
        
        if new_value == 0:
            return 0

        # deltas
        delta_y = pwm_sgm[ch_mode_sgm - 1] - min_value
        delta_x = dmx_sgm[ch_mode_sgm - 1]

        res = delta_y * new_value
        res = res / delta_x
        res = res + min_value
        return int(res)

    # el segmento del cambio
    elif curr_sgm == ch_mode_sgm:
        # delta y
        delta_y = pwm_sgm[ch_mode_sgm] - pwm_sgm[ch_mode_sgm - 1]

        # delta x siempre 16, llevo el punto al eje
        delta_x = dmx_sgm[ch_mode_sgm] - dmx_sgm[ch_mode_sgm - 1]

        # nuevo punto al eje
        new_value = new_value - dmx_sgm[ch_mode_sgm - 1]

        res = delta_y * new_value
        res = res / delta_x
        res = res + pwm_sgm[ch_mode_sgm - 1]
        return int(res)

    # los segmentos superiores al del cambio
    else:
        # deltas
        delta_y = pwm_sgm[15] - pwm_sgm[ch_mode_sgm]
        delta_x = dmx_sgm[15] - dmx_sgm[ch_mode_sgm]

        # nuevo punto al eje
        new_value = new_value - dmx_sgm[ch_mode_sgm]

        res = delta_y * new_value
        res = res / delta_x
        res = res + pwm_sgm[ch_mode_sgm]
        return int(res)



# dmx posible inputs        
dmx_segments = np.asarray([15, 31, 47, 63, \
                           79, 95, 111, 127, \
                           143, 159, 175, 191, \
                           207, 223, 239, 255])

# white measurements
white_segments = np.asarray([1403, 1800, 2269, 2609, \
                             2963, 3300, 3608, 3899, \
                             4202, 4510, 4555, 4599, \
                             4642, 4685, 4725, 4766])

ch_mode_sgm = 9
white_minimun = 140

# all channels minimun
ch_minimun_values = np.asarray([46, 150, 150, 140, 140, 140])


# full_white_outputs = np.zeros_like(color_segments)
full_dmx_input = np.arange(256)

full_dmx_output_white_map_change_mode = np.zeros_like(full_dmx_input)
full_dmx_output_white_map_each_sgm = np.zeros_like(full_dmx_input)

for i in range(256):
    full_dmx_output_white_map_change_mode[i] = Mapping_change_mode(white_segments, dmx_segments, ch_mode_sgm, white_minimun, i)

    full_dmx_output_white_map_each_sgm[i] = Mapping_each_sgm(white_segments, dmx_segments, white_minimun, i)



print ('Mapping each 15')
print (Mapping_each_sgm(white_segments, dmx_segments, white_minimun, 15))

print ('Mapping each 31')
print (Mapping_each_sgm(white_segments, dmx_segments, white_minimun, 31))

print ('Mapping ch_mode 15')
print (Mapping_change_mode(white_segments, dmx_segments, ch_mode_sgm, white_minimun, 15))

print ('Mapping ch_mode 31')
print (Mapping_change_mode(white_segments, dmx_segments, ch_mode_sgm, white_minimun, 31))


fig, ax = plt.subplots()
ax.set_title('pwm = f(dmx), Blue mapeo completo, Green con el segmento de cambio')
ax.plot(full_dmx_input, full_dmx_output_white_map_each_sgm, 'b')
ax.plot(full_dmx_input, full_dmx_output_white_map_change_mode, 'g')
plt.show()

white_each = np.diff(full_dmx_output_white_map_each_sgm)
white_change = np.diff(full_dmx_output_white_map_change_mode)
in_x = np.arange(np.size(white_each))

fig, ax = plt.subplots()
ax.set_title('differences, Blue mapeo completo, Green con el segmento de cambio')
ax.plot(in_x, white_each, 'b')
ax.plot(in_x, white_change, 'g')
plt.show()


