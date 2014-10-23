//#include <peripheral/adc.h>
#include <plib/adc.h>
//#include <peripheral/timers.h>
#include <plib/timers.h>
#include <stdint.h>
#include "adc_anemometer.h"

#define AREA 0.5026

int give_windspeed(int crosses, int Ts)
{
	/*	this function utilizes a linear regresion
		to determine the wind speed given a certain 
		number of "crosses" over a given time "Ts"
	
		So de model is:
		ws=wind speed
		c=crosses
			c=a*ws+b
	*/
	double a, b, c;
	switch(Ts)
	{
		case 1:
			a=3.296;
			b=-3.774;
		break;
		case 2:
			a=1;
			b=0;
		break;
		case 4:
			a=1;
			b=0;
		break;
		case 8:
			a=1;
			b=0;
		break;
		case 16:
			a=1;
			b=0;
		break;
		case 32:
			a=1;
			b=0;
		break;
		case 64:
			a=1;
			b=0;
		break;
		case 128:
			a=1;
			b=0;
		break;
		case 256:
			a=1;
			b=0;
		break;
	}
	//printf("give_windspeed(crosses=%d, Ts=%d)\n", crosses, Ts);
	//c=a*ws+b
	//ws=(c-b)/a

	double velocidad = (crosses*a+b)*1000;
    double caudal = velocidad*3.6*AREA;

    if(caudal > 0)
        return (int)(caudal);
    else
        0;
}
void timer_int_ms(int Ts)
{
	//printf("timer_int_ms(Ts=%d)\n", Ts);
	switch(Ts)
	{
		case 1:
			CloseTimer0();
			OpenTimer0(T0_8BIT&T0_SOURCE_INT&T0_PS_1_1);
		break;
		case 2:
			CloseTimer0();
			OpenTimer0(T0_8BIT&T0_SOURCE_INT&T0_PS_1_2);
		break;
		case 4:
			CloseTimer0();
			OpenTimer0(T0_8BIT&T0_SOURCE_INT&T0_PS_1_4);
		break;
		case 8:
			CloseTimer0();
			OpenTimer0(T0_8BIT&T0_SOURCE_INT&T0_PS_1_8);
		break;
		case 16:
			CloseTimer0();
			OpenTimer0(T0_8BIT&T0_SOURCE_INT&T0_PS_1_16);
		break;
		case 32:
			CloseTimer0();
			OpenTimer0(T0_8BIT&T0_SOURCE_INT&T0_PS_1_32);
		break;
		case 64:
			CloseTimer0();
			OpenTimer0(T0_8BIT&T0_SOURCE_INT&T0_PS_1_64);
		break;
		case 128:
			CloseTimer0();
			OpenTimer0(T0_8BIT&T0_SOURCE_INT&T0_PS_1_128);
		break;
		case 256:
			CloseTimer0();
			OpenTimer0(T0_8BIT&T0_SOURCE_INT&T0_PS_1_256);
		break;
	}
    TMR0IE = 1;
    TMR0IP = 1;
}
int take_measurment(int results_size, int Ts)
{
	/* 	this function should be called every "Tadc"
		probably by the ISR of a Timer perpheral
		it push_back the result "sample" in a 
		global int array named "results".
		After "results_size" samples are pushed
		this function turn-off the ISR of the
		Timer and returns an int "0x01"
		indicating success.
	*/
	static int state, i;
	//state: 0=convert0, 1=push, 2=convert
	switch(state)
	{
		case 0:	//convert0
			//printf("convert0, timer_int_ms(Ts=%d), ConvertADC()\n", Ts);
			timer_int_ms(Ts);
			ConvertADC();
			state=1;
		break;
		case 1:	//push
			if(i<results_size)
			{
				//printf("push[%d], ReadADC()\n",i);
				results[i]=ReadADC();
				state=2;
				i++;
			}
			else
			{
				//printf("push, timer_int_ms(Td=%d)\n",Td);
				i=0;
				state=0;
				CloseTimer0();
				//timer_int_ms(Td);
				return 1;
			}
		break;
		case 2:	//convert
			//printf("convert, ConvertADC()\n");
			ConvertADC();
			state=1;
		break;
	}
	return 0;
}
void results_average(int results_size, int nsamp)
{
	/*	this function simply average "nsamp" consecutive
		ADC result "sample" in the same buffer "results".
		When end of "results" is reached a special character
		"EOresults" is pushed
	*/
	if(nsamp==1){return;}
	int i, j, avg, max;
	for(i=0; i<results_size; i+=nsamp)
	{	
		avg=0;
		max=i+nsamp;
		for(j=i; j<max; j++)
		{
			avg+=results[j];
		}
		avg=avg/nsamp;
		results[ (j/nsamp)-1 ]=avg;
	}
	j=i+nsamp;
	results[ (j/nsamp)-1 ]=EO_RESULTS;
}
int num_of_crosses(int results_size, int limit)
{
	/*	this function process the "results" and returns
		an int indicating how many times the signal
		crossed "limits" int the rising direction
	
		This is done using an FDA (finite
		deterministic automaton)
	*/
	int crosses, i, state;
	crosses=0;

	//state: 0=down, 1=rising, 2=up
	if( results[0]<limit )
	{
		state=0;
	}
	else
	{
		state=2;
	}
	for(i=1; i<results_size; i++)
	{
		if( results[i]==EO_RESULTS ){break;}
		switch(state)
		{
			case 0:
				if( results[i]>=limit )
				{
					state=1;
				}
			break;
			case 1:
				state=2;
				crosses++;
			break;
			case 2:
				if( results[i]<limit )
				{
					state=0;
				}
			break;
		}
	}
	return crosses;
}

uint16_t GetWindSpeed(void)
{
    results_average(RESULTS_SIZE, N_SAMPLE);
    int crosses=num_of_crosses(RESULTS_SIZE, VOLTAGE_LIMIT);
    int value = give_windspeed(crosses, PS_1_1);
    return (uint16_t)(value);
}

void StartMeasurement(void)
{
    timer_int_ms(PS_1_1);
}