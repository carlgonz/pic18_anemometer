/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        20000000L
#define FCY             SYS_FREQ/4

/* Baudrate generator's definitions */
#define BAUDRATE_H_20M_9600 129
#define BAUDRATE_H_25M_9600 162
#define BAUDRATE_L_20M_9600 32
#define BAUDRATE_L_40M_9600 64

/* TMR 0 MS CONVERTION */
#define T0_20M_16B_13MS    0b11111111  // Prescaler 1:1 (NO Prescaler)
#define T0_20M_16B_26MS    0b11110000  //           1:2
#define T0_20M_16B_52MS    0b11110001  //           1:4
#define T0_20M_16B_104MS   0b11110010  //           1:8
#define T0_20M_16B_210MS   0b11110011  //           1:16
#define T0_20M_16B_420MS   0b11110100  //           1:32
#define T0_20M_16B_838MS   0b11110101  //           1:64
#define T0_20M_16B_1680MS  0b11110110  //           1:128
#define T0_20M_16B_3355MS  0b11110111  //           1:256

/* TMR 2 MS CONVERTION */
#define TMR2_MS_16_16_20MHZ 20
#define TMR2_MS_16_16_40MHZ 50


/* OLIMEX PIC-P40 LED CONTROL */
#define LED_ENABLE  TRISA0 = 0
#define LED_TOGGLE  RA0 = LATA0 ^ 0x01;
#define LED_ON      RA0 = 1
#define LED_OFF     RA0 = 0

typedef struct{
    uint8_t LEN;
    uint8_t BUF[16];
    uint8_t CNT;
}ioBuffasd;

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

void ConfigureInterrupts(void);
void ConfigureOscillator(void); /* Handles clock switching/osc initialization */
void ConfigureRS232(void);
void ConfigureAdc(void);
void ConfigureTimer0(void);//uint8_t T0_PS_MS);
void ConfigureTimer1(void);
void ConfigureTimer2(void);
void ConfigureTimer3(void);
void SysDelayUS(uint16_t time_us);
void SysDelayMS(uint16_t time_ns);
char* MesureToString(uint16_t int_val);