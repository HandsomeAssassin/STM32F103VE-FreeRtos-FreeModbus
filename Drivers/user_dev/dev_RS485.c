#include "dev_RS485.h"
#include <string.h>
#include <stdio.h>
#include "dev_printf.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "dev_port_config.h"

#define DGB_INFO(fmt, args...) do { printf("[DEV_485_INFO] %s(), %d,]"fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_WARN(fmt, args...) do { printf("[DEV_485_WARN] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_ERROR(fmt, args...) do { printf("[DEV_485_ERROR] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)

static UART_HandleTypeDef rs485;

static int RS485RWCtrlInit()
{
	GPIO_InitTypeDef io;
	io.Mode = GPIO_MODE_OUTPUT_PP;
	io.Pull = GPIO_PULLUP;
	io.Speed = GPIO_SPEED_FREQ_HIGH;
	io.Pin = RS485_RE_PIN;

	HAL_GPIO_Init(RS485_RE_PORT, &io);

	io.Pin = RS485_DE_PIN;
	HAL_GPIO_Init(RS485_DE_PORT, &io);

	return 0;
}

static int RS485RWCtrlDeInit()
{
	HAL_GPIO_DeInit(RS485_RE_PORT, RS485_RE_PIN);
	HAL_GPIO_DeInit(RS485_DE_PORT, RS485_DE_PIN);
	return 0;
}

static void RS485RxEn(int en)
{
	if (en)
		rs485.Instance->CR1 |= 1 << 2;
	else
		rs485.Instance->CR1 &= ~(1 << 2);
}

int RS485RWCtrl(RS485Mode mode)
{
	if (mode == RS485_W) {
		RS485RxEn(0);	//because in the tx mode the rxen is set 1,it's may be the hardware error,
						//so need to disable rx.
		HAL_GPIO_WritePin(RS485_RE_PORT, RS485_RE_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(RS485_DE_PORT, RS485_DE_PIN, GPIO_PIN_SET);
	} else {
		RS485RxEn(1);
		HAL_GPIO_WritePin(RS485_RE_PORT, RS485_RE_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(RS485_DE_PORT, RS485_DE_PIN, GPIO_PIN_RESET);
	}

	return 0;
}

int DevRS485Init(unsigned long badu_rate, uint32_t Parity)
{
	rs485.Instance = RS485_UART;
  	rs485.Init.BaudRate = badu_rate;
  	rs485.Init.WordLength = UART_WORDLENGTH_8B;
  	rs485.Init.StopBits = UART_STOPBITS_1;
  	rs485.Init.Parity = Parity;
  	rs485.Init.Mode = UART_MODE_TX_RX;
  	rs485.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  	rs485.Init.OverSampling = UART_OVERSAMPLING_16;
  	if (HAL_UART_Init(&rs485) != HAL_OK) {
		DGB_ERROR("Uart int error\n");
		return -1;
  	}

	HAL_NVIC_EnableIRQ(USART3_IRQn);
	HAL_NVIC_SetPriority(USART3_IRQn, 1, 0);

	DevRS485RecEndIrq(0);
	DevRS485SendEndIrq(0);

	osDelay(200);			//because the CR1 TE set, uart will be send a empty farme, and the tc will be set 1
							//so need delay some time, then clear the tc.
	rs485.Instance->SR = 0;

	RS485RWCtrlInit();

	return 0;
}

int DevRS485RecEndIrq(uint8_t en)
{
	en?__HAL_UART_ENABLE_IT(&rs485, UART_IT_RXNE):__HAL_UART_DISABLE_IT(&rs485, UART_IT_RXNE);
	return 0;
}

int DevRS485SendEndIrq(uint8_t en)
{
	en?__HAL_UART_ENABLE_IT(&rs485, UART_IT_TC):__HAL_UART_DISABLE_IT(&rs485, UART_IT_TC);
	return 0;
}

int DevRS485DeInit()
{
	HAL_UART_DeInit(&rs485);
	RS485RWCtrlDeInit();
	HAL_NVIC_DisableIRQ(USART3_IRQn);

	return 0;
}

int DevRS485Write(uint8_t *data, uint32_t size)
{
	while(size--)
		rs485.Instance->DR = (*data++ & (uint8_t)0xFF);

	return 0;
}

int DevRS485Read(uint8_t *data, uint32_t size)
{
	while(size--) {
		*data = (uint8_t)(rs485.Instance->DR & (uint8_t)0x00FF);
		data++;
	}

	return 0;
}

void DevRS485ClearFlag(void)
{
	rs485.Instance->SR = 0;
}

void DevRS485ClearOverFlow(void)
{
	while((__HAL_UART_GET_FLAG(&rs485, UART_FLAG_ORE) ? SET : RESET) == SET) {
		uint32_t temp = rs485.Instance->SR;
		uint8_t ch =  (uint8_t)(rs485.Instance->DR & (uint8_t)0x00FF);
	}
}

int DevRS485RxRecEndIrq(void)
{
	return (__HAL_UART_GET_FLAG(&rs485, UART_FLAG_RXNE) == SET);
}

int DevRS485TxTcIrq(void)
{
	return (__HAL_UART_GET_FLAG(&rs485, UART_FLAG_TC) == SET);
}

void DevRS485IrqHandle(void)
{
	HAL_UART_IRQHandler(&rs485);
}

#if 0
static uint8_t ch = 'v',flag =0;

void USART3_IRQHandler(void)
{
	if(DevRS485RxRecEndIrq() == 0) {
		DevRS485Read(&ch, 1);
	}

	DevRS485IrqHandle();
	DevRS485ClearOverFlow();
}


/*********RS485 DEMO*********/

void RS485Test()
{
	DevRS485Init(USART3, 9600, UART_PARITY_NONE);

	DevRS485RecEndIrq(1);
	RS485RWCtrl(RS485_R);
	while(1);
}
#endif

