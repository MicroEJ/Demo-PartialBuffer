/*
 * C
 *
 * Copyright 2014-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include "display_configuration.h"
#include "LLUI_DISPLAY_impl.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "microej.h"
#include "bsp_util.h"
#include "framerate.h"
#include "interrupts.h"
#include "drawing_dma2d.h"

/* Defines -------------------------------------------------------------------*/

// Declare the Frame Buffer memory
uint8_t frame_buffer_mem[FRAME_BUFFER_MEM_SIZE] __attribute__((section(".lldisplay_frame_buffer_mem"))); /* The memory space needs to be aligned due to DMA2D use */
#define FRAME_BUFFER ((int32_t)&frame_buffer_mem[0])

// Back buffer memory (used in both single and dual back buffer modes)
uint8_t back_buffer_mem[BACK_BUFFER_MEM_SIZE] __attribute__((section(".lldisplay_back_buffer_mem"))); /* The memory space needs to be aligned due to DMA2D use */
#define BACK_BUFFER ((int32_t)&back_buffer_mem[0])

/* Global --------------------------------------------------------------------*/

static SemaphoreHandle_t dma2d_sem;

extern LTDC_HandleTypeDef hLtdcHandler;

// Used to store the screen region to update
DISPLAY_CONFIGURATION_flush_region flush_region;

/* Interrupt functions -------------------------------------------------------*/

void DMA2D_IRQHandler(void)
{
	DRAWING_DMA2D_IRQHandler();
}

/* API -----------------------------------------------------------------------*/

void LLUI_DISPLAY_IMPL_initialize(LLUI_DISPLAY_SInitData* init_data)
{
	BSP_LCD_Init();

	BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, FRAME_BUFFER);

#if LLDISPLAY_BPP == 16
	HAL_LTDC_SetPixelFormat(&hLtdcHandler, LTDC_PIXEL_FORMAT_RGB565, LTDC_ACTIVE_LAYER);
#elif LLDISPLAY_BPP == 24
	HAL_LTDC_SetPixelFormat(&hLtdcHandler, LTDC_PIXEL_FORMAT_RGB888, LTDC_ACTIVE_LAYER);
#elif LLDISPLAY_BPP == 32
	HAL_LTDC_SetPixelFormat(&hLtdcHandler, LTDC_PIXEL_FORMAT_ARGB8888, LTDC_ACTIVE_LAYER);
#else
	#error "Define 'LLDISPLAY_BPP' is required (16, 24 or 32)"
#endif

	init_data->lcd_width = RK043FN48H_WIDTH;
	init_data->lcd_height = RK043FN48H_HEIGHT;
	init_data->back_buffer_address = (uint8_t*)BACK_BUFFER;
	init_data->binary_semaphore_0 = (LLUI_DISPLAY_binary_semaphore*)xSemaphoreCreateBinary();
	init_data->binary_semaphore_1 = (LLUI_DISPLAY_binary_semaphore*)xSemaphoreCreateBinary();
	dma2d_sem = xSemaphoreCreateBinary();

	// Custom init function defined in single_back_buffer.c and dual_back_buffer.c
	DISPLAY_CONFIGURATION_display_init();

	// interruptions
	HAL_NVIC_SetPriority(LTDC_IRQn, 5, 3);
	HAL_NVIC_EnableIRQ(LTDC_IRQn);

	DRAWING_DMA2D_initialize((void*)dma2d_sem);
}

void LLUI_DISPLAY_IMPL_binarySemaphoreTake(void* sem)
{
	xSemaphoreTake((xSemaphoreHandle)sem, portMAX_DELAY);
}

void LLUI_DISPLAY_IMPL_binarySemaphoreGive(void* sem, bool under_isr)
{
	if (under_isr)
	{
		portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR((xSemaphoreHandle)sem, &xHigherPriorityTaskWoken);
		if (xHigherPriorityTaskWoken != pdFALSE)
		{
			// Force a context switch here.
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
	else
	{
		xSemaphoreGive((xSemaphoreHandle)sem);
	}
}

/*
 * PartialRenderPolicy
 *
 * Returns the platform the rendering buffer (back buffer) maximum height (in
 * pixels).
 * <p>
 * The policy will ensure to not draw outside this limit.
 *
 * @return the platform rendering buffer height.
 */
int32_t Java_com_microej_example_partial_support_PartialBufferNatives_getBufferHeight()
{
	return BACK_BUFFER_HEIGHT;
}

/*
 * PartialRenderPolicy
 *
 * Sets the X,Y coordinates offsets and sizes the platform has to use when will perform the next display flush.
 * 
 * This offsets indicates where flushing the rendering buffer (back buffer) in the frame buffer.
 *
 * @param xOffset
 *            the x offset the platform has to apply at next flush.
 * @param yOffset
 *            the y offset the platform has to apply at next flush.
 * @param width
 *            the width the platform has to apply at next flush.
 * @param xOffset
 *            the height the platform has to apply at next flush.
 */
void Java_com_microej_example_partial_support_PartialBufferNatives_setFlushLimits(jint xOffset, jint yOffset, jint width, jint height)
{
	flush_region.x_offset = xOffset;
	flush_region.y_offset = yOffset;
	flush_region.width = width;
	flush_region.height = height;
}
