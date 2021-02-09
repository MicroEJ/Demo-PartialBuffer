/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the STM32F7508 Discovery BSP Drivers
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * Copyright 2020 MicroEJ Corp. This file has been modified by MicroEJ Corp.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "microej_main.h"
#include "cpuload.h"
#include "memory.h"
#include "stm32f7xx_ll_system.h"
#include "display_configuration.h"
#ifndef COREMARK
#include "FreeRTOS.h"
#include "task.h"
#ifndef IPERF
#include "watchdog.h"
#include "watchdog_config.h"
#endif
#include "SEGGER_RTT.h"
#include "SEGGER_SYSVIEW.h"
#endif

/** @addtogroup STM32F7xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MICROJVM_STACK_SIZE      (12 * 1024)
#define JAVA_TASK_PRIORITY       ( 11 ) /** Should be > tskIDLE_PRIORITY & < configTIMER_TASK_PRIORITY */
#define JAVA_TASK_STACK_SIZE     MICROJVM_STACK_SIZE/4

#define IPERF_STACK_SIZE         (12 * 1024)
#define IPERF_TASK_PRIORITY      ( 11 ) /** Should be > tskIDLE_PRIORITY & < configTIMER_TASK_PRIORITY */
#define IPERF_TASK_STACK_SIZE    IPERF_STACK_SIZE/4

/* For partial buffer purpose */
extern uint8_t frame_buffer_mem[];
#define FRAME_BUFFER ((int32_t)&frame_buffer_mem[0])

extern uint8_t back_buffer_mem[];
#define BACK_BUFFER ((int32_t)&back_buffer_mem[0])

#ifdef DUAL_BACK_BUFFER_MODE_ENABLED
	extern uint8_t back_buffer_B_mem[];
	#define BACK_BUFFER_B ((int32_t)&back_buffer_B_mem[0])
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Global extern variables ---------------------------------------------------*/
#ifndef USE_FULL_ASSERT
uint32_t ErrorCounter = 0;
#endif

#if defined(__GNUC__)
	extern int _java_heap_start;
	extern int _java_heap_end;
	extern int _java_immortals_start;
	extern int _java_immortals_end;
	extern int _microui_heap_start;
	extern int _microui_heap_end;
#elif defined(__ICCARM__)
	extern int RW_MICROEJ_JAVA_HEAP$$Base;
	extern int RW_MICROEJ_JAVA_HEAP$$Limit;
	extern int RW_MICROEJ_IMMORTALS$$Base;
	extern int RW_MICROEJ_IMMORTALS$$Limit;
	extern int RW_MICROUI_HEAP$$Base;
	extern int RW_MICROUI_HEAP$$Limit;
#else
	#error "Java heap not initialized for this toolchain !"
#endif

#if defined(__GNUC__)
	extern int _siitcm;
	extern int _sitcm;
	extern int _eitcm;
#elif defined(__ICCARM__)
	#pragma section = "RO_ITCM_CODE_INIT"
	#pragma section = "RO_ITCM_CODE"
#else
	#error "ITCM not initialized for this toolchain !"
#endif

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);

#ifndef COREMARK
static void MPU_Config (void);
static void itcm_region_initialize(void);
#ifndef IPERF
static void java_heap_initialize(void);
#endif
#endif

static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
void init()
{
	/* Enable fault handlers */
	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk;

	/* Enable the CPU Cache */
	CPU_CACHE_Enable();

	/* STM32F7xx HAL library initialization:
		- Configure the Flash prefetch, instruction and Data caches
		- Configure the Systick to generate an interrupt each 1 msec
		- Set NVIC Group Priority to 4
		- Global MSP (MCU Support Package) initialization
	*/
	HAL_Init();

	/* Configure the system clock to 200 Mhz */
	SystemClock_Config();
}

#if !defined(COREMARK) && !defined(IPERF)
__weak void LLCOMM_stack_initialize(void)
{
	// does nothing by default, overrided when ECOM-COMM is used
}

void xJavaTaskFunction(void * pvParameters)
{
	/* Start the CPU Load task */
	cpuload_init();

	/* Initialize the ECOM-COMM stack */
	LLCOMM_stack_initialize();

	/* Zero initialize for Java memory */
	java_heap_initialize();

	/* Start the VM */
	microej_main();

	/* Should reach this point only if the VM exits */
	vTaskDelete( xTaskGetCurrentTaskHandle() );
}
#endif

#ifdef IPERF
void xIperfTaskFunction(void * pvParameters)
{
	/* Start the iperf */
	iperf_main();

	/* Should reach this point only if the iperf exits */
	vTaskDelete( xTaskGetCurrentTaskHandle() );
}
#endif

/**
  * @brief  Bootloader program
  * @param  None
  * @retval None
  */
int main(void)
{
	/* Init the core */
	init();

#if (CODE_AREA == USE_QSPI) || (BINARY_AREA == USE_SPI_NOR)
	/* Configure Required Non-Volatile Memory */
	if (QSPI_Startup() != MEMORY_OK)
	{
		Error_Handler();
	}
#endif

#ifdef COREMARK
	/* Launch coremark application */
	core_main();
#else
	/* Launch user application in external QSPI */
	app_main();
#endif

	/* We should never get here as execution is now on user application */
	while(1)
	{
	}
}

#ifndef COREMARK
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int app_main(void)
{
	/* Initialize the ITCM region, mandatory to be the first thing done in app_main() */
	itcm_region_initialize();

	printf("Start\n");

	/* Print MCU identifiers */
	printf("MCU revision identifier: 0x%lX\n", LL_DBGMCU_GetRevisionID());
	printf("MCU device identifier: 0x%lX\n", LL_DBGMCU_GetDeviceID());

	/* Configure MPU for all memory regions */
	MPU_Config();

#if (DATA_AREA == USE_EXTERNAL_SDRAM) || (CODE_AREA == USE_EXTERNAL_SDRAM)
	/* Configure Required Volatile Memory */
	if (SDRAM_Startup() != MEMORY_OK)
	{
		Error_Handler();
	}
#endif

#ifndef IPERF
	/* Check if the system has resumed from IWDG reset */
	if (is_watchdog_reset()) {
		printf("reset performed by the watchdog\n");
		clear_watchdog_flags();
	}

	/* Start the watchdog, but note that if the watchdog period is customized by the application,
	 * the watchdog timeout value will be compute using the real LSI frequency
	 * and the watchdog will start using this computed value.
	 */
	start_watchdog();

#ifdef ENABLE_SYSTEM_VIEW
	printf("Enable Segger system view\n");
	SEGGER_SYSVIEW_Conf();
	printf("SEGGER_RTT block address: %p\n", &(_SEGGER_RTT));
#endif
#endif

#ifdef IPERF
	/* Start iperf task */
	xTaskCreate( xIperfTaskFunction, "Iperf", IPERF_TASK_STACK_SIZE, NULL, IPERF_TASK_PRIORITY, NULL );
#else
	/* Start MicroJvm task */
	TaskHandle_t pvCreatedTask;
	xTaskCreate( xJavaTaskFunction, "MicroJvm", JAVA_TASK_STACK_SIZE, NULL, JAVA_TASK_PRIORITY, &pvCreatedTask );
#ifdef ENABLE_SYSTEM_VIEW
	SEGGER_SYSVIEW_setMicroJVMTask((U32)pvCreatedTask);
#endif
#endif

	vTaskStartScheduler();

	/* We should never get here unless the MicroJvm exits */
	while (1)
	{
	}
}
#endif

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 200000000
  *            HCLK(Hz)                       = 200000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 400
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
	HAL_StatusTypeDef ret = HAL_OK;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* The voltage scaling allows optimizing the power consumption when the device is
		clocked below the maximum system frequency, to update the voltage scaling value 
		regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 400;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 8;
	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	ASSERT(ret != HAL_OK);

	/* activate the OverDrive */
	ret = HAL_PWREx_EnableOverDrive();
	ASSERT(ret != HAL_OK);

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
		clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
	ASSERT(ret != HAL_OK);
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
	/* Enable I-Cache */
	SCB_EnableICache();

	/* Enable D-Cache */
	SCB_EnableDCache();
}

#ifndef COREMARK
/**
  * @brief  Configure MPU for all memory regions.
  * @param  None
  * @retval None
  */
static void MPU_Config (void)
{
	MPU_Region_InitTypeDef MPU_InitStruct;

	/* Disable the MPU */
	HAL_MPU_Disable();

	/* Set common MPU attributes for all regions */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;

	/* Configure the MPU region for FLASH (write-through, no write allocate) */
	MPU_InitStruct.BaseAddress = 0x08000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU region for QSPI complete address space (strongly-ordered) */
	/* See AN4861 LCD-TFT display controller (LTDC) on STM32 MCUs, Chapter 4.6.2 */
	MPU_InitStruct.BaseAddress = 0x90000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_256MB;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER1;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU region for QSPI used address space (write-through, no write allocate) */
	MPU_InitStruct.BaseAddress = 0x90000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER2;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU region for SRAM (write-through, no write allocate) */
	MPU_InitStruct.BaseAddress = 0x20010000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER3;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU region for SDRAM used address space (write-through, no write allocate) */
	MPU_InitStruct.BaseAddress = 0xC0000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER4;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU region for SDRAM display buffers (strongly-ordered) */
	/* See AN4861 LCD-TFT display controller (LTDC) on STM32 MCUs, Chapter 4.6.2 */
	MPU_InitStruct.BaseAddress = FRAME_BUFFER;
	MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER5;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	MPU_InitStruct.BaseAddress = BACK_BUFFER;
	MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER6;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

#ifdef DUAL_BACK_BUFFER_MODE_ENABLED
	MPU_InitStruct.BaseAddress = BACK_BUFFER_B;
	MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER7;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
#endif

	/* TCM does not need MPU */

	/* Enable the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
#endif

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
		ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif /* USE_FULL_ASSERT */

#ifndef COREMARK
/**
  * @brief  Initialize the ITCM region by loading the code from QSPI.
  * @param  None
  * @retval None
  */
static void itcm_region_initialize(void)
{
#if defined( __GNUC__ )
	uint32_t itcm_load_region = (uint32_t)(&_siitcm);
	uint32_t itcm_execution_region_start = (uint32_t)(&_sitcm);
	uint32_t itcm_execution_region_end = (uint32_t)(&_eitcm);
#elif defined(__ICCARM__)
	uint32_t itcm_load_region = (uint32_t)__section_begin("RO_ITCM_CODE_INIT");
	uint32_t itcm_execution_region_start = (uint32_t)__section_begin("RO_ITCM_CODE");
	uint32_t itcm_execution_region_end = itcm_execution_region_start + (uint32_t)__section_size("RO_ITCM_CODE_INIT");
#else
	"ITCM not initialized for this toolchain !"
#endif

	memcpy((void *)itcm_execution_region_start, (void *)itcm_load_region, itcm_execution_region_end - itcm_execution_region_start);
}

#ifndef IPERF
/**
  * @brief  Zero initialize for Java heap and Java immortals.
  * @param  None
  * @retval None
  */
static void java_heap_initialize(void)
{
#if defined( __GNUC__ )
	uint32_t java_heap_start = (uint32_t)(&_java_heap_start);
	uint32_t java_heap_end = (uint32_t)(&_java_heap_end);
	uint32_t java_immortals_start = (uint32_t)(&_java_immortals_start);
	uint32_t java_immortals_end = (uint32_t)(&_java_immortals_end);
	uint32_t microui_heap_start = (uint32_t)(&_microui_heap_start);
	uint32_t microui_heap_end = (uint32_t)(&_microui_heap_end);
#elif defined(__ICCARM__)
	uint32_t java_heap_start = (uint32_t)(&RW_MICROEJ_JAVA_HEAP$$Base);
	uint32_t java_heap_end = (uint32_t)(&RW_MICROEJ_JAVA_HEAP$$Limit);
	uint32_t java_immortals_start = (uint32_t)(&RW_MICROEJ_IMMORTALS$$Base);
	uint32_t java_immortals_end = (uint32_t)(&RW_MICROEJ_IMMORTALS$$Limit);
	uint32_t microui_heap_start = (uint32_t)(&RW_MICROUI_HEAP$$Base);
	uint32_t microui_heap_end = (uint32_t)(&RW_MICROUI_HEAP$$Limit);
#else
	#error "Java heap not initialized for this toolchain !"
#endif

	memset((void*)java_heap_start, 0, java_heap_end - java_heap_start);
	memset((void*)java_immortals_start, 0, java_immortals_end - java_immortals_start);
	memset((void*)microui_heap_start, 0, microui_heap_end - microui_heap_start);
}
#endif
#endif

static void Error_Handler(void)
{
}

#if !defined(COREMARK) && !defined(IPERF)
#include "sni.h"
jfloat Java_com_is2t_microjvm_test_MJVMPortValidation_testFPU__FF (jfloat a, jfloat b) {return a * b;}
jdouble Java_com_is2t_microjvm_test_MJVMPortValidation_testFPU__DD (jdouble a, jdouble b) {return a * b;}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
