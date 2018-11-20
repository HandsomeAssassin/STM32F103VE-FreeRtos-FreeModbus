#ifndef APPL_CMD_CONSOLE_H_
#define APPL_CMD_CONSOLE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	CMD_SUCCESS,
	CMD_INVALID,
	CMD_BUSY,
	CMD_ERROR,
	CMD_STA_NUM,
} CmdSta;

typedef CmdSta(*Exec)(char *cmd);

typedef struct  {
	char *name;
	Exec exec; /*exec the cmd*/
} CmdData;

CmdSta CmdExec(char *cmd, const CmdData *cdata, int count);
int ApplConsoleStart(void);
int ApplConsoleStop(void);

#ifdef __cplusplus
}
#endif

#endif /* APPL_CMD_CONSOLE_H_ */

