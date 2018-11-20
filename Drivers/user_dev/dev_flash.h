#ifndef USER_DEV_DEV_FLASH_H_
#define USER_DEV_DEV_FLASH_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_MIN_RW_SIZE sizeof(uint32_t)
#define F_SIZE_TO_CNT(val) (val / FLASH_MIN_RW_SIZE)

/*The addr it's between 0~2047. the size: 2048byte*/
uint32_t DevReadFlash(uint32_t addr, uint32_t len, uint32_t *buf);

/*The addr must be divisible 4, addr it's between 0~2047, size: 2048byte*/
uint32_t DevWriteFlash(uint32_t addr, uint32_t *buf, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif //USER_DEV_DEV_FLASH_H_

