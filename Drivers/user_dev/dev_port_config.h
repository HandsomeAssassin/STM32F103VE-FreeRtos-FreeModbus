#ifndef USER_DEV_DEV_PORT_CONFIG_H_
#define USER_DEV_DEV_PORT_CONFIG_H_

#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************set exdi port*****************/
#ifdef USER_DEV_DEV_DI_H_
#define DI_MAX_NUM 32

typedef struct {
	GPIO_TypeDef *port;
	uint32_t pin;
} DiInfo;

static DiInfo di_list[] = {
	{GPIOB, GPIO_PIN_1},
	{GPIOB, GPIO_PIN_15},
};
#endif //USER_DEV_DEV_DI_H_

/*****************set exdo port*****************/
#ifdef USER_DEV_DEV_DO_H_
#define DO_MAX_NUM 16

typedef struct {
	GPIO_TypeDef *port;
	uint32_t pin;
} DoInfo;

static DoInfo do_list[] = {
	{GPIOC, GPIO_PIN_13},
	{GPIOB, GPIO_PIN_14},
	{GPIOC, GPIO_PIN_12},
};
#endif //USER_DEV_DEV_DO_H_

/*****************set printf port***************/
#define PRINTF_UART USART1
#define PRINTF_BAUDRATE 115200

/*****************set modbus rs485 port*********/
#define RS485_UART USART3
#define RS485_RE_PORT GPIOC
#define	RS485_RE_PIN GPIO_PIN_4
#define RS485_DE_PORT GPIOC
#define RS485_DE_PIN GPIO_PIN_5

/*****************set Timer's port**************/
#define TIMER TIM3

/*****************Flash Info********************/
#define DEV_MAX_FLASH_ADDR 0X0807ffff
#define DEV_MIN_FLASH_ADDR 0X0807F800

#ifdef __cplusplus
}
#endif

#endif //USER_DEV_DEV_PORT_CONFIG_H_

