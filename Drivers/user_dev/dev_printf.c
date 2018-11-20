#include <string.h>
#include <stdio.h>
#include "dev_printf.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "dev_port_config.h"

static osSemaphoreId semaphore;
static osSemaphoreDef(_semaphore);

static void OsSemInit(void)
{
	semaphore = osSemaphoreCreate (osSemaphore(_semaphore), 1);
	osSemaphoreWait(semaphore, 0xffffffff);
}

static UART_HandleTypeDef huart;

int DevPrintInit()
{
	OsSemInit();

	huart.Instance = PRINTF_UART;
  	huart.Init.BaudRate = PRINTF_BAUDRATE;
  	huart.Init.WordLength = UART_WORDLENGTH_8B;
  	huart.Init.StopBits = UART_STOPBITS_1;
  	huart.Init.Parity = UART_PARITY_NONE;
  	huart.Init.Mode = UART_MODE_TX_RX;
  	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  	huart.Init.OverSampling = UART_OVERSAMPLING_16;

  	if (HAL_UART_Init(&huart) != HAL_OK)
		while (1);

	while(!__HAL_UART_GET_FLAG(&huart, UART_FLAG_TC));

	HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	huart.Instance->SR = 0;

	return 0;
}


int fputc(int ch, FILE *f)
{
	huart.Instance->DR = (ch & (uint8_t)0xFF);
	while(!__HAL_UART_GET_FLAG(&huart, UART_FLAG_TC));

    return ch;
}

static void RxClearOverFlow(void)
{
	while((__HAL_UART_GET_FLAG(&huart, UART_FLAG_ORE) ? SET : RESET) == SET) {
		uint32_t temp = huart.Instance->SR;
		uint8_t ch =  (uint8_t)(huart.Instance->DR & (uint8_t)0x00FF);
	}
}

static char *rx_buf;
int DevGets(char *buf)
{
	rx_buf = buf;
	__HAL_UART_ENABLE_IT(&huart, UART_IT_RXNE);
	osSemaphoreWait(semaphore, 0xffffffff);

	return 0;
}

void
USART1_IRQHandler(void)
{
	if(__HAL_UART_GET_FLAG(&huart, UART_FLAG_RXNE) == SET) {
		*rx_buf = (uint8_t)(huart.Instance->DR & (uint8_t)0x00FF);
		if (*rx_buf == '\n') {
			*rx_buf = 0;
			osSemaphoreRelease(semaphore);
			__HAL_UART_DISABLE_IT(&huart, UART_IT_RXNE);
		} else if(*rx_buf == 0x0d) {
			*rx_buf = 0;
		}

		rx_buf++;
	}

	RxClearOverFlow();
	huart.Instance->SR = 0;
}

#if 0
/*demo*/
int __Write(uint8_t *data, uint32_t size)
{
	int i = 0;
	while(i < size) {
		while((__HAL_UART_GET_FLAG(&huart, UART_FLAG_TXE) ? SET : RESET) == RESET);
		huart.Instance->DR = (*data++ & (uint8_t)0xFF);
		i++;
	}
	huart.gState = HAL_UART_STATE_READY;
	return 0;
}

static int Read(uint8_t *data, uint32_t size)
{
	int i = 0;
	while(i < size) {
		while((__HAL_UART_GET_FLAG(&huart, UART_FLAG_RXNE) ? SET : RESET) == RESET);
		*data++ = (uint8_t)(huart.Instance->DR & (uint8_t)0x00FF);
		i++;
	}
	huart.RxState = HAL_UART_STATE_READY;
	return 0;
}

void PrintfTest()
{
	while(1)
		printf("hello\n");

}
#endif
