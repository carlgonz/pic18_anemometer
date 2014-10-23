/*	"Tadc" should be larger than (t1+t2). Where:
		t1= time needed by the ADC to obtain "sample"
		t2= time needed to push "sample" inside "results"

	In the other hand "Tadc" should be shorter than the 0.5*(1/Fmax)=0.5*(Tmin).
	Where Fmax is the maximum frecuency of the ADC-converted signal that the
	Routine is able to detect. Fmax is by now 287Hz, but in a worst case 
	scenario let's assume Fmax is 0.5kHz, so Tadc=1ms .
		
*/
#include <stdint.h>

#define	EO_RESULTS	1025	// ADC is 10 bits, it means 1025 works as an EOF for the "results" buffer

#define	N_SAMPLE	4       // Number of consecutive samples to average MAKE SURE RESULTS_SIZE/N_SAMPLE= integer
#define VOLTAGE_LIMIT	156	// 1024*(0.5/3.3) = 155.151515

#define	RESULTS_SIZE	500	// Ts*Tadc = sample_time*samples_per_second = 300[ms]*1[samples/ms] = 300 samples

/*
Tosc=1/(20*10^6)=0.05[us]
Tcyc=4*Tosc=2.0[us]

ADC:
Tda>=1,6[us] => ADC_FOSC_32
Timer0:
Tint=(Tosc*4)*(2^8)=(0.05/4)*(256)=0051.2[ms]
*/
#define PS_1_1			1		// time[ms] between 2 consecutive ADC Int. Ts=0051.2[ms]*1[ms]	=0051.2[ms]
#define PS_1_2			2		// time[ms] between 2 consecutive ADC Int. Ts=0051.2[ms]*2[ms]	=0102.4[ms]
#define PS_1_4			4		// time[ms] between 2 consecutive ADC Int. Ts=0051.2*4[ms]		=0204.8[ms]
#define PS_1_8			8		// time[ms] between 2 consecutive ADC Int. Ts=0051.2*8[ms]		=0409.6[ms]
#define PS_1_16			16		// time[ms] between 2 consecutive ADC Int. Ts=0051.2*16[ms]		=0819.2[ms]
#define PS_1_32			32		// time[ms] between 2 consecutive ADC Int. Ts=0051.2*32[ms]		=1638.4[ms]
#define PS_1_64			64		// time[ms] between 2 consecutive ADC Int. Ts=0051.2*64[ms]		=3276.8[ms]
#define PS_1_128		128		// time[ms] between 2 consecutive ADC Int. Ts=0051.2*128[ms]	=6553.6[ms]
#define PS_1_256		256		// time[ms] between 2 consecutive ADC Int. Ts=0051.2*256[ms]	=13107.2[ms]

int results[ RESULTS_SIZE ];
	/*	global variable, it stores the "sample"
		samples of the ADC module.
	*/

/*--PUBLIC FUNCTIONS-------------------------------------------------------------------------------------*/
int take_measurment(int results_size, int Ts);
	/* 	this function should be called every "Tadc"
		probably by the ISR of a Timer perpheral
		it push_back the result "sample" in a 
		global int array named "results".
		After "results_size" samples are pushed
		this function turn-off the ISR of the
		Timer and returns an int "0x01"
		indicating success.
		
		This is done using an FDA (finite
		deterministic automaton)
	*/
void results_average(int results_size, int nsamp);
	/*	this function simply average "nsamp" consecutive
		ADC result "sample" in the same buffer "results".
		When end of "results" is reached a special character
		"EOresults" is pushed
	*/
int num_of_crosses(int results_size, int limit);
	/*	this function process the "results" and returns
		an int array and return how many times the 
		signal crossed "limits" int the rising direction
	*/

void timer_int_ms(int Ts);
int give_windspeed(int crosses, int Ts);

void StartMeasurement(void);
uint16_t GetWindSpeed(void);