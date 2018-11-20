#include "exdoio_appl.h"
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"
#include "mb.h"

#define DGB_INFO(fmt, args...) do { printf("[APPL_EXDOIO_INFO] %s(), %d,"fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_WARN(fmt, args...) do { printf("[APPL_EXDOIO_WARN] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_ERROR(fmt, args...) do { printf("[APPL_EXDOIO_ERRIR] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)

#define EXDO_ADDR 0
#define EXDO_LENGTH 1 //16BIT

#define EXDI_ADDR 1
#define EXDI_LENGTH 2 //32BIT

#define EXDOIO_STACK 64
static osThreadId TaskHandle;
static int task_run = 0;

static ReadExDi _ReadExDi = NULL;
static SetExDo _SetExDo = NULL;

static osSemaphoreId semaphore;
static osSemaphoreDef(_semaphore);

static void OsSemInit(void)
{
	semaphore = osSemaphoreCreate (osSemaphore(_semaphore), 1);
}

static int OsSemTake(uint32_t lTimeOut)
{
    /*If waiting time is -1 .It will wait forever */

	if (osSemaphoreWait(semaphore, lTimeOut) == osOK)
		return 1;

    return  0;
}

void ApplExDoIoRefresh(void)
{
   	/* release resource */
	if (semaphore)
		osSemaphoreRelease(semaphore);
}

static void ExDoIoTask(void const *arg)
{
	uint16_t exdo = 0;
	uint32_t io = 0;

	task_run = 1;
	while(task_run) {
		OsSemTake(0xffffffff);
		io = _ReadExDi();
		eMBRegHoldingCB((uint8_t *)&io, EXDI_ADDR, EXDI_LENGTH, MB_REG_WRITE);
		eMBRegHoldingCB((uint8_t *)&exdo, EXDO_ADDR, EXDO_LENGTH, MB_REG_READ);
		_SetExDo(exdo);
	}

	osThreadTerminate (TaskHandle);
}

int ApplExDoIoInit(ReadExDi di, SetExDo exdo)
{
	_ReadExDi = di;
	_SetExDo = exdo;
	OsSemInit();

	return 0;
}

int ApplExDoIoTaskStart()
{
	if (_ReadExDi == NULL || _SetExDo == NULL || task_run)
		return -1;

	osThreadDef(ApplExDoIoTask, ExDoIoTask, osPriorityNormal, 0, EXDOIO_STACK);
  	TaskHandle = osThreadCreate(osThread(ApplExDoIoTask), NULL);
	if (TaskHandle == NULL) {
		DGB_ERROR("Create Tread error\n");
		return -1;
	}

	return 0;
}

int ApplExDoIoTaskStop(void)
{
	task_run = 0;
	return 0;
}

int ApplExDoIoRunning()
{
	return task_run;
}
