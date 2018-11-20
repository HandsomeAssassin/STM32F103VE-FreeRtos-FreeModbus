#include "dev_di.h"
#include "dev_port_config.h"

#define DGB_INFO(fmt, args...) do { printf("[DEV_DI_INFO] %s(), %d,]"fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_WARN(fmt, args...) do { printf("[DEV_DI_WARN] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_ERROR(fmt, args...) do { printf("[DEV_DI_ERROR] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)

int DevDiInit(void)
{
	GPIO_InitTypeDef io;
	GPIO_TypeDef *port;
	io.Mode = GPIO_MODE_INPUT;
	io.Pull = GPIO_PULLDOWN;
	io.Speed = GPIO_SPEED_FREQ_HIGH;

	int len = sizeof(di_list) / sizeof(di_list[0]);
	if (len > DI_MAX_NUM)		//di max num is 16
		return -1;

	int i = 0;
	while(i < len) {
		port = di_list[i].port;
		io.Pin = di_list[i++].pin;
		HAL_GPIO_Init(port, &io);
	}

	return 0;
}

int DevDiDeinit(void)
{
	int len = sizeof(di_list) / sizeof(di_list[0]);
	int i = 0;
	if (len > DI_MAX_NUM)		//di max num is 32
		return -1;

	while(i < len) {
		HAL_GPIO_DeInit(di_list[i].port, di_list[i].pin);
		i++;
	}
	return 0;
}

uint32_t DevReadDi(void)
{
	uint32_t val = 0;

	int len = sizeof(di_list) / sizeof(di_list[0]);
	if (len > DI_MAX_NUM)		//di max num is 32
		return 0;

	int i = 0;
	while(i < len) {
		val |= HAL_GPIO_ReadPin(di_list[i].port, di_list[i].pin) << i;
		i++;
	}

	return val;
}
