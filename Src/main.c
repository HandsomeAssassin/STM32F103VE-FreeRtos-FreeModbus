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

typedef struct sc_context {
    scconf_context *conf;
    scconf_block *conf_blocks[3];
    char *app_name;
    int debug;
    unsigned long flags;
    FILE *debug_file;
    char *debug_filename;
    char *preferred_language;
    list_t readers;
    struct sc_reader_driver *reader_driver;
    void *reader_drv_data;
    struct sc_card_driver *card_drivers[SC_MAX_CARD_DRIVERS];
    struct sc_card_driver *forced_driver;
    sc_thread_context_t *thread_ctx;
    void *mutex;
    unsigned int magic;
} sc_context_t;

typedef struct {
    /** version number of this structure (0 for this version) */
    unsigned int  ver;
    /** name of the application (used for finding application
     *  dependent configuration data). If NULL the name "default"
     *  will be used. */
    const char    *app_name;
    /** context flags */
    unsigned long flags;
    /** mutex functions to use (optional) */
    sc_thread_context_t *thread_ctx;
} sc_context_param_t;
