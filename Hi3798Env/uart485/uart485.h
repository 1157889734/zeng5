
#define TRX_EN_PIN  19
#define TRX_EN_REG 0XF8A2104C
#define TRX_EN_REG_VAL 0x130

#define UART3_TXD_REG 0XF8A21058
#define UART3_TXD_REG_VAL 0x132

#define UART3_RXD_REG 0XF8A21054
#define UART3_RXD_REG_VAL 0x132

#define UART_HEAD 0x01
#define UART_HEADDATA 0x02
#define UART_DATA 0x03
#define UART_END 0x04


unsigned char get_rs485_com(void);
void set485BegFlag(unsigned char flag);
unsigned char  get485BegFlag(void) ;
int uart485Test(void);

