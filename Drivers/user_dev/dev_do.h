#ifndef USER_DEV_DEV_DO_H_
#define USER_DEV_DEV_DO_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int DevDoInit(void);
int DevDoDeinit(void);
int DevSetDo(uint16_t val);

#ifdef __cplusplus
}
#endif

#endif //USER_DEV_DEV_DO_H_

