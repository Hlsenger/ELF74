//*****************************************************************************
//
// project0.c - Example to demonstrate minimal TivaWare setup
//
// Copyright (c) 2012-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.2.0.295 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"

//*****************************************************************************
//
// Define pin to LED mapping.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Project Zero (project0)</h1>
//!
//! This example demonstrates the use of TivaWare to setup the clocks and
//! toggle GPIO pins to make the LED blink. This is a good place to start
//! understanding your launchpad and the tools that can be used to program it.
//
//*****************************************************************************

#define USER_LED1  GPIO_PIN_1

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


bool button_pressed = false;
bool timerRunning = false;
uint32_t tickCount = 0;
uint32_t ticksAtPress = 0;


void IntGPIOJHandler(void){  
  uint32_t status = GPIOIntStatus(GPIO_PORTJ_BASE,false);
  GPIOIntClear(GPIO_PORTJ_BASE, GPIO_PIN_0|GPIO_PIN_1);
  
  //If button 1 is pressed
  if(status == 1 && timerRunning){
    button_pressed = true;
    ticksAtPress = SysTickValueGet();
    timerRunning = false;
    GPIOPinWrite(GPIO_PORTN_BASE,USER_LED1,0);
  }
  //If button 2 is pressed
  else if(status == 2){
    tickCount = 0;
    timerRunning = true;
    GPIOPinWrite(GPIO_PORTN_BASE,USER_LED1,USER_LED1);
  }
 
}


void SysTickIntHandler(void)
{

  tickCount += 1;
}



//*****************************************************************************
//
// Main 'C' Language entry point.  Toggle an LED using TivaWare.
//
//*****************************************************************************
int main(void)
{
    uint32_t ui32SysClock;

    //
    // Run from the PLL at 120 MHz.
    // Note: SYSCTL_CFG_VCO_240 is a new setting provided in TivaWare 2.2.x and
    // later to better reflect the actual VCO speed due to SYSCTL#22.
    //
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                       SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |
                                       SYSCTL_CFG_VCO_240), 120000000);

    //
    // Enable GPIO ports N(leds) and J(Input) and wait for the port to be ready for access
    //
   
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    {
    }
    
    
    //Enable Interrupts
    IntMasterEnable();
    
    // Configure the GPIO port for the LED operation.
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, USER_LED1);

    

    //HW Buttons setup 
    //Enable HW1 and HW2 buttons pins as input with internal pullup
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,(GPIO_PIN_0|GPIO_PIN_1));    
    GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0|GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    
    
    //Make HW1 and HW2 trigger interrupts on rising edge
    GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_FALLING_EDGE);
    
    
    //Enable interrupts for HW1 and HW2
    GPIOIntRegister(GPIO_PORTJ_BASE, IntGPIOJHandler);
    GPIOIntEnable(GPIO_PORTJ_BASE,GPIO_PIN_0|GPIO_PIN_1); 

    
    //Systick setup
    SysTickPeriodSet(ui32SysClock/1000); //systick nterrupt every ms
    SysTickIntEnable();
    SysTickEnable();


    //
    // Loop Forever
    //
    while(1)
    {
      if(button_pressed){
        uint32_t totalCycles = tickCount*120000+ticksAtPress;
        printf("Tempo: %d ms\n",tickCount);
        printf("Ciclos: %d \n",totalCycles);
        button_pressed = false;
      }
      
    }
}
