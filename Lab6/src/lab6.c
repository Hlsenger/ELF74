
// ==== LAB 6 ======

//Executando a 'rotina_led' por 10 milhoes de ciclos a 25Mhz de clock foi
//foi encontrado o numero de ciclos de cada thread como sendo: t1=172500; t2= 287500; t3=460000;



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
#define BYTE_POOL_SIZE     4096*2



typedef struct ThreadInput{
  UINT num_led;
  UINT num_ciclos;
  UINT tempo_sleep;
} ThreadInput;




TX_THREAD               threads[3];
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
void thread_led(ULONG input);






void tiva_setup();


ThreadInput t1 = {.num_led = 0, .num_ciclos = 172500, .tempo_sleep = 70};
ThreadInput t2 = {.num_led = 1, .num_ciclos = 287500, .tempo_sleep = 100};
ThreadInput t3 = {.num_led = 2, .num_ciclos = 460000, .tempo_sleep = 320};


void tiva_setup(){
  
  
  //SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_OSC, 25000000);
  
  sysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                         SYSCTL_OSC_MAIN |
                         SYSCTL_USE_PLL |
                         SYSCTL_CFG_VCO_480), 25000000);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
  
  
  
  //Led 1(N0) e Led 0 (N1)
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  
  //Led 2(F4) e Led 3(F0)
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
  
  
}






int main()
{
  tiva_setup();
  
  
 
  tx_kernel_enter();
}




#define MODO_1_TIMESLICE
//#define MODO_2_PRIORIDADES
//#define MODO_3_PREEMPTIVO
//#define MODO_4_MUTEX

void    tx_application_define(void *first_unused_memory)
{
  
  
  
  CHAR *sp[3]; //Ponteiros para os stacks dos threads
  
  
  tx_byte_pool_create(&byte_pool_0, "byte pool 0", byte_pool_memory, BYTE_POOL_SIZE);

  
  for(UINT i=0;i<3;i++){
    tx_byte_allocate(&byte_pool_0, (VOID **) &sp[i], THREAD_STACK_SIZE, TX_NO_WAIT);
  }
  
  
  
#ifdef MODO_1_TIMESLICE
  tx_thread_create(&threads[0], "T1", thread_led, (ULONG) &t1,  sp[0], THREAD_STACK_SIZE, 1, 1, 5, TX_AUTO_START);
  
  tx_thread_create(&threads[1], "T2", thread_led, (ULONG) &t2,  sp[1], THREAD_STACK_SIZE, 1, 1, 5, TX_AUTO_START);
  
  tx_thread_create(&threads[2], "T3", thread_led, (ULONG) &t3,  sp[2], THREAD_STACK_SIZE, 1, 1, 5, TX_AUTO_START);
#endif
  
#ifdef MODO_2_PRIORIDADES
  tx_thread_create(&threads[0], "T1", thread_led, (ULONG) &t1,  sp[0], THREAD_STACK_SIZE, 1, 1, 5, TX_AUTO_START);
  
  tx_thread_create(&threads[1], "T2", thread_led, (ULONG) &t2,  sp[1], THREAD_STACK_SIZE, 2, 1, 5, TX_AUTO_START);
  
  tx_thread_create(&threads[2], "T3", thread_led, (ULONG) &t3,  sp[2], THREAD_STACK_SIZE, 3, 1, 5, TX_AUTO_START);
#endif
  
  
#ifdef  MODO_3_PREEMPTIVO
  tx_thread_create(&threads[0], "T1", thread_led, (ULONG) &t1,  sp[0], THREAD_STACK_SIZE, 1, 1, 5, TX_AUTO_START);
  
  tx_thread_create(&threads[1], "T2", thread_led, (ULONG) &t2,  sp[1], THREAD_STACK_SIZE, 2, 2, 5, TX_AUTO_START);
  
  tx_thread_create(&threads[2], "T3", thread_led, (ULONG) &t3,  sp[2], THREAD_STACK_SIZE, 3, 3, 5, TX_AUTO_START);
#endif
  

  

  
  
}

void rotina_led(UINT num_ciclos, UINT num_led){
  UINT i=0;
  UINT led_port;
  UINT led_pin;
  
  switch(num_led){
  case 0:
    led_port = GPIO_PORTN_BASE;
    led_pin = GPIO_PIN_1;
    break;
  case 1:
    led_port = GPIO_PORTN_BASE;
    led_pin = GPIO_PIN_0;
    break;
  case 2:
    led_port = GPIO_PORTF_BASE;
    led_pin = GPIO_PIN_4;
    break;
  case 3:
    led_port = GPIO_PORTF_BASE;
    led_pin = GPIO_PIN_0;
    break;
  }
  
  
  for(i=0;i<num_ciclos;i++){
    
    //Acende
     GPIOPinWrite(led_port, led_pin, led_pin);
     
     //Apaga todos os leds
     GPIOPinWrite(GPIO_PORTN_BASE, 0xFF, 0x0);
     GPIOPinWrite(GPIO_PORTF_BASE, 0xFF, 0x0);
    
  }
  
}

void thread_led(ULONG input)
{
  ThreadInput *in = (ThreadInput *) input;
  
  while(1)
  {
    rotina_led(in->num_ciclos,in->num_led);
    
    tx_thread_sleep(in->tempo_sleep);
  }
  
  
}

