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

/* ----------------------- FreeRTOS includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

/* ----------------------- STM32 includes ----------------------------------*/
#include "dev_timer.h"
#include "cmsis_os.h"


/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Static functions ---------------------------------*/
static void            prvvMBTimerExpired( void *arg);

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTimTimerout50us )
{
	DevTimerInit(usTimTimerout50us);
	DevTimerSetCb(prvvMBTimerExpired, NULL);

    return TRUE;
}

static void
prvvMBTimerExpired( void *arg )
{
	( void )pxMBPortCBTimerExpired();
}

inline void
vMBPortTimersEnable(  )
{
	DevTimerReset();
	DevTimerIrqEnable(1);
}

inline void
vMBPortTimersDisable(  )
{
  	DevTimerReset();
	DevTimerIrqEnable(0);
}

