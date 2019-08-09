//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DSP.C #################################
//---------------------------------------------

#include "dsp.h"


#include <stdlib.h>
#include <math.h>


/* Externals variables ---------------------------------------------------------*/


/* Global variables ---------------------------------------------------------*/
//------- de los PID ---------
#ifdef USE_PID_CONTROLLERS
int acc = 0;
short error_z1 = 0;
short error_z2 = 0;
short d_last = 0;
#endif

/* Module Definitions ---------------------------------------------------------*/
// #define PID_CONSTANT_DIVIDER    10    //todos se dividen por 1024
// #define PID_CONSTANT_DIVIDER    8    //todos se dividen por 256
#define PID_CONSTANT_DIVIDER    7    //todos se dividen por 128
// #define PID_CONSTANT_DIVIDER    6    //todos se dividen por 64

//from microinverter01.py
// #define KPV	14    //0.108
// #define KIV	11    //0.08333
// #define KDV	0

//estos funcionan bastante bien para tension en vacio, prende apaga alrededor de 100V
//usan divisor por 128, ajusta en 35.6V, este con carga ajusta mejor 34.3 a 35.6
#define KPV    5    //kp_dig = 0.039
// #define KIV    3    //ki_dig = 0.023, ajusta pero tiene bastante error 42pts
#define KIV    128    //ki_dig = 0.023, ajusta pero tiene bastante error 42pts
#define KDV    0

//estos funcionan bastante bien para tension en vacio, prende apaga alrededor de 80V
//usan divisor por 128, ajustan en 34.3V, ajusta muy rapido diferentes cambios de tension
//con carga no ajusta tan bien 32.3 a 34.6
// #define KPV	19    //kp_dig = 0.15
// #define KIV	1    //ki_dig = 0.0078
// #define KDV	182    //kd_dig = 1.42

// #define KPV	2    //kp_dig = 0.01
// #define KIV	1    //ki_dig = 0.0000416
// #define KDV	24    //kd_dig = 0.024


#define K1V (KPV + KIV + KDV)
#define K2V (KPV + KDV + KDV)
#define K3V (KDV)

/* Module functions ---------------------------------------------------------*/

unsigned short RandomGen (unsigned int seed)
{
	unsigned int random;

	//Random Generator
	srand (seed);
	random = rand();

	return (unsigned short) random;

}


unsigned short MAFilterFast (unsigned short new_sample, unsigned short * vsample)
{
    unsigned int total_ma;

    total_ma = new_sample + *(vsample) + *(vsample + 1) + *(vsample + 2) +
        *(vsample + 3) + *(vsample + 4) + *(vsample + 5) + *(vsample + 6);
    
    *(vsample + 7) = *(vsample + 6);
    *(vsample + 6) = *(vsample + 5);
    *(vsample + 5) = *(vsample + 4);
    *(vsample + 4) = *(vsample + 3);
    *(vsample + 3) = *(vsample + 2);        
    *(vsample + 2) = *(vsample + 1);
    *(vsample + 1) = *(vsample);
    *(vsample) = new_sample;

    return (unsigned short) (total_ma >> 3);
}


unsigned short MAFilterFast16 (unsigned short new_sample, unsigned short * vsample)
{
    unsigned int total_ma;

    total_ma = new_sample + *(vsample) + *(vsample + 1) + *(vsample + 2) +
        *(vsample + 3) + *(vsample + 4) + *(vsample + 5) + *(vsample + 6) +
        *(vsample + 7) + *(vsample + 8) + *(vsample + 9) + *(vsample + 10) +
        *(vsample + 11) + *(vsample + 12) + *(vsample + 13) + *(vsample + 14);        

    *(vsample + 15) = *(vsample + 14);        
    *(vsample + 14) = *(vsample + 13);    
    *(vsample + 13) = *(vsample + 12);
    *(vsample + 12) = *(vsample + 11);
    *(vsample + 11) = *(vsample + 10);
    *(vsample + 10) = *(vsample + 9);
    *(vsample + 9) = *(vsample + 8);        
    *(vsample + 8) = *(vsample + 7);    
    *(vsample + 7) = *(vsample + 6);
    *(vsample + 6) = *(vsample + 5);
    *(vsample + 5) = *(vsample + 4);
    *(vsample + 4) = *(vsample + 3);
    *(vsample + 3) = *(vsample + 2);        
    *(vsample + 2) = *(vsample + 1);
    *(vsample + 1) = *(vsample);
    *(vsample) = new_sample;

    return (unsigned short) (total_ma >> 4);
}


void SetNewValueInVector (unsigned short new_sample, unsigned short * vsample)
{
    *(vsample + 7) = *(vsample + 6);
    *(vsample + 6) = *(vsample + 5);
    *(vsample + 5) = *(vsample + 4);
    *(vsample + 4) = *(vsample + 3);
    *(vsample + 3) = *(vsample + 2);
    *(vsample + 2) = *(vsample + 1);
    *(vsample + 1) = *(vsample);
    *(vsample) = new_sample;
}


//unsigned short MAFilter8 (unsigned short new_sample, unsigned short * vsample)
unsigned short MAFilter8 (unsigned short * vsample)
{
    unsigned int total_ma;

    total_ma = *(vsample) + *(vsample + 1) +
        *(vsample + 2) + *(vsample + 3) +
        *(vsample + 4) + *(vsample + 5) +
        *(vsample + 6) + *(vsample + 7);

    return (unsigned short) (total_ma >> 3);
}


unsigned short MAFilter32 (unsigned short new_sample, unsigned short * vsample)
{
	unsigned short total_ma;

	total_ma = new_sample + *(vsample) + *(vsample + 1) + *(vsample + 2) + *(vsample + 3) + *(vsample + 4) + *(vsample + 5) + *(vsample + 6);
	total_ma += *(vsample + 7) + *(vsample + 8) + *(vsample + 9) + *(vsample + 10) + *(vsample + 11) + *(vsample + 12) + *(vsample + 13) + *(vsample + 14);
	total_ma += *(vsample + 15) + *(vsample + 16) + *(vsample + 17) + *(vsample + 18) + *(vsample + 19) + *(vsample + 20) + *(vsample + 21) + *(vsample + 22);
	total_ma += *(vsample + 23) + *(vsample + 24) + *(vsample + 25) + *(vsample + 26) + *(vsample + 27) + *(vsample + 28) + *(vsample + 29) + *(vsample + 30);

	*(vsample + 30) = *(vsample + 29);
	*(vsample + 29) = *(vsample + 28);
	*(vsample + 28) = *(vsample + 27);
	*(vsample + 27) = *(vsample + 26);
	*(vsample + 26) = *(vsample + 25);
	*(vsample + 25) = *(vsample + 24);
	*(vsample + 24) = *(vsample + 23);

	*(vsample + 23) = *(vsample + 22);
	*(vsample + 22) = *(vsample + 21);
	*(vsample + 21) = *(vsample + 20);
	*(vsample + 20) = *(vsample + 19);
	*(vsample + 19) = *(vsample + 18);
	*(vsample + 18) = *(vsample + 17);
	*(vsample + 17) = *(vsample + 16);
	*(vsample + 16) = *(vsample + 15);

	*(vsample + 15) = *(vsample + 14);
	*(vsample + 14) = *(vsample + 13);
	*(vsample + 13) = *(vsample + 12);
	*(vsample + 12) = *(vsample + 11);
	*(vsample + 11) = *(vsample + 10);
	*(vsample + 10) = *(vsample + 9);
	*(vsample + 9) = *(vsample + 8);
	*(vsample + 8) = *(vsample + 7);

	*(vsample + 7) = *(vsample + 6);
	*(vsample + 6) = *(vsample + 5);
	*(vsample + 5) = *(vsample + 4);
	*(vsample + 4) = *(vsample + 3);
	*(vsample + 3) = *(vsample + 2);
	*(vsample + 2) = *(vsample + 1);
	*(vsample + 1) = *(vsample);
	*(vsample) = new_sample;

	return total_ma >> 5;
}


//Filtro circular, recibe
//new_sample, p_vec_samples: vector donde se guardan todas las muestras
//p_vector: puntero que recorre el vector de muestras, p_sum: puntero al valor de la sumatoria de muestras
//devuelve resultado del filtro
unsigned short MAFilter32Circular (unsigned short new_sample, unsigned short * p_vec_samples, unsigned char * index, unsigned int * p_sum)
{
	unsigned int total_ma;
	unsigned short * p_vector;

	p_vector = (p_vec_samples + *index);

	//agrego la nueva muestra al total guardado
	total_ma = *p_sum + new_sample;

	//guardo el nuevo sample y actualizo el puntero
	*p_vector = new_sample;
	if (p_vector < (p_vec_samples + 31))
	{
		p_vector++;
		*index += 1;
	}
	else
	{
		p_vector = p_vec_samples;
		*index = 0;
	}

	//resto la muestra - 32 (es la apuntada por el puntero porque es circular)
	total_ma -= *p_vector;
	*p_sum = (unsigned short) total_ma;

	return total_ma >> 5;
}

#ifdef USE_PID_CONTROLLERS
short PID (short setpoint, short sample)
{
	short error = 0;
	short d = 0;

	short val_k1 = 0;
	short val_k2 = 0;
	short val_k3 = 0;

	error = setpoint - sample;

	//K1
	acc = K1V * error;		//5500 / 32768 = 0.167 errores de hasta 6 puntos
	val_k1 = acc >> 7;

	//K2
	acc = K2V * error_z1;		//K2 = no llega pruebo con 1
	val_k2 = acc >> 7;			//si es mas grande que K1 + K3 no lo deja arrancar

	//K3
	acc = K3V * error_z2;		//K3 = 0.4
	val_k3 = acc >> 7;

	d = d_last + val_k1 - val_k2 + val_k3;

	//Update variables PID
	error_z2 = error_z1;
	error_z1 = error;
	d_last = d;

	return d;
}


short PID_roof (short setpoint, short sample, short local_last_d, short * e_z1, short * e_z2)
{
	short error = 0;
	short d = 0;

	short val_k1 = 0;
	short val_k2 = 0;
	short val_k3 = 0;

	error = setpoint - sample;

	//K1
	acc = K1I * error;		//5500 / 32768 = 0.167 errores de hasta 6 puntos
	val_k1 = acc >> 7;

	//K2
	acc = K2I * *e_z1;		//K2 = no llega pruebo con 1
	val_k2 = acc >> 7;			//si es mas grande que K1 + K3 no lo deja arrancar

	//K3
	acc = K3I * *e_z2;		//K3 = 0.4
	val_k3 = acc >> 7;

	d = local_last_d + val_k1 - val_k2 + val_k3;

	//Update variables PID
	*e_z2 = *e_z1;
	*e_z1 = error;

	return d;
}
#endif     //USE_PID_CONTROLLERS

#ifdef USE_MA16_CIRCULAR
//set de punteros y vaciado del filtro
//recibe:
// puntero a estructura de datos del filtro "ma16_data_obj_t *"
void MA16Circular_Reset (ma16_data_obj_t * p_data)
{
    unsigned char i;
    
    for (i = 0; i < 16; i++)
        p_data->v_ma[i] = 0;

    p_data->p_ma = p_data->v_ma;
    p_data->total_ma = 0;
}

//Filtro circular, necesito activar previamente con MA16Circular_Reset()
//recibe:
// puntero a estructura de datos del filtro "ma16_data_obj_t *"
// nueva mustra "new_sample"
//contesta:
// resultado del filtro
unsigned short MA16Circular (ma16_data_obj_t *p_data, unsigned short new_sample)
{
    p_data->total_ma -= *(p_data->p_ma);
    p_data->total_ma += new_sample;
    *(p_data->p_ma) = new_sample;

    if (p_data->p_ma < ((p_data->v_ma) + 15))
        p_data->p_ma += 1;
    else
        p_data->p_ma = p_data->v_ma;

    return (unsigned short) (p_data->total_ma >> 4);    
}

unsigned short MA16Circular_Only_Calc (ma16_data_obj_t *p_data)
{
    return (unsigned short) (p_data->total_ma >> 4);
}

#endif


//calculate the samples fequencies from a samples vector
//the size of the vector ranges must be at least one more of the size of the selected classes (ranges_size)
//frequencies vector must be the same size
void DSP_Vector_Calcule_Frequencies (unsigned short *samples,
                                     unsigned char samples_size,
                                     unsigned short *ranges,
                                     unsigned char ranges_size,
                                     unsigned char *frequencies)
{
    // char s_to_send [64];
    unsigned char i, j;
    unsigned short min_value = 0;
    unsigned short max_value = 0;
    unsigned short range;
    unsigned short width;
    
    //get the MAX and min from the vector samples
    min_value = DSP_Vector_Get_Min_Value(samples, samples_size);
    max_value = DSP_Vector_Get_Max_Value(samples, samples_size);

    // sprintf(s_to_send, "min_value: %d, max_value: %d\n",
    //         min_value,
    //         max_value);
    // Usart2Send(s_to_send);    

    range = max_value - min_value;

    width = range / (ranges_size - 1);
    
    //assembly of the ranges vector
    for (i = 0; i < (ranges_size - 1); i++)
        *(ranges + i) = min_value + width * i;

    *(ranges + ranges_size - 1) = max_value;
    //end of assembly
    
    for (i = 0; i < ranges_size; i++)
    {
        //TODO: mejorar este algoritmo, no entra en el ultimo rango
        //o si es <= lo cuenta dos veces
        for (j = 0; j < samples_size; j++)
        {
            if ((*(samples + j) >= *(ranges + i)) &&
                 (*(samples + j) < *(ranges + i + 1)))
                *(frequencies + i) += 1;
        }
    }
}

//get a vector min value
unsigned short DSP_Vector_Get_Min_Value (unsigned short *vect, unsigned char vect_size)
{
    unsigned char i;
    unsigned short min_value = 0xFFFF;
    
    for (i = 0; i < vect_size; i++)
    {
        if (min_value > *(vect + i))
            min_value = *(vect + i);
    }

    return min_value;
}

//get a vector MAX value
unsigned short DSP_Vector_Get_Max_Value (unsigned short *vect, unsigned char vect_size)
{
    unsigned char i;
    unsigned short max_value = 0;
    
    for (i = 0; i < vect_size; i++)
    {
        if (max_value < *(vect + i))
            max_value = *(vect + i);
    }

    return max_value;
}


//--- end of file ---//

