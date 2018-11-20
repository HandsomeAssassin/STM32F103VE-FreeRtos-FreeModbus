#ifndef CMD_CMD_MODUBS_H_
#define CMD_CMD_MODUBS_H_

#include <stdint.h>
#include "console.h"

#ifdef __cplusplus
extern "C" {
#endif

CmdSta CmdModbusExec(char *cmd);

#ifdef __cplusplus
}
#endif

#endif /* CMD_CMD_MODUBS_H_ */
