#ifndef USER_DEV_DEV_RS485_H_
#define USER_DEV_DEV_RS485_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	RS485_W,
	RS485_R,
	RS485_NULL,
} RS485Mode;

int DevRS485Init(unsigned long badu_rate, uint32_t Parity);
int DevRS485DeInit(void);
int DevRS485RecEndIrq(uint8_t en);
int DevRS485SendEndIrq(uint8_t en);
int RS485RWCtrl(RS485Mode mode);
int DevRS485Write(uint8_t *data, uint32_t size);
int DevRS485Read(uint8_t *data, uint32_t size);
int DevRS485RxRecEndIrq(void);
int DevRS485TxTcIrq(void);
void DevRS485IrqHandle(void);
void DevRS485ClearOverFlow(void);
void DevRS485ClearFlag(void);

#ifdef __cplusplus
}
#endif

#endif //USER_DEV_DEV_PRINTF_H_

