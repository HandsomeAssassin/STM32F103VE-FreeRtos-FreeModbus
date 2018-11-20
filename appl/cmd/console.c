#include "console.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "cmsis_os.h"
#include "dev_printf.h"
#include "cmds.h"

#define DGB_INFO(fmt, args...) do { printf("[CMD_INFO] %s(), %d,]"fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_WARN(fmt, args...) do { printf("[CMD_WARN] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_ERROR(fmt, args...) do { printf("[CMD_ERROR] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)

#define WAIT_CONSOLE_MS 3000

static void TimerCb(void const *arg);
static osTimerId TimerId;
static osTimerDef(Timer, TimerCb);

static int ConsoleEnableMonitorStart()
{
	TimerId = osTimerCreate (osTimer(Timer), osTimerOnce, NULL);

	osStatus ret = osTimerStart (TimerId, WAIT_CONSOLE_MS);
	if (ret != osOK) {
		DGB_ERROR("Timer start error\n");
		return -1;
	}

	return 0;
}

static int ConsoleEnableMonitorStop()
{
	osStatus ret = osTimerStop (TimerId);
	if (ret != osOK) {
		DGB_ERROR("Timer stop error\n");
		return -1;
	}

	ret = osTimerDelete (TimerId);
	if (ret != osOK) {
		DGB_ERROR("Timer dekete error\n");
		return -1;
	}

	return 0;
}

#define TASK_STACK 128
static osThreadId TaskHandle;

static void TimerCb(void const *arg)
{
	printf("$ Sys started, delete console\n");
	osThreadTerminate (TaskHandle);
	ConsoleEnableMonitorStop();
}

/* cmd format: <command-name> <arg>... */
CmdSta CmdExec(char *cmd, const CmdData *cdata, int count)
{
	int i;
	char *args = NULL;

	args = strchr(cmd, ' ');
	if (args) {
		*args++ = '\0'; /* has arguments */
	}

	for (i = 0; i < count; ++i, ++cdata) {
		if (strcmp(cmd, cdata->name) == 0) {
			return cdata->exec(args ? args : "");
		}
	}

	DGB_ERROR("unknown cmd '%s'\n", cmd);
	return CMD_INVALID;
}

static int CmdGetInput(char *buf, int count)
{
	buf[0] = 0;
	DevGets(buf);

	return 0;
}

static CmdSta CmdClearConsloe(char *cmd)
{
	printf("\033[2J");

	return CMD_SUCCESS;
}

static CmdSta CmdConsoleStop(char *cmd)
{
	ApplConsoleStop();

	return CMD_SUCCESS;
}

static CmdData ConcleCmds[] = {
	{"exit", CmdConsoleStop},
};

static CmdSta CmdConsoleExec(char *cmd)
{
	return CmdExec(cmd, ConcleCmds, sizeof(ConcleCmds) / sizeof(ConcleCmds[0]));
}

#define BUF_SIZE 256
static char console_buf[BUF_SIZE];
static int console_run;

static CmdData ApplCmds[] = {
	{"Console", CmdConsoleExec},
	{"IO", CmdIoExec},
	{"Modbus", CmdModbusExec}
};

static char *CmdStaPrint[CMD_STA_NUM] = {
	"cmd success", "cmd invalid",
	"cmd busy", "cmd error",
};

static void ConsoleTask(void const *arg)
{
	printf("$ If you want into console, please input Enter(in 3s)\n");

	/*create a os timer to kill console task after WAIT_CONSOLE_MS ms*/
	ConsoleEnableMonitorStart();
	/*Wait input some string, string must end with '\n'*/
	DevGets(console_buf);
	/*stop and delete the ostimer, the console is run*/
	ConsoleEnableMonitorStop();

	printf("$ Into console\n");

	CmdSta sta;

	while(console_run) {
		CmdGetInput(console_buf, BUF_SIZE);
		if (strcmp(console_buf, "clear") == 0) {
			CmdClearConsloe(console_buf);
			continue;
		} else if(console_buf[0] == 0) {
			printf("Rokae$ \n");
			continue;
		}

		sta = CmdExec(console_buf, ApplCmds, sizeof(ApplCmds) / sizeof(ApplCmds[0]));
		printf("Rokae$ %s\n", CmdStaPrint[sta]);
	}

	osThreadTerminate (TaskHandle);
}

int ApplConsoleStart(void)
{
	console_run = 1;
	osThreadDef(ApplConsoleTask, ConsoleTask, osPriorityNormal, 0, TASK_STACK);
  	TaskHandle = osThreadCreate(osThread(ApplConsoleTask), NULL);
	if (TaskHandle == NULL) {
		DGB_ERROR("Create Tread error\n");
		return -1;
	}

	return 0;
}

int ApplConsoleStop(void)
{
	console_run = 0;
	osThreadTerminate (TaskHandle);
	return 0;
}

