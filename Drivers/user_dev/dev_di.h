#ifndef USER_DEV_DEV_DI_H_
#define USER_DEV_DEV_DI_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int DevDiInit(void);
int DevDiDeinit(void);
uint32_t DevReadDi(void);

#ifdef __cplusplus
}
#endif

#endif //USER_DEV_DEV_DI_H_

