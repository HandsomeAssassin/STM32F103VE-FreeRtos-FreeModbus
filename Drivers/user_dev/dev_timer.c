#include <stdio.h>
#include "dev_timer.h"
#include "dev_port_config.h"

#define DGB_INFO(fmt, args...) //do { printf("[DEV_TIMER_INFO] %s(), %d,"fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_WARN(fmt, args...) do { printf("[DEV_TIMER_WARN] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)
#define DGB_ERROR(fmt, args...) do { printf("[DEV_TIMER_ERROR] %s(), %d, "fmt, __func__, __LINE__, ##args);}while(0)

static TIM_HandleTypeDef htim;

int DevTimerInit(uint32_t period_n50us)
{
	DGB_INFO("period_n50us : %lu\n", period_n50us);
	__HAL_RCC_TIM3_CLK_ENABLE();
	HAL_NVIC_SetPriority(TIM3_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);

	htim.Instance = TIMER;
	htim.Init.Prescaler = 3599;		 	//The Timer clk is 72mhz£¬clock division 71+1
	htim.Init.Period = period_n50us - 1;//* 50 - 1;	//Ther Timer Interrupt period us, period = period + 1
	htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim.Init.CounterMode = TIM_COUNTERMODE_UP;

	HAL_TIM_Base_Init(&htim);

	return 0;
}

int DevTimerDeinit(void)
{
	return 0;
}

int DevTimerReset()
{
	__HAL_TIM_SET_COUNTER(&htim, 0);
	 __HAL_TIM_CLEAR_IT(&htim, TIM_IT_UPDATE);
	return 0;
}

int DevTimerIrqEnable(int en)
{
	en? HAL_TIM_Base_Start_IT(&htim) : HAL_TIM_Base_Stop_IT(&htim);

	return 0;
}

void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim);
}

static TimerPeriodElapsedCallback Tim3Cb = NULL;
static void *Tim3CbArg = NULL;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *tim)
{
	if(tim == &htim && Tim3Cb!= NULL) {
		Tim3Cb(Tim3CbArg);
	}
}

void DevTimerSetCb(TimerPeriodElapsedCallback cb, void *arg)
{
	Tim3Cb = cb;
	Tim3CbArg = arg;
}

/********************/
#if 0
#include "cmsis_os.h"
#include "dev_delay.h"

static void LED_Init()
{
	GPIO_InitTypeDef io;
	io.Mode = GPIO_MODE_OUTPUT_PP;
	io.Pull = GPIO_PULLUP;
	io.Speed = GPIO_SPEED_FREQ_HIGH;
	io.Pin = GPIO_PIN_12;

	HAL_GPIO_Init(GPIOC, &io);
}

static void TestCb(void *arg)
{
	static GPIO_PinState sta = GPIO_PIN_SET;

	sta ? (sta = GPIO_PIN_RESET) : (sta = GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, sta);
}

void TimerTest()
{
	LED_Init();
	DevTimerInit(10);	//period 10 * 50us
	DevTimerIrqEnable(1);

	DevTimerSetCb(TestCb, NULL);

	Dev_DelayMs(10000 * 8);
	DevTimerIrqEnable(0);
}
#endif

