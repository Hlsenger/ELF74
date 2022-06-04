/* This is a small demo of the high-performance ThreadX kernel.  It includes examples of eight
threads of different priorities, using a message queue, semaphore, mutex, event flags group, 
byte pool, and block pool. Please refer to Chapter 6 of the ThreadX User Guide for a complete
description of this demonstration.  */

#include "tx_api.h"
#include "tx_port.h"

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include "driverlib/gpio.h"
#include "driverlib/uart.h"

#define THREAD_STACK_SIZE         1024
#define BYTE_POOL_SIZE     4096




/* Define the ThreadX object control blocks...  */

TX_THREAD               thread_0;
TX_THREAD               thread_1;
TX_BYTE_POOL            byte_pool_0;


/* Define byte pool memory.  */

UCHAR                   byte_pool_memory[BYTE_POOL_SIZE];


/* Define the counters used in the demo application...  */
ULONG                   thread_0_counter;
ULONG                   thread_1_counter;


//Satus dos leds
bool led1 = false;
bool led2 = false;

uint32_t sysClock;



/* Define thread prototypes.  */
void    thread_tled1(ULONG thread_input);
void    thread_tled2(ULONG thread_input);

void UART0IntHandler(void)
{
    uint32_t ui32Status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, ui32Status);
    
    while(UARTCharsAvail(UART0_BASE)) {
      unsigned char chr = UARTCharGetNonBlocking(UART0_BASE);
      //UARTCharPutNonBlocking(UART0_BASE, chr);
    }
}



void tiva_setup();
void uart0_setup();


void uart0_setup(){
  //Enable UART and GPIOA clocks
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}
  
  UARTFIFODisable(UART0_BASE);
  
  
  
  //Enable UART interrupts
  IntMasterEnable();
  UARTIntRegister(UART0_BASE,UART0IntHandler);
  
  //Configure UART0 pins
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  
  //configure UART to 15200 baudrate and 8-N-1
  UARTConfigSetExpClk(UART0_BASE, sysClock, 115200,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
  
  
  
  
  
  
  
  
  
  IntEnable(INT_UART0);
  
  UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}



void tiva_setup(){
  
  sysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                 SYSCTL_OSC_MAIN |
                                   SYSCTL_USE_PLL |
                                     SYSCTL_CFG_VCO_480), 120000000);
  
  
  uart0_setup();
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  
  
  
  
}


/* Define main entry point.  */

int main()
{
  tiva_setup();
  
  /* Please refer to Chapter 6 of the ThreadX User Guide for a complete
  description of this demonstration.  */
  


          
  

  /* Enter the ThreadX kernel.  */
  tx_kernel_enter();
}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{
  
  CHAR    *pointer = TX_NULL;
  
  
  /* Create a byte memory pool from which to allocate the thread stacks.  */
  tx_byte_pool_create(&byte_pool_0, "byte pool 0", byte_pool_memory, BYTE_POOL_SIZE);
  
  /* Put system definition stuff in here, e.g. thread creates and other assorted
  create information.  */
  
  /* Allocate the stack for thread 0.  */
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
  
  /* Create the main thread.  */
  
  
  tx_thread_create(&thread_0, "TLed1", thread_tled1, 0,  
                   pointer, THREAD_STACK_SIZE, 
                   1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
  
  /* Allocate the stack for thread 1.  */
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
  
  /* Create threads 1 and 2. These threads pass information through a ThreadX 
  message queue.  It is also interesting to note that these threads have a time
  slice.  */
  tx_thread_create(&thread_1, "TLed2", thread_tled2, 1,  
                   pointer, THREAD_STACK_SIZE, 
                   1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
  
  
}



void    thread_tled1(ULONG thread_input)
{
  while(1)
  {
    led1 = !led1;
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, led1<<1);
    tx_thread_sleep(50); // 50*10ms = 0,5s
  }
}


void    thread_tled2(ULONG thread_input)
{
  
  
     UARTCharPutNonBlocking(UART0_BASE, 'e');
     UARTCharPutNonBlocking(UART0_BASE, 'r');
     UARTCharPutNonBlocking(UART0_BASE, 0xD);
  while(1)
  {
    led2 = !led2;
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, led2);
    
    
    
   
     UARTCharPutNonBlocking(UART0_BASE, 'e');
     UARTCharPutNonBlocking(UART0_BASE, 'f');
     UARTCharPutNonBlocking(UART0_BASE, 0xD);
    
    tx_thread_sleep(100); // 100*10ms = 1s
    
    
     UARTCharPutNonBlocking(UART0_BASE, 'e');
     UARTCharPutNonBlocking(UART0_BASE, 'a');
     UARTCharPutNonBlocking(UART0_BASE, 0xD);
    
     tx_thread_sleep(100); // 100*10ms = 1s
    
  }
}
