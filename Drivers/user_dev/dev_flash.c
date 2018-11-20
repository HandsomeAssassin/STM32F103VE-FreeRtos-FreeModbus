#include "dev_flash.h"
#include <stdio.h>
#include "dev_port_config.h"
#include "cmsis_os.h"

#define DGB_INFO(fmt, args...) do { printf("[DEV_FLASH_INFO] %s(), %d,]"fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_WARN(fmt, args...) do { printf("[DEV_FLASH_WARN] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_ERROR(fmt, args...) do { printf("[DEV_FLASH_ERROR] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)

static uint32_t ReadFlash(uint32_t addr)
{
	return *(uint32_t *)addr;
}

int FlashNeedEase(uint32_t addr, uint32_t len)
{
	while(len-- && addr < DEV_MAX_FLASH_ADDR) {
		if(ReadFlash(addr++) != 0xff)
			return 1;
	}

	return 0;
}

uint32_t DevReadFlash(uint32_t addr, uint32_t count, uint32_t *buf)
{
	addr *= 4;
	addr += DEV_MIN_FLASH_ADDR;

	uint32_t i = 0;
	while(addr < DEV_MAX_FLASH_ADDR && count > 0) {
		*buf++ = ReadFlash(addr);
		addr += 4;
		count--;
		i++;
	}

	return i;
}

static int EaseFlash()
{
	FLASH_EraseInitTypeDef pEraseInit;
	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES; //erase one page
	pEraseInit.PageAddress = DEV_MIN_FLASH_ADDR;
	pEraseInit.NbPages = 1;
	uint32_t PageError;

	HAL_StatusTypeDef sta = HAL_FLASHEx_Erase(&pEraseInit, &PageError);
	if (sta != HAL_OK) {
		DGB_ERROR("flash erase error\n");
		return -1;
	}

	return 0;
}

uint32_t DevWriteFlash(uint32_t addr, uint32_t *buf, uint32_t count)
{
	HAL_StatusTypeDef sta;

	addr *= 4;
	addr += DEV_MIN_FLASH_ADDR;
	if (addr > DEV_MAX_FLASH_ADDR || addr % 4)
		return 0;

	HAL_FLASH_Unlock();
	if (FlashNeedEase(addr, count))
		EaseFlash();

	sta = FLASH_WaitForLastOperation(0XFFFFFFFF);
	if (sta != HAL_OK)
		DGB_WARN("wait flash error, %d\n", sta);

	int i =  0;
	while(count--) {
		sta = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, buf[i]);
		if (sta != HAL_OK) {
			DGB_WARN("write flash error, %d\n", sta);
			break;
		}
		addr += 4;
		i++;
	}
	sta = HAL_FLASH_Lock();
	if (sta != HAL_OK)
		DGB_WARN("Flash lock error\n");

	return i;
}



void FlashTest()
{
	uint32_t flag = 0;
	DevReadFlash(0, 1, &flag);
	printf("flag : %u\n", flag);
	
	uint32_t addr = 0;
	DevReadFlash(1, 1, &addr);
	printf("addr : %u\n", addr);

	uint32_t baud = 0;
	DevReadFlash(2, 1, &baud);
	printf("baud : %u\n", baud);

}

#define STACK 64
static osThreadId TaskHandle;

void task(const void *arg)
{
	FlashTest();
}

int FlashTestTask()
{
	osThreadDef(FLASHTEST, task, osPriorityNormal, 0, STACK);
  	TaskHandle = osThreadCreate(osThread(FLASHTEST), NULL);
	if (TaskHandle == NULL) {
		DGB_ERROR("Create Tread error\n");
		return -1;
	}

	return 0;
}

