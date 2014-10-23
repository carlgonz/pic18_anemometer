/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#include <htc.h>           /* HiTech General Includes */
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <plib/usart.h> /* USART headers */
#include <plib/timers.h>
#include "system.h"

/******************************************************************************/
/* DealysDegine                                                */
/******************************************************************************/
#ifdef SYS_FREQ
#if SYS_FREQ == 20000000L

#define _delay200n  asm("nop")
#define _delay400n  _delay200n; _delay200n
#define _delay1u    _delay400n; _delay400n; _delay200n

#endif
#endif

/* Configures USART as RS232 8Bit, no parity, 9600bps */
void ConfigureRS232(void)
{
    /*NOTE: Replace #define BusyUSART( ) (!TRMT_x) by
     #define BusyUSART( ) (!TRMT) in usart.h file*/

    OpenUSART(  USART_TX_INT_OFF & USART_RX_INT_ON &
                USART_BRGH_HIGH & USART_CONT_RX &
                USART_EIGHT_BIT & USART_ASYNCH_MODE & USART_ADDEN_OFF,
                BAUDRATE_H_20M_9600);
    
    RCIP = 1; /*USART RX Interrupt HIGH Prio*/
}

/* Enables interrupts - Priority ON - Periph ON*/
void ConfigureInterrupts(void)
{
    IPEN = 1; /* Priorities enable*/
    GIEH = 1; /* Enable HIGH priority int.*/
    PEIE_GIEL = 1; /* LOW Priority int and Peripheral int ENABLE */
}

/* Configure TMR0 ms miliseconds*/
void ConfigureTimer0(void)//uint8_t T0_PS_MS)
{
    //OpenTimer0( T0_16BIT & T0_SOURCE_INT &
    //            T0_EDGE_RISE & T0_PS_MS );
    OpenTimer0(T0_8BIT & T0_SOURCE_INT & T0_PS_1_32);
    TMR0IE = 1; /* Enable int */
    TMR0IP = 1; /* Hig priority */
}

void ConfigureTimer1(void)
{
    OpenTimer1(T1_16BIT_RW & T1_OSC1EN_OFF & T1_PS_1_8 & T1_SOURCE_INT & T1_SYNC_EXT_OFF);
    TMR1IE = 1; /* Enable int */
    TMR1IP = 0; /* Low priority */
}

/* Configure TMR1 for DELAYS in us*/
void ConfigureTimer2(void)
{
    OpenTimer2( T2_PS_1_1 & T2_POST_1_5 );
    TMR2IE = 0; /* Enable int */
    TMR2IP = 0; /* Low priority */
}

void ConfigureTimer3(void)
{
    OpenTimer3(T3_16BIT_RW & T3_PS_1_8 & T3_SOURCE_INT & T3_SYNC_EXT_OFF);
    TMR3IE = 1; /* Enable int */
    TMR3IP = 0; /* Low priority */
}

void SysDelayUS(uint16_t time_us)
{
    while(time_us--)
        _delay1u;
}
void SysDelayMS(uint16_t time_ms)
{
    while(time_ms--)
        SysDelayUS(1000);
}

void ConfigureAdc(void);

void ConfigureOscillator(void);
    /* Typical actions in this function are to tweak the oscillator tuning
    register, select new clock sources, and to wait until new clock sources
    are stable before resuming execution of the main project. */


/* Defines STDOUT as USART*/
void putch(char data)
{
    while( !TXIF ); /* Wait until TXREG is empty*/
    TXREG = data;
}

char* MesureToString(uint16_t int_val)
{
    static char str_val[18]=">>Measure: 12.345\n";
    str_val[16] = int_val%10 + 0x30;
    str_val[15] = (int_val%100)/10 + 0x30;
    str_val[14] = (int_val%1000)/100 + 0x30;
    str_val[12] = (int_val%10000)/1000 + 0x30;
    str_val[11] = (int_val%100000)/10000 + 0x30;
    return str_val;
}
