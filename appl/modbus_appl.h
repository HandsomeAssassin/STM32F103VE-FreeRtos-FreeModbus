#ifndef APPL_MODBUS_APPL_H_
#define APPL_MODBUS_APPL_H_

#include "stm32f1xx_hal.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/*if you don't set, the default addr is 1. You should set addr before Modbus start.*/
int ApplModbusSetSlaveAddr(uint8_t addr);

/*if you don't set, the default BadnRate is 460800. You should set 460800 before Modbus start.*/
int ApplModbusSetBaudRate(uint32_t val);
/*return baudrate*/
uint32_t ApplModbusBaudRate(void);

/*return addr*/
uint8_t ApplModbusSlaveAddr(void);

/*Init the Modbus slave and run it.*/
int ApplModbusThreadStart(void);

/*Stop Modbus Thread, and deinit modbus*/
int ApplModbusThreadStop(void);
int ApplModbusRunning(void);


#ifdef __cplusplus
}
#endif

#endif //APPL_MODBUS_APPL_H_

