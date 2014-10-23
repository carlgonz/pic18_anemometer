/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void InitApp(void);         /* I/O and Peripheral Initialization */
void InitApp2(void);
void NewTick(uint32_t *tick);
void NewMeasure(uint16_t value);
void ContMeasure(uint8_t sema_val);
void FanStart(uint8_t sema_val);
void ParseCmd(uint8_t *cmd);
void NewCommand(uint8_t *buf);