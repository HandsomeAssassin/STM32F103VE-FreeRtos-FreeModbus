#include "cmd_modbus.h"
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"
#include "modbus_appl.h"
#include "mb.h"
#include "mbutils.h"

#define MAX_BAUD_RATE 460800

static CmdSta ModbusStopAppl(char *cmd)
{
	if (!ApplModbusRunning()) {
		printf("Modbus appl already stop\n");
		return CMD_ERROR;
	}

	int ret =  ApplModbusThreadStop();
	if (ret == 0)
		return CMD_SUCCESS;

	return CMD_ERROR;
}

static CmdData ModbusStopCmds[] = {
	{"appl", ModbusStopAppl},
};

static CmdSta ModbusStopExec(char *cmd)
{
	return CmdExec(cmd, ModbusStopCmds, sizeof(ModbusStopCmds) / sizeof(ModbusStopCmds[0]));
}


static CmdSta ModbusStartAppl(char *cmd)
{
	if (ApplModbusRunning()) {
		printf("Modbus appl already start\n");
		return CMD_ERROR;
	}

	int ret = ApplModbusThreadStart();
	if (ret == 0)
		return CMD_SUCCESS;

	return CMD_ERROR;
}

static CmdData ModbusStartCmds[] = {
	{"appl", ModbusStartAppl},
};

static CmdSta ModbusStartExec(char *cmd)
{
	return CmdExec(cmd, ModbusStartCmds, sizeof(ModbusStartCmds) / sizeof(ModbusStartCmds[0]));
}

static CmdSta ModbusSetBaudrate(char *cmd)
{
	uint32_t val;
	int cnt = sscanf(cmd, "%u", &val);
	if (cnt != 1)
		return CMD_INVALID;

	if (val > MAX_BAUD_RATE) {
		printf("The baudRate must be small then %u bit/s\n", MAX_BAUD_RATE);
		return CMD_ERROR;
	}

	ApplModbusSetBaudRate(val);

	printf("Set BaudRate = %u, it's valid after reboot modbus\n", val);

	return CMD_SUCCESS;
}

static CmdSta ModbusSetSlaveAddr(char *cmd)
{
	uint32_t addr;
	int cnt = sscanf(cmd, "%u", &addr);
	if (cnt != 1)
		return CMD_INVALID;

	if (addr > 0xff)
		return CMD_ERROR;

	ApplModbusSetSlaveAddr((uint8_t)addr);

	printf("Settings are valid after reboot modbus\n");

	return CMD_SUCCESS;
}

static CmdSta ModbusReadConfig(char *cmd)
{
	uint32_t baud = ApplModbusBaudRate();
	uint8_t addr = ApplModbusSlaveAddr();

	printf("BaudRate = %u, addr = %d\n", baud, addr);
	return CMD_SUCCESS;
}

static CmdData ModbusConfigCmds[] = {
	{"baudrate", ModbusSetBaudrate},
	{"slave_addr", ModbusSetSlaveAddr},
	{"read", ModbusReadConfig},
};
static CmdSta ModbusConfigExec(char *cmd)
{
	return CmdExec(cmd, ModbusConfigCmds, sizeof(ModbusConfigCmds) / sizeof(ModbusConfigCmds[0]));
}

static CmdData ModbusCmds[] = {
	{"config", ModbusConfigExec},
	{"start", ModbusStartExec},
	{"stop", ModbusStopExec},
	//{"read", },
	//{"write", }
};

CmdSta CmdModbusExec(char *cmd)
{
	return CmdExec(cmd, ModbusCmds, sizeof(ModbusCmds) / sizeof(ModbusCmds[0]));
}
