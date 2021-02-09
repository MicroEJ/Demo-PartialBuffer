/*
 * C
 *
 * Copyright 2014-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#ifndef _LLDISPLAY_DISCO_CONFIGURATION
#define _LLDISPLAY_DISCO_CONFIGURATION

#include <stdio.h>
#include "stm32f7508_discovery_lcd.h"

/* Defines -------------------------------------------------------------------*/
#define LLDISPLAY_BPP DRAWING_DMA2D_BPP

// Define size to allocate for the Frame Buffer
#define FRAME_BUFFER_MEM_SIZE RK043FN48H_WIDTH * RK043FN48H_HEIGHT * DRAWING_DMA2D_BPP / 8 /* Frame buffer: (480 * 272 * (16 / 8)). Rounded up to fill completely an MPU region */

// Declare the Back buffer height
#define BACK_BUFFER_HEIGHT 136 /* Use a partial back buffer of 136 lines (half of the screen height) */

// Define size to allocate for the Back Buffer
#define BACK_BUFFER_MEM_SIZE RK043FN48H_WIDTH * BACK_BUFFER_HEIGHT * DRAWING_DMA2D_BPP / 8 /* Back buffer: (480 * 136 * (16 / 8))). Rounded up to fill completely an MPU region */

/**
 * DUAL_BACK_BUFFER_MODE_ENABLED
 * Use this define to enable the dual back buffer mode
 * If DUAL_BACK_BUFFER_MODE_ENABLED is not set, the single back buffer mode will be used
 */
#define DUAL_BACK_BUFFER_MODE_ENABLED

/* Structures -------------------------------------------------------------------*/

/* structure that contains the region to update, unit: px */
typedef struct {
	int32_t x_offset;
	int32_t y_offset;
	uint32_t width;
	uint32_t height;
} DISPLAY_CONFIGURATION_flush_region;

/* Functions -------------------------------------------------------------------*/

void DISPLAY_CONFIGURATION_display_init(void);

#endif
