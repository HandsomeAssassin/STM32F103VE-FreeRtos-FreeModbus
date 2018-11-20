#include "main.h"
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"

int FlashTestTask(void);
void FlashTest(void);

int main(void)
{
	PlatfromInit();

	ApplExDoIoInit(DevReadDi, DevSetDo);
	ApplExDoIoTaskStart();
  	ApplModbusThreadStart();

	//FlashTestTask();
	//FlashTest();

	osKernelStart();

  	while (1)
  		printf("error\n");

}
