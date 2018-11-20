#include "cmd_io.h"
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"
#include "dev_printf.h"
#include "dev_di.h"
#include "dev_do.h"

static CmdSta IoSetDo(char *cmd)
{
	if (ApplExDoIoRunning()) {
		printf("You need stop the io appl, then run this func\n");
		return CMD_ERROR;
	}


	uint32_t val;
	int cnt = sscanf(cmd, "0x%x", &val);
	if (cnt != 1)
		return CMD_INVALID;

	printf("EXDO: 0x%x\n",val);
	DevSetDo((uint16_t)val);

	return CMD_SUCCESS;
}

static CmdSta IoReadDi(char *cmd)
{
	uint32_t val = DevReadDi();
	printf("EXDI: 0x%x\n", val);

	return CMD_SUCCESS;
}

static CmdSta IoStopAppl(char *cmd)
{
	if (!ApplExDoIoRunning()) {
		printf("Io appl already stop\n");
		return CMD_ERROR;
	}

	int ret = ApplExDoIoTaskStop();
	if (ret == 0)
		return CMD_SUCCESS;

	return CMD_ERROR;
}

static CmdSta IoStartAppl(char *cmd)
{
	if (ApplExDoIoRunning()) {
		printf("Io appl already running\n");
		return CMD_ERROR;
	}

	int ret = ApplExDoIoTaskStart();
	if (ret == 0)
		return CMD_SUCCESS;

	return CMD_ERROR;
}

static CmdData IoCmds[] = {
	{"set", IoSetDo},
	{"read", IoReadDi},
	{"startAppl", IoStartAppl},
	{"stopAppl", IoStopAppl},
};

CmdSta CmdIoExec(char *cmd)
{
	return CmdExec(cmd, IoCmds, sizeof(IoCmds) / sizeof(IoCmds[0]));
}

