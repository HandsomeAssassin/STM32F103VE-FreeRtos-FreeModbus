#ifndef USER_DEV_DEV_TIMER_H_
#define USER_DEV_DEV_TIMER_H_

#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TimerPeriodElapsedCallback)(void *arg);

int DevTimerInit(uint32_t period_n50us);
int DevTimerDeinit(void);
int DevTimerReset(void);
int DevTimerIrqEnable(int en);
void DevTimerSetCb(TimerPeriodElapsedCallback cb, void *arg);


#ifdef __cplusplus
}
#endif

#endif //USER_DEV_DEV_TIMER_H_
