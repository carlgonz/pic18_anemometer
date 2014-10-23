/******************************************************************************/
/*                            LCD DRIVER HEADER                               */
/*               SparkFun LCD-00710 - 128x64 Matrix Dot - Backlight           */
/******************************************************************************/
#include <xc.h>
#include <stdint.h>

typedef struct LCD_CTRL{
    uint8_t RW      :1;
    uint8_t RST     :1;
    uint8_t CS1     :1;
    uint8_t CS2     :1;
    uint8_t DI      :1;
    uint8_t E       :1;
    uint8_t DATA    :8;
}LCD_CTRL;

#define CS_ENA      0
#define CS_DIS      1

#define SET_RESET   0
#define NOT_RESET   1

#define DI_DATA     1
#define DI_INST     0

#define RW_READ     1
#define RW_WRITE    0

#define E_LOW       0
#define E_HIG       1

#define SIDE_L      0
#define SIDE_R      1
#define SIDE_B      2

#define LCD_ON      1
#define LCD_OFF     0

/*----------------------------------------------------------------------------*/
/*                          Function declarations                             */
/* ---------------------------------------------------------------------------*/
void LcdStart(void);
void LcdInit(void);
void LcdSetRow(uint8_t row, uint8_t side);
void LcdSetCol(uint8_t col);
void LcdWriteData(uint8_t dt, uint8_t side);
uint8_t LcdReadData(uint8_t side);
void LcdDisplayData(uint8_t on_off, uint8_t side);
void LcdFill(uint8_t color);
void LcdSetColRow(uint8_t col, uint8_t row);
