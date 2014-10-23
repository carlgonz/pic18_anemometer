/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <xc.h>            /* HiTech General Includes */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <stdio.h>
#include <string.h>
#include <plib/usart.h>
#include <plib/adc.h>

#include "system.h"
#include "user.h"
#include "lcd.h"
#include "display.h"
#include "adc_anemometer.h"


/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/
uint8_t *CMD;
uint8_t SEMA_CMD;
uint8_t SEMA_FAN = 1;
uint8_t SEMA_MEASURE = 0;
uint8_t SEMA_CONTMEASURE = 1;

uint16_t MES_VALUE = 0;

void InitApp(void)
{
    /**/TRISD6 = 0; RD6 = 0;
    TRISA0 = 1; /* A0->ADC Input */
    SWDTEN = 0; /*WDT OFF*/
    ConfigureRS232();
}

void InitApp2(void)
{
    ConfigureTimer1();
    OpenADC(ADC_FOSC_32&ADC_RIGHT_JUST&ADC_1ANA_0REF, ADC_CH0&ADC_INT_OFF);	//AN_0
    ConfigureInterrupts();
}

/* SYSTEM TICK ROUTINE*/
void NewTick(uint32_t *tick)
{
    uint8_t two_ticks;
    two_ticks = (*tick) % 2;
    if(two_ticks == 0)
    {
        if(SEMA_FAN) DispDrawFan(0);
    }
    else
    {
        if(SEMA_FAN) DispDrawFan(1);
    }
        //putsUSART(">>System tick\0");

    if(SEMA_CMD)
    {
        ParseCmd(CMD);
        SEMA_CMD = 0;
    }

    if(SEMA_MEASURE)
    {
        putsUSART(MesureToString(MES_VALUE));
        DispDrawNumber(MES_VALUE);
        SEMA_MEASURE = 0;
    }

    if(SEMA_CONTMEASURE)
    {
        if(!SEMA_MEASURE)
        {
            StartMeasurement();
            DispDrawNumber(MES_VALUE);
        }
    }

}

void NewCommand(uint8_t *buf)
{
    SEMA_CMD = 1;
    CMD = buf;
}

void NewMeasure(uint16_t value)
{
    SEMA_MEASURE = 1;
    MES_VALUE = value;
}

void ContMeasure(uint8_t sema_val)
{
    SEMA_CONTMEASURE = sema_val;
}

void FanStart(uint8_t sema_val)
{
    SEMA_FAN = sema_val;
}

void ParseCmd(uint8_t *cmd)
{
    bool ok = false;
    uint8_t inst[8] = "\0\0\0\0\0\0\0\0";
    uint8_t i;
    /* Get instruction */
    for(i=0; i<8; i++)
    {
        if( (cmd[i] == '\0') || (cmd[i] == '\n') || (cmd[i] == ' '))
            break;
        else
            inst[i] = cmd[i];
    }
    i++;

    /* FAN_ON */
    const uint8_t fanon[8] = "fan_on\0\0";
    if(strcmp(inst, fanon) == 0)
    {
        SEMA_FAN = 1;
        return;
    }

    /* FAN_OFF */
    const uint8_t fanoff[8] = "fan_off\0";
    if(strcmp(inst, fanoff) == 0)
    {
        SEMA_FAN = 0;
        return;
    }

    /* BATERY LEVEL */
    const uint8_t bat[8] = "bat\0\0\0\0\0";
    if(strcmp(inst, bat) == 0)
    {
        uint8_t level = cmd[i]-0x30;
        DispDrawBat(level);
        return;
    }

    /* DISPLAY NUMBER */
    const uint8_t disp[8] = "disp\0\0\0\0";
    if(strcmp(inst, disp) == 0)
    {
        uint8_t numb[5];
        ok = true;
        for(uint8_t j=0; j<5; j++)
        {
            if( (cmd[i+j]>0x39) || (cmd[i+j]<0x30) )
            {
                ok = false;
                break;
            }
            else
                numb[j] = cmd[i+j];
        }

        if(ok)
        {
            DispDrawsStrNumber(numb);
            return;
        }
    }
    
    /* GET WINDSPEED */
    const uint8_t measure[8] = "measure\0";
    if(strcmp(inst, measure) == 0)
    {
        putsUSART(">>StartMeasure...\n");
        StartMeasurement();
        return;
    }

    const uint8_t cntmeasure[8] = "contmes\0";
    if(strcmp(inst, cntmeasure) == 0)
    {
        SEMA_CONTMEASURE = SEMA_CONTMEASURE ? 0:1;
        if(SEMA_CONTMEASURE)
            putsUSART(">>Continuous Measurement [ON]\n");
        else
            putsUSART(">>Continuous Measurement [OFF]\n");
        return;
    }

    /* INVALID */
    if(!ok)
        putsUSART("\n>>Invalid Command\n");
}

