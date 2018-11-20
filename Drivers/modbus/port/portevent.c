/*
 * FreeModbus Libary: STR71x Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- System includes ----------------------------------*/
#include "assert.h"

/* ----------------------- FreeRTOS -----------------------------------------*/
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#define DGB_INFO(fmt, args...) do { printf("[PORT_EVENT_INFO] %s(), %d,]"fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_WARN(fmt, args...) do { printf("[PORT_EVENT_WARN] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_ERROR(fmt, args...) do { printf("[PORT_EVENT_ERROR] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)

static xQueueHandle xMBPortQueueHdl;

BOOL
xMBPortEventInit( void )
{
    xMBPortQueueHdl = xQueueCreate(1, sizeof(eMBEventType));
    return xMBPortQueueHdl != NULL ? TRUE : FALSE;
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    portBASE_TYPE ret = pdFALSE, xEventSent = pdFALSE;

	ret = xQueueIsQueueFullFromISR(xMBPortQueueHdl);
	if (ret == pdTRUE)
		return FALSE;

    ret = xQueueSendFromISR(xMBPortQueueHdl, &eEvent, &xEventSent );


	if (xEventSent)
		portYIELD_FROM_ISR(xEventSent);

    return ret == pdTRUE ? TRUE : FALSE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    if(xQueueReceive( xMBPortQueueHdl, eEvent, portMAX_DELAY) == pdTRUE )
    	return TRUE;

    return FALSE;
}
#endif

