#include "modbus_appl.h"
#include "string.h"
#include <stdio.h>
#include "cmsis_os.h"
#include "mb.h"
#include "mbutils.h"
#include "dev_flash.h"

#define DGB_INFO(fmt, args...) //do { printf("[APPL_MODBUS_INFO] %s(), %d,"fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_WARN(fmt, args...) do { printf("[APPL_MODBUS_WARN] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_ERROR(fmt, args...) do { printf("[APPL_MODBUS_ERRIR] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)

#define DEF_MODBUS_SLAVE_ADDR 0X01
#define DEF_MODBUS_BAUD_RATE  460800//115200
#define MODBUS_SERIAL_PORT 0X01 //NO effect, is set in dev_port_config.h->RS485 set, usart3

#define ALIGN_4BYTE(val) ((val - 1) / 4 + 1)

#define WRITE_FLAG_IN_FLASH 0
#define WRITE_FLAG 0x44556677
#define SLAVE_ADDR_IN_FLASH (WRITE_FLAG_IN_FLASH + F_SIZE_TO_CNT(sizeof(WRITE_FLAG)))
static uint32_t slave_addr = DEF_MODBUS_SLAVE_ADDR;

#define BAUD_RATE_IN_FLASH (SLAVE_ADDR_IN_FLASH + F_SIZE_TO_CNT(sizeof(slave_addr)))
static uint32_t modbus_baud_rate = DEF_MODBUS_BAUD_RATE;

//输入寄存器起始地址
#define REG_INPUT_START       0x0000
//输入寄存器数量
#define REG_INPUT_NREGS       8
//保持寄存器起始地址
#define REG_HOLDING_START     0x0000
//保持寄存器数量
#define REG_HOLDING_NREGS     8

//线圈起始地址
#define REG_COILS_START       0x0000
//线圈数量
#define REG_COILS_SIZE        16

//开关寄存器起始地址
#define REG_DISCRETE_START    0x0000
//开关寄存器数量
#define REG_DISCRETE_SIZE     16


/* Private variables ---------------------------------------------------------*/
//输入寄存器内容
static uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0};
//寄存器起始地址
static uint16_t usRegInputStart = REG_INPUT_START;

//保持寄存器内容
static uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0};
//保持寄存器起始地址
static uint16_t usRegHoldingStart = REG_HOLDING_START;

//线圈状态
static uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0};
//开关输入状态
static uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0};

 /**
  * @brief  输入寄存器处理函数，输入寄存器可读，但不可写。
  * @param  pucRegBuffer  返回数据指针
  *         usAddress     寄存器起始地址
  *         usNRegs       寄存器长度
  * @retval eStatus       寄存器状态
  */
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int16_t         iRegIndex;

  //查询是否在寄存器范围内
  //为了避免警告，修改为有符号整数
  if( ( (int16_t)usAddress >= REG_INPUT_START ) \
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
  {
    //获得操作偏移量，本次操作起始地址-输入寄存器的初始地址
    iRegIndex = ( int16_t )( usAddress - usRegInputStart );
    //逐个赋值
    while( usNRegs > 0 )
    {
      //赋值高字节
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] >> 8 );
      //赋值低字节
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] & 0xFF );
      //偏移量增加
      iRegIndex++;
      //被操作寄存器数量递减
      usNRegs--;
    }
  }
  else
  {
    //返回错误状态，无寄存器
    eStatus = MB_ENOREG;
  }

  return eStatus;
}

/**
  * @brief  保持寄存器处理函数，保持寄存器可读，可读可写
  * @param  pucRegBuffer  读操作时--返回数据指针，写操作时--输入数据指针
  *         usAddress     寄存器起始地址
  *         usNRegs       寄存器长度
  *         eMode         操作方式，读或者写
  * @retval eStatus       寄存器状态
  */
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
  //错误状态
  eMBErrorCode    eStatus = MB_ENOERR;
  //偏移量
  int16_t         iRegIndex;

  //判断寄存器是不是在范围内
  if( ( (int16_t)usAddress >= REG_HOLDING_START ) \
     && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
  {
    //计算偏移量
    iRegIndex = ( int16_t )( usAddress - usRegHoldingStart );

    switch ( eMode )
    {
      //读处理函数
      case MB_REG_READ:
        while( usNRegs > 0 )
        {
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] >> 8 );
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] & 0xFF );
          iRegIndex++;
          usNRegs--;
        }
        break;

      //写处理函数
      case MB_REG_WRITE:
        while( usNRegs > 0 )
        {
          usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
          usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
          iRegIndex++;
          usNRegs--;
        }
        break;
     }
  }
  else
  {
    //返回错误状态
    eStatus = MB_ENOREG;
  }

  return eStatus;
}


/**
  * @brief  线圈寄存器处理函数，线圈寄存器可读，可读可写
  * @param  pucRegBuffer  读操作---返回数据指针，写操作--返回数据指针
  *         usAddress     寄存器起始地址
  *         usNRegs       寄存器长度
  *         eMode         操作方式，读或者写
  * @retval eStatus       寄存器状态
  */
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
  //错误状态
  eMBErrorCode    eStatus = MB_ENOERR;
  //寄存器个数
  int16_t         iNCoils = ( int16_t )usNCoils;
  //寄存器偏移量
  int16_t         usBitOffset;

  //检查寄存器是否在指定范围内
  if( ( (int16_t)usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
  {
    //计算寄存器偏移量
    usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
    switch ( eMode )
    {
      //读操作
      case MB_REG_READ:
        while( iNCoils > 0 )
        {
          *pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                          ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
          iNCoils -= 8;
          usBitOffset += 8;
        }
        break;

      //写操作
      case MB_REG_WRITE:
        while( iNCoils > 0 )
        {
          xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
                        ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ),
                        *pucRegBuffer++ );
          iNCoils -= 8;
        }
        break;
    }

  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  //错误状态
  eMBErrorCode    eStatus = MB_ENOERR;
  //操作寄存器个数
  int16_t         iNDiscrete = ( int16_t )usNDiscrete;
  //偏移量
  uint16_t        usBitOffset;

  //判断寄存器时候再制定范围内
  if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
        ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
  {
    //获得偏移量
    usBitOffset = ( uint16_t )( usAddress - REG_DISCRETE_START );

    while( iNDiscrete > 0 )
    {
      *pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
                                      ( uint8_t)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
      iNDiscrete -= 8;
      usBitOffset += 8;
    }

  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

static int ModbusChcekWriteFlashFlag()
{
	uint32_t data;
	DevReadFlash(WRITE_FLAG_IN_FLASH, F_SIZE_TO_CNT(sizeof(data)), &data);
	if (data != WRITE_FLAG)
		return -1;

	return 0;
}

static int ModbusWrieConfigDataToFlash()
{
	uint32_t buf[] = {WRITE_FLAG, slave_addr, modbus_baud_rate};
	DevWriteFlash(WRITE_FLAG_IN_FLASH, buf, F_SIZE_TO_CNT(sizeof(buf)));

	return 0;
}

static uint8_t ModbusSlaveAddr()
{
	DevReadFlash(DEF_MODBUS_SLAVE_ADDR, F_SIZE_TO_CNT(sizeof(slave_addr)), &slave_addr);
	return (uint8_t)slave_addr;
}

static uint32_t ModbusBoudRate()
{
	DevReadFlash(BAUD_RATE_IN_FLASH, F_SIZE_TO_CNT(sizeof(modbus_baud_rate)),  &modbus_baud_rate);
	return modbus_baud_rate;
}

static int ModbusInit()
{
	if (ModbusChcekWriteFlashFlag() != 0) {
		ModbusWrieConfigDataToFlash();
		printf("write def data to flash\n");
	}

	eMBInit(MB_RTU, ModbusSlaveAddr(), MODBUS_SERIAL_PORT, ModbusBoudRate(), MB_PAR_NONE);
	DGB_INFO("modbus init end\n");

	eMBEnable();
	DGB_INFO("modbus enable end\n");

	return 0;
}

static int ModbulDeInit()
{
	eMBDisable();
	eMBClose();

	return 0;
}

#define MODBUS_STACK 128
static osThreadId ModbusTaskHandle;
static int modbus_task_run;

static void ModbusTask(void const * argument)
{
	modbus_task_run = 1;
	ModbusInit();

	while(modbus_task_run) {
		eMBPoll();
	}

	osThreadTerminate (ModbusTaskHandle);
}

int ApplModbusSetSlaveAddr(uint8_t addr)
{
	slave_addr = addr;
	ModbusWrieConfigDataToFlash(); //Reflash data to flash
	return 0;
}

int ApplModbusSetBaudRate(uint32_t val)
{
	modbus_baud_rate = val;
	ModbusWrieConfigDataToFlash(); //Reflash data to flash
	return 0;
}

uint32_t ApplModbusBaudRate(void)
{
	return modbus_baud_rate;
}

uint8_t ApplModbusSlaveAddr(void)
{
	return slave_addr;
}

int ApplModbusThreadStart(void)
{
	osThreadDef(FreeModbusTask, ModbusTask, osPriorityNormal, 0, MODBUS_STACK);
  	ModbusTaskHandle = osThreadCreate(osThread(FreeModbusTask), NULL);
	if (ModbusTaskHandle == NULL) {
		DGB_ERROR("Create Tread error\n");
		return -1;
	}

	return 0;
}

int ApplModbusThreadStop(void)
{
	modbus_task_run = 0;
	osDelay(10);
	ModbulDeInit();

	return 0;
}

int ApplModbusRunning()
{
	return modbus_task_run;
}
