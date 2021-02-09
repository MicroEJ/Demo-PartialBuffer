..
    Copyright 2020-2021 MicroEJ Corp. All rights reserved.
	This library is provided in source code for use, modification and test, subject to license terms.
	Any modification of the source code will break MicroEJ Corp. warranties on the whole library.

=============================================================================================
Running the partial buffer demo and integrating partial buffer support to an existing project
=============================================================================================

Overview
========

This example shows the minimum steps required to implement a UI partial buffer in a MicroEJ application and in a MicroEJ platform.
To learn more about the partial buffer, please visit the `MicroEJ Developer Documentation <https://docs.microej.com/en/latest/PlatformDeveloperGuide/uiDisplay.html#section-display-partial-buffer>`__.

The following steps will be described:

- Running the demo application on STM32F7508-DK
- Changing your MicroEJ application to use partial buffer
- Changing your platform to support partial buffer

Requirements
------------

This example has been tested on:

- STM32F7508-DK board
- MicroEJ SDK 5.3.0
- STM32CubeIDE 1.5.0

Repository content
------------------

This repository provides:

- a fork of the STM32F7508-DK demo platform 1.2.0, which supports the partial buffer mode
- a support library, which allows to use the partial buffer support of the platform in the application
- a demo application, which uses partial buffer and shows animations
- this documentation

Running the demo application on STM32F7508-DK
=============================================

Setting up the workspace
------------------------

Importing the projects
~~~~~~~~~~~~~~~~~~~~~~

1. Open MicroEJ SDK in an empty workspace
2. Select ``File > Import...``
3. Select ``General > Existing Projects into Workspace``
4. Press ``Next >``
5. Next to the ``Select root directory`` field, press ``Browse...``
6. Navigate to the root folder of this Git repository
7. Press ``OK``
8. Press ``Finish``

Downloading the architecture and packs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Download the following architecture and packs (``.xpf`` and ``.xpfp``):

- Architecture: https://repository.microej.com/architectures/com/microej/architecture/CM7/CM7hardfp_GCC48/flopi7G26/7.14.0/
- UI pack: https://repository.microej.com/architectures/com/microej/architecture/CM7/CM7hardfp_GCC48/flopi7G26-ui-pack/13.0.3/
- FS pack: https://repository.microej.com/modules/com/microej/pack/fs/5.1.2/
- HAL pack: https://repository.microej.com/modules/com/microej/pack/hal/2.0.1/
- NET pack: https://repository.microej.com/modules/com/microej/pack/net/9.2.2/

Importing the architecture and packs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. In MicroEJ SDK, select ``File > Import...``
2. Select ``MicroEJ > Architectures``
3. Press ``Next >``
4. Next to the ``Select directory`` field, press ``Browse...``
5. Navigate to the folder containing the downloaded architecture and packs
6. Press ``OK``
7. Press ``Deselect All``
8. Check ``ARM Cortex-M7 GCC EVAL``
9. Click on ``ARM Cortex-M7 GCC EVAL``
10. Check ``I agree and accept the above terms and conditions and I want to install the copyrighted Software``
11. Click on ``HAL``
12. Check ``I agree and accept the above terms and conditions and I want to install the copyrighted Software``
13. Press ``Finish``

Activate your architecture license
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Follow the steps in `this documentation <https://docs.microej.com/en/latest/overview/licenses.html#evaluation-license>`__ in order to activate your license of the evaluation architecture.

Building the platform
~~~~~~~~~~~~~~~~~~~~~

1. In MicroEJ SDK, open the ``stm32f7508_freertos-configuration/STM32F7508.platform`` file
4. Click on ``Build Platform``

Running the demo application on simulator
-----------------------------------------

1. In MicroEJ SDK, select ``Run > Run Configurations...``
2. Under ``MicroEJ Application``, select the ``Partial Buffer Demo (SIM)`` run configuration
3. Press ``Run``

Running the demo application on device
--------------------------------------

Building the application on device
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. In MicroEJ SDK, select ``Run > Run Configurations...``
2. Under ``MicroEJ Application``, select the ``Partial Buffer Demo (EMB)`` run configuration
3. Press ``Run``

Importing the BSP project
~~~~~~~~~~~~~~~~~~~~~~~~~

1. Open STM32CubeIDE in an empty workspace
2. Select ``File > Import...``
3. Select ``General > Existing Projects into Workspace``
4. Press ``Next >``
5. Next to the ``Select root directory`` field, press ``Browse...``
6. Navigate to the ``stm32f7508_freertos-bsp/projects/microej/SW4STM32`` folder of this Git repository
7. Press ``OK``
8. Press ``Finish``

Building the BSP project
~~~~~~~~~~~~~~~~~~~~~~~~

1. In STM32CubeIDE, right-click on the ``application`` project
2. Press ``Build Project``
3. Wait for the end of the build

Flashing the application on the board
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Plug-in your STM32F7508-DK board
2. In STM32CubeIDE, select ``Run > Run Configurations...``
3. Under ``STM32 Cortex-M C/C++ Application``, select the ``application_debug`` run configuration
4. Press ``Run``

Changing your MicroEJ application to use partial buffer
=======================================================

Setting up the render policy
----------------------------

First, add a dependency to the support library in the ``module.ivy`` file of your application:

.. code-block:: xml

  <dependency org="com.microej.example.partial" name="support" rev="1.0.0"/>

In the code of your application, use the partial render policy instead of the default policy when the desktop is created:

.. code-block:: java

  Desktop desktop = new Desktop() {
	@Override
	protected RenderPolicy createRenderPolicy() {
		return new PartialRenderPolicy(this, false);
	}
  };

Solving rendering issues
------------------------

Using a partial buffer may lead to rendering issues.
Refer to the ``Application Limitations`` section of `this documentation <https://docs.microej.com/en/latest/PlatformDeveloperGuide/uiDisplay.html#section-display-partial-buffer>`__ in order to understand why these issues occur and how to fix them.

Updating the run configuration
------------------------------

To be able to run your application, the platform selected in the run configuration has to support partial buffer.

If you want to check that your application doesn't have rendering issues in partial buffer mode, you may run your application on the STM32F7508-DK platform provided in this demo.

If you want to run your application on your own platform, follow the next section in order to integrate the partial buffer support in your platform.

Changing your platform to support partial buffer
================================================

Running your application on simulator
-------------------------------------

Run your application launcher on your platform.
When the application will try to use the partial render policy, the execution will fail because your platform does not support partial buffer yet.

Here is the expected error message:

::

  Exception in thread "UIPump" @T:java.lang.UnsatisfiedLinkError@: No HIL client implementor found (timeout)
    at java.lang.Throwable.fillInStackTrace(Throwable.java:82)
    at java.lang.Throwable.<init>(Throwable.java:37)
    at java.lang.Error.<init>(Error.java:18)
    at java.lang.LinkageError.<init>(LinkageError.java:18)
    at java.lang.UnsatisfiedLinkError.<init>(UnsatisfiedLinkError.java:10)
    at com.microej.example.partial.support.PartialRenderPolicy.<init>(PartialRenderPolicy.java:42)
    at com.microej.demo.widget.common.PageHelper$2.createRenderPolicy(PageHelper.java:216)
    at ej.mwt.Desktop.onShown(Desktop.java:217)
    at ej.microui.display.Display.replaceDisplayable(Display.java:322)
    at ej.microui.display.Display.executeEventOnShow(Display.java:271)
    at ej.microui.display.DisplayPump.executeEvent(DisplayPump.java:81)
    at ej.microui.MicroUIPump.execute(MicroUIPump.java:236)
    at ej.microui.MicroUIPump.run(MicroUIPump.java:177)
    at java.lang.Thread.run(Thread.java:325)
    at java.lang.Thread.runWrapper(Thread.java:387

This error is due to the missing implementation of the partial buffer natives. 
Indeed, the simulator engine expects to find a Java implementation emulating the behavior of the partial buffer natives.

The next section describes how to implement a mock of the partial buffer for the simulator.

Implementing the partial buffer mock for the simulator
------------------------------------------------------

The simulator is using a mock called Front Panel (``{YOUR_PLATFORM_NAME}-fp`` project) to generate a graphical representation of the target device. 
The partial buffer mock needs to be integrated to the Front Panel to simulate the use of a partial buffer.

To integrate the partial buffer mock to a Front Panel project, perform the following steps:

1. Copy the ``com.microej.example.partial.support`` package from the `stm32f7508_freertos-fp <stm32f7508_freertos-fp/src/main/java/>`__ project to your Front Panel project
2. Open the ``PartialBufferNatives`` class of your Front Panel project and change the value of the ``BUFFER_HEIGHT`` constant to the desired value
3. Open the description file of your Front Panel project (``.fp`` file) and replace ``ej.fp.widget.Display`` by ``com.microej.example.partial.support.PartialDisplay``
4. Open the configuration file of your platform (``.platform`` file) and build the platform

Once done, you should be able to run your application on simulator.

Running your application on device
----------------------------------

Building the application
~~~~~~~~~~~~~~~~~~~~~~~~

Run your application launcher on your platform, and link the application with your BSP.
Since your application uses the partial render policy, the link will fail because your platform does not support partial buffer yet.

Here is the expected error message:

  ::

    undefined reference to Java_com_microej_example_partial_support_PartialBufferNatives_setFlushLimits
    undefined reference to Java_com_microej_example_partial_support_PartialBufferNatives_getBufferHeight

This error is due to the missing implementation of the partial buffer natives. 
Indeed, the linker expects to find the C implementation of the partial buffer natives.

The next section describes how to implement the partial buffer in the BSP code.

Implementing the partial buffer in the BSP
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This section shows the process for a STM32F7508-DK target using the GCC toolchain. 
Please refer to the relevant ``Build a Java Platform`` guide in the ``Getting started`` section of MicroEJ Resource Center for indications on how to adapt this process to your specific target.

The partial buffer is designed to work with the ``copy`` mode of the MicroUI Display engine. 
See the `Buffer Modes <https://docs.microej.com/en/latest/PlatformDeveloperGuide/uiDisplay.html#principle>`__ section of the MicroEJ GUI documentation for more information.

This ``copy`` mode is a prerequisite to complete the following steps. 

The implementation of the partial buffer is done in the ``LLUI_DISPLAY.c`` file located in the ``stm32f7508_freertos-bsp/projects/microej/ui/src/`` folder.

Step 1: Allocate memory for the back buffer and the frame buffer
################################################################

In the ``LLUI_DISPLAY.c`` file, remove the following lines:

.. code-block:: C

  #define DISPLAY_MEM_SIZE 0x80000 
  uint8_t display_mem[DISPLAY_MEM_SIZE] __ALIGNED(DISPLAY_MEM_SIZE) __attribute__((section(".DisplayMem")));
  #define BUFFER_SIZE (RK043FN48H_WIDTH * RK043FN48H_HEIGHT * (DRAWING_DMA2D_BPP / 8))
  #define BACK_BUFFER ((int32_t)&display_mem[0])
  #define FRAME_BUFFER (BACK_BUFFER + BUFFER_SIZE)

Add the following lines to allocate a dedicated array for the frame buffer:

.. code-block:: C

  #define FRAME_BUFFER_MEM_SIZE RK043FN48H_WIDTH * RK043FN48H_HEIGHT * DRAWING_DMA2D_BPP / 8
  uint8_t frame_buffer_mem[FRAME_BUFFER_MEM_SIZE] __attribute__((section(".DisplayMem"))); 
  #define FRAME_BUFFER ((int32_t)&frame_buffer_mem[0]) 

Add the following lines to allocate a dedicated array for the back buffer:

.. code-block:: C

  #define BACK_BUFFER_HEIGHT 136
  #define BACK_BUFFER_MEM_SIZE RK043FN48H_WIDTH * BACK_BUFFER_HEIGHT * DRAWING_DMA2D_BPP / 8
  uint8_t back_buffer_mem[BACK_BUFFER_MEM_SIZE] __attribute__((section(".DisplayMem")));
  #define BACK_BUFFER ((int32_t)&back_buffer_mem[0])

In the ``main.c`` file, update the MPU (Memory Protection Unit) configuration: 

- Add the following lines to make a reference to the back buffer:

  .. code-block:: C

    extern uint8_t back_buffer_mem[];
    #define BACK_BUFFER ((int32_t)&back_buffer_mem[0])

- In the ``MPU_Config`` function: 

  - Update the size of the protected region for the back buffer:

    - Change the ``MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;`` line to ``MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;``

  - Add the following lines after the last ``HAL_MPU_ConfigRegion(&MPU_InitStruct)`` line, 
    to protect the memory space of the frame buffer:

    .. code-block:: C

      MPU_InitStruct.BaseAddress = FRAME_BUFFER;
      MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
      MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
      MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
      MPU_InitStruct.Number = MPU_REGION_NUMBER6;
      HAL_MPU_ConfigRegion(&MPU_InitStruct);

Note that the ``MPU_InitStruct.Number`` must be unique for each protected memory space.

Step 2: Setup variables for the partial buffer
##############################################

In the ``LLUI_DISPLAY.c`` file:

Declare a structure that will be used to store the parameters of the flushed region:

.. code-block:: C

  typedef struct { 
    int32_t x_offset;
    int32_t y_offset;
    uint32_t width;
    uint32_t height;
  } flush_region_t;

  static flush_region_t flush_region;

Step 3: Implement the partial buffer natives
############################################

Add the partial Buffer native functions, following the strict SNI naming convention mentioned earlier.

For the ``..._setFlushLimits`` native, use the ``flush_region`` structure to save the flush parameters:

.. code-block:: C
  
  void Java_com_microej_example_partial_support_PartialBufferNatives_setFlushLimits(jint xOffset, jint yOffset, jint width, jint height)
  {
      flush_region.x_offset = xOffset;
      flush_region.y_offset = yOffset;
      flush_region.width = width;
      flush_region.height = height;
  }

For the ``..._getBufferHeight`` native, return the back buffer height:

.. code-block:: C

  int32_t Java_com_microej_example_partial_support_PartialBufferNatives_getBufferHeight()
  {
    return BACK_BUFFER_HEIGHT;
  }

Step 4: Adapt the LLUI_DISPLAY_IMPL_flush function to use the partial buffer
############################################################################

In the partial buffer mode, the role of the ``LLUI_DISPLAY_IMPL_flush`` function is to copy the content of the back buffer to a specific area of the frame buffer. 

First, the back buffer content needs to be fetched without exceeding its boundaries. 
Thus, ``Xmax`` and ``Ymax`` will respectively be the width and the height of the area to update:

.. code-block:: C

	xmax = flush_region.width - 1;
	ymax = flush_region.height - 1;

Secondly, that content needs to be copied to a specific area of the frame buffer.
This area is determined thanks to the X and Y flush offsets.
Slide the frame buffer address to copy the content to the right area:

.. code-block:: C

  int y_flush_offset = flush_region.y_offset;
  int x_flush_offset = flush_region.x_offset;

  uint8_t* dest_addr = FRAME_BUFFER_ADDRESS + ((x_flush_offset + y_flush_offset * SCREEN_WIDTH) * DISPLAY_BPP / 8);

The ``SCREEN_WIDTH`` and ``DISPLAY_BPP`` constants are depending on the configuration of the target device.

Finally, copy the back buffer to the frame buffer, it can be done with a ``memcpy`` or thanks to DMA.
In this example, the DMA2D accelerator of the STM32F7508-DK board is used: 

.. code-block:: C

  DRAWING_DMA2D_configure_memcpy(srcAddr, dest_addr, xmin, ymin, xmax, ymax, SCREEN_WIDTH, &dma2d_memcpy);
  DRAWING_DMA2D_start_memcpy(&dma2d_memcpy);

Extra: Switching to dual back buffer mode
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A dual back buffer can be implemented for performances enhancement.
It is interesting to implement it when the copy from the back buffer to the frame buffer takes a long time compared the time taken by the MicroEJ application to draw in the back buffer.

The dual back buffer mode is using 2 back buffers of the same size and the frame buffer:

- Back Buffer A (BB_A)
- Back Buffer B (BB_B)
- Frame Buffer (FB)

This switch mode will run the following way:

1. App is drawing in BB_A
2. BB_A is copied to FB
3. App is drawing in BB_B
4. BB_B is copied to FB
5. App is drawing in BB_A
6. ...

This process can be parallelized, doing the back buffer to frame buffer copy in a dedicated task (DMA flush task):

+------+----------------+------------------+
| Time | Flush function | DMA flush task   |
+======+================+==================+
| T1   | App -> BB_A    | WAITING          |
+------+----------------+------------------+
| T2   | App -> BB_B    | BB_A -> FB       |
+------+----------------+------------------+
| T3   | App -> BB_A    | BB_B -> FB       |
+------+----------------+------------------+
| T4   | App -> BB_B    | BB_A -> FB       |
+------+----------------+------------------+
| T5   | ...            | ...              |
+------+----------------+------------------+

In the following example, the dual back buffer has already been implemented 
in the `dual_back_buffer.c <stm32f7508_freertos-bsp/projects/microej/ui/src/dual_back_buffer.c>`__ file.

It can be enabled by adding the ``#define DUAL_BACK_BUFFER_MODE_ENABLED`` directive in the
in the `display_configuration.h <stm32f7508_freertos-bsp/projects/microej/ui/inc/display_configuration.h>`__ file.

The following steps explain how to implement this dual back buffer.

Step 1: Allocate the memory for a second back buffer
#####################################################

In the ``LLUI_DISPLAY.c`` file:

- Allocate the memory for the second back buffer:

  .. code-block:: C

    uint8_t back_buffer_B_mem[BACK_BUFFER_MEM_SIZE] __attribute__((section(".DisplayMem")));
    #define BACK_BUFFER_B ((int32_t)&back_buffer_B_mem[0])

In the ``main.c`` file, update the MPU (Memory Protection Unit) configuration: 

- Add the following lines to make a reference to the second back buffer:

.. code-block:: C

  extern uint8_t back_buffer_B_mem[];
  #define BACK_BUFFER_B ((int32_t)&back_buffer_B_mem[0])

- In the ``MPU_Config`` function, update the MPU configuration,
  add the following lines after the last ``HAL_MPU_ConfigRegion(&MPU_InitStruct)`` line:

  .. code-block:: C

    MPU_InitStruct.BaseAddress = BACK_BUFFER_B;
    MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER7;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

Note that the ``MPU_InitStruct.Number`` must be unique for each protected memory space.

Step 2: Declare binary semaphores to synchronize the DMA flush task with the flush function
#############################################################################################

In the ``LLUI_DISPLAY.c`` file:

- Declare binary semaphores:
  
  .. code-block:: C

    static xSemaphoreHandle task_start_copy_sem;
    static xSemaphoreHandle dma2d_end_of_copy_sem;

- Initialize the semaphores in the ``LLUI_DISPLAY_IMPL_initialize`` function:
  
  .. code-block:: C
  
    task_start_copy_sem = xSemaphoreCreateBinary();
    dma2d_end_of_copy_sem = xSemaphoreCreateBinary();

Step 3: Implement a custom callback function for the DMA2D
###########################################################

By default, when the DMA copy ends, the callback function ``LLUI_DISPLAY_flushDone`` 
is called to notify the MicroEJ application that the flush is done.
Once received, the application will start drawing a new frame in the back buffer.

In the dual back buffer case, it is not necessary to wait for the end of the DMA copy anymore. 
Indeed, one back buffer will be copied to the frame buffer while the application will draw in the other one.

A custom callback will be used to replace the call to the ``LLUI_DISPLAY_flushDone`` callback.
This callback will be used to notify the DMA flush task that the DMA have finished its job.

In the ``LLUI_DISPLAY.c`` file:

- Add a custom callback function that unlocks the ``dma2d_end_of_copy_sem`` semaphore:
  
  .. code-block:: C

    void DMA2D_EndOfCopy(bool under_isr)
    {
      LLUI_DISPLAY_IMPL_binarySemaphoreGive((void*)dma2d_end_of_copy_sem, under_isr);
    }

In the ``drawing_dma2d.c`` file:

- Declare the ``DMA2D_EndOfCopy`` function:
  
  .. code-block:: C

    extern void DMA2D_EndOfCopy(bool under_isr);

- In the ``DRAWING_DMA2D_configure_memcpy`` function, configure the DMA2D to call the
  ``DMA2D_EndOfCopy`` callback :

  .. code-block:: C
    
    g_callback_notification = &DMA2D_EndOfCopy;

Step 4: Implement the DMA flush task
####################################

In the ``LLUI_DISPLAY.c`` file:

- Declare a structure that will be used to pass the flush parameters of the flush function to the DMA flush task:

  .. code-block:: C

    typedef struct { 
      uint8_t* srcAddr;
      uint8_t* dest_addr;
      uint32_t xmin;
      uint32_t ymin;
      uint32_t xmax;
      uint32_t ymax;
    } flush_params_t;

    static flush_params_t flush_params;

- Create a new task in the ``LLUI_DISPLAY_IMPL_initialize`` function:

  .. code-block:: C

    xTaskCreate(&DMA_flush_task, "DMA_flush_task", 
                1024, &flush_params, DMA_TASK_PRIO, NULL);  

  The ``DMA_TASK_PRIO`` should have less priority than the MicroJVM task (``JAVA_TASK_PRIORITY``) 
  defined in the `main.c <stm32f7508_freertos-bsp/projects/microej/main/src/main.c>`__ file.

- Implement the ``DMA_flush_task`` function:

  .. code-block:: C

    void DMA_flush_task(void * params)
    {
      while(1){
        // wait for flush asked by the Java application
        LLUI_DISPLAY_IMPL_binarySemaphoreTake((void*)task_start_copy_sem);
        
        flush_params_t* flush_p = (flush_params_t*) params; /* get the parameters */
        
        // The DMA2D configure waits for the end of a previous DMA2D copy before starting
        DRAWING_DMA2D_configure_memcpy(flush_p->srcAddr, flush_p->dest_addr, flush_p->xmin, flush_p->ymin, flush_p->xmax, flush_p->ymax, RK043FN48H_WIDTH, &dma2d_memcpy, false);
        
        LLUI_DISPLAY_flushDone(false); /* send the flush done event to the Java world */
        
        DRAWING_DMA2D_start_memcpy(&dma2d_memcpy);

        // wait for the end of the DMA copy
        LLUI_DISPLAY_IMPL_binarySemaphoreTake((void*)dma2d_end_of_copy_sem);
      }
    }

  Note that the ``DRAWING_DMA2D_configure_memcpy`` and ``DRAWING_DMA2D_start_memcpy`` functions have been moved from the ``LLUI_DISPLAY_IMPL_flush`` to the DMA flush task.

Step 5: Update the LLUI_DISPLAY_IMPL_flush function
###################################################

In the ``LLUI_DISPLAY.c`` file, in the ``LLUI_DISPLAY_IMPL_flush`` function:

- Delete the call to the ``DRAWING_DMA2D_configure_memcpy`` and ``DRAWING_DMA2D_start_memcpy`` functions
- Store the flush parameters in the ``flush_params`` structure:
  
  .. code-block:: C

    flush_params.srcAddr=srcAddr;
    flush_params.dest_addr=dest_addr;
    flush_params.xmin=xmin;
    flush_params.xmax=flush_region.width -1;
    flush_params.ymin=ymin;
    flush_params.ymax=flush_region.height-1;

- Wake up the DMA flush task:
  
  .. code-block:: C
  
  	xSemaphoreGive(task_start_copy_sem);

- Change the returned address to switch between the back buffer A and B:

  .. code-block:: C

 	  return srcAddr == (uint8_t*)BACK_BUFFER_A ?  (uint8_t*)BACK_BUFFER_B :  (uint8_t*)BACK_BUFFER_A; 

Tweaking the size of the partial buffer
---------------------------------------

The goal of using a partial buffer is to reduce the memory footprint used by the display.
The following section explains how to adapt the size of the partial buffer.

In the Front Panel
~~~~~~~~~~~~~~~~~~

1. Open the ``com.microej.example.partial.support.PartialBufferNatives`` class of your Front Panel project
2. Change the value of the ``BUFFER_HEIGHT`` constant to the desired value

In the BSP
~~~~~~~~~~

The generic formula to compute the size of a pixel buffer is the following one: ``SCREEN_WIDTH * BUFFER_HEIGHT * DISPLAY_BPP / 8``

The value of ``DISPLAY_BPP`` depends on the MicroUI configuration on the target board.
Refer to the `MicroEJ documentation <https://docs.microej.com/en/latest/PlatformDeveloperGuide/uiDisplay.html#section-display-installation>`__ for more information.

For the STM32F7508-DK board, the parameters are the following ones:

- ``SCREEN_WIDTH``: 480
- ``SCREEN_HEIGHT``: 272
- ``DISPLAY_BPP``: 16

Frame buffer size computation
#############################

Since the display uses a buffer in RAM to read the pixel data to display, this buffer has to be a complete buffer.

For example, on the STM32F7508-DK board, the size of the frame buffer would be ``262120`` bytes.

Back buffer size computation
############################

A partial buffer always has the same width as the screen but a smaller height.

For example, on the STM32F7508-DK board, the size of a back buffer presenting half the screen would be ``130560`` bytes.
