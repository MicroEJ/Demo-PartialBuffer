/*
 * C
 *
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */
#include "display_configuration.h"
#ifdef DUAL_BACK_BUFFER_MODE_ENABLED

#include "stm32f7508_discovery_lcd.h"
#include "LLUI_DISPLAY_impl.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "drawing_dma2d.h"

/* Defines -------------------------------------------------------------------*/

// Declare the Back Buffer memory
extern uint8_t back_buffer_mem[];
#define BACK_BUFFER ((int32_t)&back_buffer_mem[0])

// Second back buffer for dual back buffer purpose
uint8_t back_buffer_B_mem[BACK_BUFFER_MEM_SIZE] __attribute__((section(".lldisplay_back_buffer_B_mem")));
#define BACK_BUFFER_B ((int32_t)&back_buffer_B_mem[0])

// Define the priority of the DMA Flush task
#define DMA_TASK_PRIO 10 /** Should be < JAVA_TASK_PRIORITY (MicroJVM task) defined in main.c */

// Destination buffer
#define LTDC_Layer LTDC_Layer2

/* Structs -------------------------------------------------------------------*/

// Used to give the flush parameters to the DMA_flush_task
typedef struct {
	uint8_t* srcAddr;
	uint8_t* dest_addr;
	uint32_t xmin;
	uint32_t ymin;
	uint32_t xmax;
	uint32_t ymax;
} flush_params_t;

/* Global --------------------------------------------------------------------*/

extern DISPLAY_CONFIGURATION_flush_region flush_region;

static DRAWING_DMA2D_memcpy dma2d_memcpy;

// Used to pass parameters to the DMA task of the screen region to update
static flush_params_t flush_params;

// Semaphore used to synchronize the Flush task and the DMA copy task
static xSemaphoreHandle task_start_copy_sem;
static xSemaphoreHandle dma2d_end_of_copy_sem;

/* Interrupt functions -------------------------------------------------------*/

// Called by DRAWING_DMA2D_IRQHandler()
void DMA2D_EndOfCopy(bool under_isr)
{
	LLUI_DISPLAY_IMPL_binarySemaphoreGive((void*)dma2d_end_of_copy_sem, under_isr);
}

/* Private functions ---------------------------------------------------------*/

static void DMA_flush_task(void * params)
{
	while(1) {
		// Wait for flush asked by Java application
		LLUI_DISPLAY_IMPL_binarySemaphoreTake((void*)task_start_copy_sem);

		// Get the parameters
		flush_params_t* flush_p = (flush_params_t*) params;

		// The DMA2D configure waits for the end of a previous DMA2D copy before starting
		DRAWING_DMA2D_configure_memcpy(flush_p->srcAddr, flush_p->dest_addr, flush_p->xmin, flush_p->ymin, flush_p->xmax, flush_p->ymax, RK043FN48H_WIDTH, &dma2d_memcpy, &DMA2D_EndOfCopy);

		// Send the flush done event to the Java world
		// The current back buffer has been sent to the frame buffer
		// The old back buffer is free and becomes the new back buffer
		// The application can draw in this new back buffer while the other one is sent to the screen
		LLUI_DISPLAY_flushDone(false);

		DRAWING_DMA2D_start_memcpy(&dma2d_memcpy);

		// Wait for the end of the DMA copy
		LLUI_DISPLAY_IMPL_binarySemaphoreTake((void*)dma2d_end_of_copy_sem);
	}
}

/* Public functions ----------------------------------------------------------*/

uint8_t* LLUI_DISPLAY_IMPL_flush(MICROUI_GraphicsContext* gc, uint8_t* srcAddr, uint32_t xmin, uint32_t ymin, uint32_t xmax, uint32_t ymax)
{
	// Slide the destination buffer
	uint8_t* dest_addr = (uint8_t*)LTDC_Layer->CFBAR + ((flush_region.x_offset + flush_region.y_offset * RK043FN48H_WIDTH) * DRAWING_DMA2D_BPP / 8);

#ifdef FRAMERATE_ENABLED
	framerate_increment();
#endif

	flush_params.srcAddr = srcAddr;
	flush_params.dest_addr = dest_addr;
	flush_params.xmin = xmin;
	flush_params.xmax = flush_region.width - 1;
	flush_params.ymin = ymin;
	flush_params.ymax = flush_region.height - 1;

	// wake up the "flush" task
	xSemaphoreGive(task_start_copy_sem);

	// switch between Back buffer A and B at each call of the function
	return srcAddr == (uint8_t*)BACK_BUFFER ? (uint8_t*)BACK_BUFFER_B : (uint8_t*)BACK_BUFFER;
}

void DISPLAY_CONFIGURATION_display_init(void){

	task_start_copy_sem = xSemaphoreCreateBinary();
	dma2d_end_of_copy_sem = xSemaphoreCreateBinary();

	// Initialize the DMA flush task
	xTaskCreate(
				&DMA_flush_task,
				"DMA_flush_task",
				1024,
				&flush_params,
				DMA_TASK_PRIO,
				NULL);
}

#endif
