/*  Virtually unaltered "blink" program to demonstrate uart_debug features
 *  Notice the calls to Debug_GPIOConfig() and Debug_UARTConfig() after
 *    configuring the system clock
 *  Notice one call to Debug_PrintLn(char *c) which is in the infinite loop
 *    and one call to Debug_PrintChar(char *c) which is in outside the 
 *    loop, in the main function. Again, these latter two functions can only
 *    be called after the two earliest mentioned functions are called
 */


#include <uart_debug.h>
#include <stm32f0xx.h>
#include <main.h>
#include <stdint.h>

static const uint32_t cycle_frequency = 100; /* Execute main_loop at 100Hz (10ms period) */
static const uint32_t systick_frequency = 10000; /* Timer frequency 10KHz (0.1ms period) */
static const uint32_t systick_priority = 0; /* Highest priority */


static void SystemClock_Config(void);
static void GPIO_Config(void);
static void main_loop(void);
int main(void);

/* Main execution loop runs at cycle_frequency Hz
 * Should never exceed 1/cycle_frequency seconds
 * HAL_Delay should never be used within loop */
void main_loop()
{

    Debug_PrintLn("I run infinitely!!!");
	static uint32_t cycle_count = 0;
	cycle_count++;

	/* Toggle LED every second */
	if(cycle_count % (systick_frequency/cycle_frequency) == 0)
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}
}

int main(void)
{
	/* Initialize Hardware Abstraction Library
	 * TODO: handle HAL init failure */
	(void)HAL_Init();

	/* Initialize clocks */
	SystemClock_Config();

	/* Set systick frequency
	 * TODO: handle systic config failure */
	(void)HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/systick_frequency);

	/* set Priority for Systick Interrupt. HAS to be after HAL_Init and HAL_SYSTICK_Config as those overwrite prioritt */
	NVIC_SetPriority(SysTick_IRQn, systick_priority);


	/* Initialize GPIOs */
	GPIO_Config();

    Debug_GPIOConfig();
    Debug_UARTConfig(); 
    
    Debug_PrintChar("I run once...");

	uint32_t timestamp = HAL_GetTick() + systick_frequency/cycle_frequency;
	while(1)
	{
		/* Execute main loop */
		main_loop();

		/* Deadline failure */
		if(HAL_GetTick() - timestamp >= systick_frequency/cycle_frequency)
		{
			/* TODO: Handle deadlile failure */
		}

		while(HAL_GetTick() - timestamp <= systick_frequency/cycle_frequency);
		timestamp = HAL_GetTick();
	}
}

static void GPIO_Config(void)
{
	/* Initialize Green LED GPIO on Nucleo board */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	/* Select HSI48 Oscillator as PLL source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;

	/* TODO: handle oscillator config failure */
	(void)HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/* Select PLL as system clock source and configure the HCLK and PCLK1 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	/* TODO: handle clock config failure */
	(void)HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

	/* Enable appropriate peripheral clocks */
	__SYSCFG_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();
}
