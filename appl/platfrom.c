#include "platfrom.h"
#include "dev_printf.h"
#include "dev_sys_clk.h"
#include "dev_do.h"
#include "dev_di.h"
#include "modbus_appl.h"
#include "console.h"
#include "version.h"

#define DEV_PRO_DGB_INFO(fmt, args...) do { printf("[DEV_PLATFROM_INFO %s(), %d,]"fmt, __func__, __LINE__, ##args);}while(0)
#define DEV_PRO_DGB_WARN(fmt, args...) do { printf("[DEV_PLATFROM_WARN] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)
#define DEV_PRO_DGB_ERROR(fmt, args...) do { printf("[DEV_PLATFROM_ERROR] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)

static int DevHalInit()
{
	HAL_Init();
	return 0;
}

static int DevGPIOInit(void)
{

  	/* GPIO Ports Clock Enable */
 	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

  	return 0;

}

typedef	int (*InitFunc)(void);

static InitFunc list[] = {
	DevHalInit,				//0
	SystemClock_Config,		//1
	DevGPIOInit, 			//2
	DevPrintInit,			//3
	DevDoInit,				//4
	DevDiInit,				//5
	ApplConsoleStart,       //6
};

int PlatfromInit(void)
{
	int len = sizeof(list) / sizeof(list[0]);
	int i = 0;
	while(i < len) {
		if (list[i]() != 0) {
			DEV_PRO_DGB_ERROR("Init error, errnum: %d", i);
			return -1;
		}

		i++;
	}

	printf("Release date : %s %s\n", __DATE__, __TIME__);
	printf("Rokae IO RS485-MODBUS-TRU version :%s\n", VERSION_STR);
	return 0;
}

