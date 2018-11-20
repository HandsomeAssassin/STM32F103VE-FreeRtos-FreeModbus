#include "dev_do.h"
#include "dev_port_config.h"

#define DGB_INFO(fmt, args...) do { printf("[DEV_DO_INFO] %s(), %d,]"fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_WARN(fmt, args...) do { printf("[DEV_DO_WARN] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_ERROR(fmt, args...) do { printf("[DEV_DO_ERROR] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)

int DevDoInit(void)
{
	GPIO_InitTypeDef io;
	GPIO_TypeDef *port;
	io.Mode = GPIO_MODE_OUTPUT_PP;
	io.Pull = GPIO_PULLUP;
	io.Speed = GPIO_SPEED_FREQ_HIGH;

	int len = sizeof(do_list) / sizeof(do_list[0]);
	if (len > DO_MAX_NUM)		//do max num is 16
		return -1;

	int i = 0;
	while(i < len) {
		port = do_list[i].port;
		io.Pin = do_list[i++].pin;
		HAL_GPIO_Init(port, &io);
	}

	return 0;
}

int DevDoDeinit(void)
{
	int len = sizeof(do_list) / sizeof(do_list[0]);
	int i = 0;
	if (len > 16)		//do max num is 16
		return -1;

	while(i < len) {
		HAL_GPIO_DeInit(do_list[i].port, do_list[i].pin);
		i++;
	}
	return 0;
}

int DevSetDo(uint16_t val)
{
	int len = sizeof(do_list) / sizeof(do_list[0]);
	if (len > DO_MAX_NUM)		//do max num is 16
		return -1;

	int i = 0;
	while(i < len) {
		GPIO_PinState sta = val & 1 ? GPIO_PIN_SET : GPIO_PIN_RESET;
		HAL_GPIO_WritePin(do_list[i].port, do_list[i].pin, sta);
		val >>= 1;
		i++;
	}

	return 0;
}

/*************/

void LED0(int val)
{
	GPIO_PinState sta = val & 1 ? GPIO_PIN_SET : GPIO_PIN_RESET;
	HAL_GPIO_WritePin(do_list[2].port, do_list[2].pin, sta);
}

