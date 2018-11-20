# STM32F103VE-FreeRtos-FreeModbus
This project is the platform is stm32f103vet6 + FreeRtos.

Global：

	1.Compilation tool ：keil mdk, STM32CubeMX

	2.Software composition：hal（stm32f103）, user dev(RS485 ...),FreeRtos(v9.0.0), 
	                        FreeModbus(link:https://github.com/cwalter-at/freemodbus)

	3.APP: ModbusSlave-rtu, IO control

Function：

	1.Use Modbus to control the IO

	2.Console：used the console to config the modbus,control the board and debug.
