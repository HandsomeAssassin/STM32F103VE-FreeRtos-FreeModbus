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
#include "cmsis_os.h"

/* ----------------------- Platform includes --------------------------------*/
#include "stm32f1xx_hal.h"
#include "dev_RS485.h"
#include "port.h"

#define prvvMBSerialIRQHandler USART3_IRQHandler

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Start implementation -----------------------------*/

BOOL
xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            xResult = TRUE;
    uint32_t eUARTParity;
	USART_TypeDef uart;

    (void)ucPort;

    switch ( eParity )
    {
        case MB_PAR_EVEN:
            eUARTParity = UART_PARITY_EVEN;
            break;
        case MB_PAR_ODD:
            eUARTParity = UART_PARITY_ODD;
            break;
        case MB_PAR_NONE:
            eUARTParity = UART_PARITY_NONE;
            break;
    }

    DevRS485Init(ulBaudRate, eUARTParity);

    return xResult;
}

void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if( xRxEnable ) {
        DevRS485RecEndIrq(1);
				RS485RWCtrl(RS485_R);
    } else {
        DevRS485RecEndIrq(0);
				RS485RWCtrl(RS485_W);
	}

    if( xTxEnable )
        DevRS485SendEndIrq(1);
    else
		DevRS485SendEndIrq(0);
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{

   	DevRS485Write((uint8_t *)&ucByte, 1);
	return 1;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    DevRS485Read((uint8_t *)pucByte, 1);
	return 1;
}

void
USART3_IRQHandler(void)
{
	//uart UART_FLAG_RXNE interrupt
 	if(DevRS485RxRecEndIrq() ) {
    	pxMBFrameCBByteReceived();
 	}

	//uart UART_FLAG_TC interrupt
	else if( DevRS485TxTcIrq()) {
    	pxMBFrameCBTransmitterEmpty();
	}

	DevRS485ClearOverFlow();
	DevRS485ClearFlag();
}
