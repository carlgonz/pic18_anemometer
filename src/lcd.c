/******************************************************************************/
/*                                  LCD DRIVER                                */
/*               SparkFun LCD-00710 - 128x64 Matrix Dot - Backlight           */
/******************************************************************************/
#include <plib/usart.h>

#include "lcd.h"
#include "system.h"

/*----------------------------------------------------------------------------*/
/*                      LCD PORT ASIGNMENT
 * ············································································
/*              LCD ->  E   DI  RW  RST CS1 CS2 RD7~RD0
/*              PIC ->  RD2 RD3 RD7 RD6 RD5 RD4 PORTB
/* ---------------------------------------------------------------------------*/

static void LcdDoCommand(uint8_t cmd);
static void LcdWriteExec(void);
static uint8_t LcdReadExec(void);
static void CleanLcdReg(void);

LCD_CTRL Lcd;

void LcdStart(void)
{
    LcdInit();
    /**/ LcdDisplayData(LCD_ON, SIDE_L);
    /**/ LcdDisplayData(LCD_ON, SIDE_R);
}

void LcdInit(void)
{
    CleanLcdReg();
    /* Configure ports */
    TRISB = 0;
    TRISD = 0;

    //putsUSART(">> LCD: Init Ports [OK]\0");

    /* RESET LCD */
    RD6 = SET_RESET; SysDelayMS(100);
    RD2 = E_LOW;
    RD3 = Lcd.DI;       /* RST DI */
    RD7 = Lcd.RW;       /* RST RW */
    RD5 = Lcd.CS1;      /* RST CS1 */
    RD4 = Lcd.CS2;      /* RST CS2 */
    RD6 = NOT_RESET; SysDelayMS(500);

    //putsUSART(">> LCD: Reset [OK]\0");
}

void LcdFill(uint8_t color)
{
    /* ASSERT COLOR == {0,1} */
    color = color ? 0xFF : 0x00;

    for(uint8_t r=0; r<8; r++)
    {
        LcdSetColRow(0,r);
        LcdSetColRow(64,r);
        for(uint8_t c=0; c<128; c++)
            LcdWriteData(color, c);
    }
}

void LcdSetColRow(uint8_t col, uint8_t row)
{
    uint8_t side = col < 64 ? SIDE_L : SIDE_R;
    LcdSetRow(row,side);
    LcdSetCol(col);
}

void LcdSetRow(uint8_t row, uint8_t side)
{
    if(row > 7) return;

    if (side == SIDE_R)         Lcd.CS2 = CS_ENA;
    else if (side == SIDE_L)   Lcd.CS1 = CS_ENA;
    else                        return;
    
    uint8_t cmd = 0b10111000 + row;
    //putsUSART(">> LCD: Set row [->]\0");
    LcdDoCommand(cmd);
}

void LcdSetCol(uint8_t col)
{
    uint8_t cmd = 0b01000000;

    if(col > 127)
    {
        return;
    }
    else if(col > 63)
    {
        cmd += col - 64;
        Lcd.CS2 = CS_ENA;
    }
    else
    {
        cmd += col;
        Lcd.CS1 = CS_ENA;
    }
    //putsUSART(">> LCD: Set col [->]\0");
    LcdDoCommand(cmd);
}

void LcdWriteData(uint8_t dt, uint8_t side)
{
    Lcd.DI = DI_DATA;
    Lcd.RW  = RW_WRITE;
    Lcd.DATA = dt;

    side = side > 63 ? SIDE_R : SIDE_L;

    switch(side)
    {
        case SIDE_L: Lcd.CS1 = CS_ENA; break;
        case SIDE_R: Lcd.CS2 = CS_ENA; break;
        default: Lcd.CS1 = Lcd.CS2 = CS_DIS; break;
    }
    //putsUSART(">> LCD: Write [->]\0");
    LcdWriteExec();
}

uint8_t LcdReadData(uint8_t side)
{
    Lcd.DI = DI_DATA;
    Lcd.RW  = RW_READ;

    switch(side)
    {
        case SIDE_L: Lcd.CS1 = CS_ENA; break;
        case SIDE_R: Lcd.CS2 = CS_ENA; break;
        default: Lcd.CS1 = Lcd.CS2 = CS_DIS; break;
    }
    //putsUSART(">> LCD: Read [->]\0");
    Lcd.DATA = LcdReadExec();
    return Lcd.DATA;
}

void LcdDisplayData(uint8_t on_off, uint8_t side)
{
    if(side == SIDE_L)
        Lcd.CS1 = CS_ENA;
    else if(side == SIDE_R)
        Lcd.CS2 = CS_ENA;
    else
        Lcd.CS1 = Lcd.CS2 = CS_ENA;

    uint8_t cmd = 0b00111110 + on_off;
    LcdDoCommand(cmd);
}

static void LcdDoCommand(uint8_t cmd)
{
    Lcd.DI = DI_INST;
    Lcd.RW  = RW_WRITE;
    Lcd.DATA = cmd;
    //putsUSART(">> LCD: DoCmd [->]\0");
    LcdWriteExec();
}

static void LcdWriteExec(void)
{
    //putsUSART(">> LCD: WriteExec [...]\0\n");

    //RD2 = E_LOW;    /* enable */
    /*LCD_EN Must be LOW */
    RD3 = Lcd.DI;       /* Write DI */
    RD7 = Lcd.RW;       /* Write RW */
    RD5 = Lcd.CS1;      /* Write CS1 */
    RD4 = Lcd.CS2;      /* Write CS2 */
    SysDelayUS(50);      /* t_asu 140ns*/
    RD2 = E_HIG;        /* Lcd reads control bits at rising*/

    //putsUSART(">> LCD: Config [OK]\0\n");

    PORTB = Lcd.DATA;   /* Put Data */
    SysDelayUS(50);      /* t_dsu */

    //putsUSART(">> LCD: Data [OK]\0\n");

    RD2 = E_LOW;        /* LCD Reads data at falling */
    SysDelayUS(50);      /* t_wl 450n*/

    CleanLcdReg();

    RD3 = Lcd.DI;       /* RST DI */
    RD7 = Lcd.RW;       /* RST RW */
    RD5 = Lcd.CS1;      /* RST CS1 */
    RD4 = Lcd.CS2;      /* RST CS2 */
    //putsUSART(">> LCD: WriteExec [OK]\0\n");
}

static uint8_t LcdReadExec(void)
{
    /* TODO: Implement LcdReadExec */
    return 0;
}

static void CleanLcdReg(void)
{
    Lcd.CS1    = CS_DIS;
    Lcd.CS2    = CS_DIS;
    Lcd.E      = E_LOW;
    Lcd.RST    = NOT_RESET;
    Lcd.DI     = DI_INST;
    Lcd.RW     = RW_WRITE;
    Lcd.DATA   = 0;
}
