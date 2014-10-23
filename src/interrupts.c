/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#include <xc.h>            /* HiTech General Includes */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>
#include <plib/usart.h>
#include <plib/timers.h>


#include "system.h"        /* For true/false definition */
#include "user.h"
#include "adc_anemometer.h"
#include "display.h"
/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
#define BUF_LEN 16
uint8_t BUF_CNT = 0;
uint8_t BUF_SERIAL[BUF_LEN];

uint32_t tick = 0;

int flag_measurement=0;

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
/* High-priority service */
void interrupt high_isr(void)
{

      /* This code stub shows general interrupt handling.  Note that these
      conditional statements are not handled within 3 seperate if blocks.
      Do not use a seperate if block for each interrupt flag to avoid run
      time errors. */

      /* Determine which flag generated the interrupt */
      if( RCIE && RCIF)
      {
          uint8_t ch = ReadUSART();

          if(BUF_CNT < BUF_LEN)
              BUF_SERIAL[BUF_CNT++] = ch;
          
          if( (BUF_CNT >= BUF_LEN) || (ch == '\n') || (ch == '\0') )
          {
              BUF_CNT = 0;
              NewCommand(BUF_SERIAL);
          }
      }

      else if( TMR0IE && TMR0IF ) // TIMER 0
      {
        int m_done = take_measurment(RESULTS_SIZE, PS_1_1);
        if( m_done )
        {
            uint16_t val = GetWindSpeed();
            NewMeasure(val);

        }
        TMR0IF = 0; // Clear Interrupt Flag 1
      }

}

/* Low-priority interrupt routine */
void low_priority interrupt low_isr(void)
{

      /* This code stub shows general interrupt handling.  Note that these
      conditional statements are not handled within 3 seperate if blocks.
      Do not use a seperate if block for each interrupt flag to avoid run
      time errors. */

      /* Determine which flag generated the interrupt */
      if( TMR1IE && TMR1IF ) /* TIMER 1*/
      {
          //LED_TOGGLE;
          WriteTimer1(0);
          tick++;

          NewTick(&tick);

          TMR1IF = 0; /* Clear Interrupt Flag 1 */
      }
}
