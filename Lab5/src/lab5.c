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
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

#define THREAD_STACK_SIZE         1024
#define BYTE_POOL_SIZE     4096




//Estruturas do Threadex utilizadas
TX_THREAD               thread_0;
TX_THREAD               thread_1;
TX_BYTE_POOL            byte_pool_0;
UCHAR                   byte_pool_memory[BYTE_POOL_SIZE];



//Status dos leds
bool led1 = false;
bool led2 = false;

uint32_t sysClock;




//Fun��es das threads
void    thread_tled1(ULONG thread_input);
void    thread_tled2(ULONG thread_input);


//Configura o clock do TIVA e inicializa o periferico dos leds
void tiva_setup();



void tiva_setup(){
  sysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                         SYSCTL_OSC_MAIN |
                         SYSCTL_USE_PLL |
                         SYSCTL_CFG_VCO_480), 120000000);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}


int main()
{
  tiva_setup();
  
 
  
  tx_kernel_enter();
}



void    tx_application_define(void *first_unused_memory)
{
  
  CHAR    *pointer = TX_NULL;
  
  tx_byte_pool_create(&byte_pool_0, "byte pool 0", byte_pool_memory, BYTE_POOL_SIZE);
  


  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
  tx_thread_create(&thread_0, "TLed1", thread_tled1, 0,  
                   pointer, THREAD_STACK_SIZE, 
                   1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
  

  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
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
  while(1)
  {
    led2 = !led2;
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, led2);
    tx_thread_sleep(100); // 100*10ms = 1s
  }
}
