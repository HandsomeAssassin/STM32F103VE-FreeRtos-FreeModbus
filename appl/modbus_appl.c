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

//����Ĵ�����ʼ��ַ
#define REG_INPUT_START       0x0000
//����Ĵ�������
#define REG_INPUT_NREGS       8
//���ּĴ�����ʼ��ַ
#define REG_HOLDING_START     0x0000
//���ּĴ�������
#define REG_HOLDING_NREGS     8

//��Ȧ��ʼ��ַ
#define REG_COILS_START       0x0000
//��Ȧ����
#define REG_COILS_SIZE        16

//���ؼĴ�����ʼ��ַ
#define REG_DISCRETE_START    0x0000
//���ؼĴ�������
#define REG_DISCRETE_SIZE     16


/* Private variables ---------------------------------------------------------*/
//����Ĵ�������
static uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0};
//�Ĵ�����ʼ��ַ
static uint16_t usRegInputStart = REG_INPUT_START;

//���ּĴ�������
static uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0};
//���ּĴ�����ʼ��ַ
static uint16_t usRegHoldingStart = REG_HOLDING_START;

//��Ȧ״̬
static uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0};
//��������״̬
static uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0};

 /**
  * @brief  ����Ĵ���������������Ĵ����ɶ���������д��
  * @param  pucRegBuffer  ��������ָ��
  *         usAddress     �Ĵ�����ʼ��ַ
  *         usNRegs       �Ĵ�������
  * @retval eStatus       �Ĵ���״̬
  */
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int16_t         iRegIndex;

  //��ѯ�Ƿ��ڼĴ�����Χ��
  //Ϊ�˱��⾯�棬�޸�Ϊ�з�������
  if( ( (int16_t)usAddress >= REG_INPUT_START ) \
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
  {
    //��ò���ƫ���������β�����ʼ��ַ-����Ĵ����ĳ�ʼ��ַ
    iRegIndex = ( int16_t )( usAddress - usRegInputStart );
    //�����ֵ
    while( usNRegs > 0 )
    {
      //��ֵ���ֽ�
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] >> 8 );
      //��ֵ���ֽ�
      *pucRegBuffer++ = ( uint8_t )( usRegInputBuf[iRegIndex] & 0xFF );
      //ƫ��������
      iRegIndex++;
      //�������Ĵ��������ݼ�
      usNRegs--;
    }
  }
  else
  {
    //���ش���״̬���޼Ĵ���
    eStatus = MB_ENOREG;
  }

  return eStatus;
}

/**
  * @brief  ���ּĴ��������������ּĴ����ɶ����ɶ���д
  * @param  pucRegBuffer  ������ʱ--��������ָ�룬д����ʱ--��������ָ��
  *         usAddress     �Ĵ�����ʼ��ַ
  *         usNRegs       �Ĵ�������
  *         eMode         ������ʽ��������д
  * @retval eStatus       �Ĵ���״̬
  */
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
  //����״̬
  eMBErrorCode    eStatus = MB_ENOERR;
  //ƫ����
  int16_t         iRegIndex;

  //�жϼĴ����ǲ����ڷ�Χ��
  if( ( (int16_t)usAddress >= REG_HOLDING_START ) \
     && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
  {
    //����ƫ����
    iRegIndex = ( int16_t )( usAddress - usRegHoldingStart );

    switch ( eMode )
    {
      //��������
      case MB_REG_READ:
        while( usNRegs > 0 )
        {
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] >> 8 );
          *pucRegBuffer++ = ( uint8_t )( usRegHoldingBuf[iRegIndex] & 0xFF );
          iRegIndex++;
          usNRegs--;
        }
        break;

      //д������
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
    //���ش���״̬
    eStatus = MB_ENOREG;
  }

  return eStatus;
}


/**
  * @brief  ��Ȧ�Ĵ�������������Ȧ�Ĵ����ɶ����ɶ���д
  * @param  pucRegBuffer  ������---��������ָ�룬д����--��������ָ��
  *         usAddress     �Ĵ�����ʼ��ַ
  *         usNRegs       �Ĵ�������
  *         eMode         ������ʽ��������д
  * @retval eStatus       �Ĵ���״̬
  */
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
  //����״̬
  eMBErrorCode    eStatus = MB_ENOERR;
  //�Ĵ�������
  int16_t         iNCoils = ( int16_t )usNCoils;
  //�Ĵ���ƫ����
  int16_t         usBitOffset;

  //���Ĵ����Ƿ���ָ����Χ��
  if( ( (int16_t)usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
  {
    //����Ĵ���ƫ����
    usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
    switch ( eMode )
    {
      //������
      case MB_REG_READ:
        while( iNCoils > 0 )
        {
          *pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                          ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
          iNCoils -= 8;
          usBitOffset += 8;
        }
        break;

      //д����
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
  //����״̬
  eMBErrorCode    eStatus = MB_ENOERR;
  //�����Ĵ�������
  int16_t         iNDiscrete = ( int16_t )usNDiscrete;
  //ƫ����
  uint16_t        usBitOffset;

  //�жϼĴ���ʱ�����ƶ���Χ��
  if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
        ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
  {
    //���ƫ����
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
