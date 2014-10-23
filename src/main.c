/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>           /* Global Header File */
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <plib/usart.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "lcd.h"
#include "display.h"
#include "adc_anemometer.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

uint8_t main(void)
{
    /* Initialize I/O and Peripherals for application */
    InitApp();
    putsUSART(">>\n\nBooting PIC18F452: [->]\n");

    SysDelayMS(2);

    putsUSART(">>Starting LCD: [->]\n");
    LcdStart();
    DispDrawFrame();
	DispDrawBat(4);
    putsUSART(">>Starting LCD: [OK]\n");
	
	SysDelayMS(2);

	InitApp2();
	putsUSART(">>\n\nBooting PIC18F452: [OK]\n");

    while(1)
    {
        /*RESET WDT*/
        CLRWDT();
    }
    return 0;
}

