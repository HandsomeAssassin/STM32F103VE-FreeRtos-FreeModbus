#ifndef APPL_EXDODI_APPL_H_
#define APPL_EXDODI_APPL_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t(*ReadExDi)(void);
typedef int (*SetExDo)(uint16_t);

int ApplExDoIoInit(ReadExDi, SetExDo);
int ApplExDoIoTaskStart(void);
int ApplExDoIoTaskStop(void);
void ApplExDoIoRefresh(void);
int ApplExDoIoRunning(void);

#ifdef __cplusplus
}
#endif

#endif //APPL_EXDODI_APPL_H_

