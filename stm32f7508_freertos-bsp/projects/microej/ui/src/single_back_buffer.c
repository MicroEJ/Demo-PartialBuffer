/*
 * C
 *
 * Copyright 2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */
#include "display_configuration.h"
#ifndef DUAL_BACK_BUFFER_MODE_ENABLED

#include "LLUI_DISPLAY_impl.h"
#include "FreeRTOS.h"
#include "drawing_dma2d.h"

/* Defines -------------------------------------------------------------------*/

// Destination buffer
#define LTDC_Layer LTDC_Layer2

/* Global --------------------------------------------------------------------*/

extern DISPLAY_CONFIGURATION_flush_region flush_region;

static DRAWING_DMA2D_memcpy dma2d_memcpy;

/* Public functions ----------------------------------------------------------*/

void DISPLAY_CONFIGURATION_display_init(void)
{
	// No need to do a custom init in the single back buffer case
}

uint8_t* LLUI_DISPLAY_IMPL_flush(MICROUI_GraphicsContext* gc, uint8_t* srcAddr, uint32_t xmin, uint32_t ymin, uint32_t xmax, uint32_t ymax)
{
	// Slide the destination buffer
	uint8_t* dest_addr = (uint8_t*)LTDC_Layer->CFBAR + ((flush_region.x_offset + flush_region.y_offset * RK043FN48H_WIDTH) * DRAWING_DMA2D_BPP / 8);

#ifdef FRAMERATE_ENABLED
	framerate_increment();
#endif

	xmax =flush_region.width - 1;
	ymax = flush_region.height - 1;
	DRAWING_DMA2D_configure_memcpy(srcAddr, dest_addr, xmin, ymin, xmax, ymax, RK043FN48H_WIDTH, &dma2d_memcpy, &LLUI_DISPLAY_flushDone);
	DRAWING_DMA2D_start_memcpy(&dma2d_memcpy);

	// return the same back buffer
	return srcAddr;
}

#endif
