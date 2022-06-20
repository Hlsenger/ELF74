/* This is a small demo of the high-performance ThreadX kernel.  It includes examples of eight
threads of different priorities, using a message queue, semaphore, mutex, event flags group, 
byte pool, and block pool. Please refer to Chapter 6 of the ThreadX User Guide for a complete
description of this demonstration.  */



#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "tx_api.h"
#include "tx_port.h"

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

#include "defines.h"
#include "charbuffer.h"
#include "elevador.h"


//Estruturas do Threadx
TX_THREAD               threads[4];
TX_BYTE_POOL            byte_pool_0;

TX_QUEUE                queue_e;
TX_QUEUE                queue_c;
TX_QUEUE                queue_d;
TX_QUEUE                uart0_queue_out;

TX_EVENT_FLAGS_GROUP    uart0_flags;
UCHAR                   byte_pool_memory[BYTE_POOL_SIZE];




uint32_t sysClock;







CharBuffer uart0Buffer;
Elevador elevador_e;
Elevador elevador_c;
Elevador elevador_d;



void thread_serial_service(ULONG thread_input);
void thread_elevador(ULONG thread_input);

void UART0IntHandler(void)
{
  _tx_thread_context_save();
  
  uint32_t ui32Status = UARTIntStatus(UART0_BASE, true);
  UARTIntClear(UART0_BASE, ui32Status);
  
  
  
  
  while(UARTCharsAvail(UART0_BASE)) {
    char a = UARTCharGetNonBlocking(UART0_BASE);
    charBufferAdd(&uart0Buffer,a);
  }
  
  tx_event_flags_set(&uart0_flags, RECIEVE_FLAG, TX_OR);
  
  _tx_thread_context_restore();
}



void tiva_setup();
void uart0_setup();


void uart0_setup(){
  //Enable UART and GPIOA clocks
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}
  
  IntMasterEnable();
  IntEnable(INT_UART0);
  

  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  
  //configure UART to 15200 baudrate and 8-N-1
  UARTConfigSetExpClk(UART0_BASE, sysClock, 115200,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
  
  
  
  
  
    UARTEnable(UART0_BASE);
    //Flush UART0
    while(UARTCharsAvail(UART0_BASE)){
      UARTCharGetNonBlocking(UART0_BASE);
    }
  
  
  

  
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


void uart0_string_put(CHAR *str, UINT size){
  for(UINT i=0;i<size;i++){
    UARTCharPutNonBlocking(UART0_BASE,str[i]);
  }
}


/* Define main entry point.  */

int main()
{
  
  //Inicializa a estrutura dos 3 elevadores
  elevador_inicializa(&elevador_e,'e',&queue_e,&uart0_queue_out,&uart0_flags);
  elevador_inicializa(&elevador_c,'c',&queue_c,&uart0_queue_out,&uart0_flags);
  elevador_inicializa(&elevador_d,'d',&queue_d,&uart0_queue_out,&uart0_flags);
  
  tiva_setup();
  
  
  
  tx_kernel_enter();
}



void    tx_application_define(void *first_unused_memory)
{
  
  CHAR    *pointer = TX_NULL;
  
  tx_byte_pool_create(&byte_pool_0, "byte pool", byte_pool_memory, BYTE_POOL_SIZE);
  
  
  
  
  //==== Threads ===
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
  tx_thread_create(&threads[0], "Serial Service", thread_serial_service, 0,  
                   pointer, THREAD_STACK_SIZE, 
                   1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
  
  
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
  tx_thread_create(&threads[1], "Elevador esquerdo", thread_elevador, (ULONG)&elevador_e,  
                   pointer, THREAD_STACK_SIZE, 
                   2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
  
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
  tx_thread_create(&threads[2], "Elevador central", thread_elevador, (ULONG)&elevador_c,  
                   pointer, THREAD_STACK_SIZE, 
                   2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
  
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
  tx_thread_create(&threads[3], "Elevador direito", thread_elevador, (ULONG)&elevador_d,  
                   pointer, THREAD_STACK_SIZE, 
                   2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
  
  
  
  
  //==== Estruras de dados ====
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, QUEUE_SIZE, TX_NO_WAIT);
  tx_queue_create(&queue_e, "queue elevador esquerdo", TX_1_ULONG, pointer, QUEUE_SIZE);
  
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, QUEUE_SIZE, TX_NO_WAIT);
  tx_queue_create(&queue_c, "queue elevador central", TX_1_ULONG, pointer, QUEUE_SIZE);
  
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, QUEUE_SIZE, TX_NO_WAIT);
  tx_queue_create(&queue_d, "queue elevador direito", TX_1_ULONG, pointer, QUEUE_SIZE);
  
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, QUEUE_SIZE, TX_NO_WAIT);
  tx_queue_create(&uart0_queue_out, "queue saida serial", TX_1_ULONG, pointer, QUEUE_SIZE);
  
  tx_event_flags_create(&uart0_flags, "uart0 flags");
  
  
}


void parse_cmd(CHAR *cmd){
  CHAR elevador = cmd[0];
  
  //Pula o /n gerado pelo 'initialized'
   // charBufferSkip(&uart0Buffer);
  
  switch(elevador){
  case 'e':
    tx_queue_send(&queue_e, &(cmd[1]), TX_NO_WAIT);
    break;
    
  case 'c':
    tx_queue_send(&queue_c, &(cmd[1]), TX_NO_WAIT);
    break;
    
  case 'd':
    tx_queue_send(&queue_d, &(cmd[1]), TX_NO_WAIT);
    break;
    
  case 'i':
    
    
    //Reseta os 3 elevadores
    elevador_reset(&elevador_e);
    elevador_reset(&elevador_c);
    elevador_reset(&elevador_d);
    
    //Inicializa os 3 elevadores
    uart0_string_put("er\x0D",3);
    uart0_string_put("cr\x0D",3);
    uart0_string_put("dr\x0D",3);
    
    break;
  } 
  
  
}



void  thread_serial_service(ULONG thread_input)
{
  UINT status;
  ULONG flags;
  CHAR a;
  
  static CHAR serial_in_data[64];
  static uint32_t i = 0;
  
  while(1)
  {
    
    status =  tx_event_flags_get(&uart0_flags, RECIEVE_FLAG | SEND_FLAG, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER);
    if(status == TX_SUCCESS){
      if(flags & RECIEVE_FLAG){
        
        a = 0;
        while(a != '\n' && !charBufferIsEmpty(&uart0Buffer))
        {
          charBufferGet(&uart0Buffer,&a);
          serial_in_data[i++] = a;
        }
        
        
        if(a == '\n'){
          parse_cmd(serial_in_data);
          memset(serial_in_data,0,64);
          i=0;
        }
      } 
    }
    
    if(flags & SEND_FLAG){
      
      CHAR msg[4];
      status = tx_queue_receive(&uart0_queue_out, msg, TX_NO_WAIT);
      
      while(status != TX_QUEUE_EMPTY){
        if(status == TX_SUCCESS){
          for(UINT i=0;i<3;i++){
            UARTCharPut(UART0_BASE,msg[i]);
          }
          
          status = tx_queue_receive(&uart0_queue_out, msg, TX_NO_WAIT);
        }
      }
    } 
  }
}


void  thread_elevador (ULONG input)
{
  Elevador *elevador = (Elevador *)input;
  
  //Inicialzia todos os elevadores
  elevador->direcao = parado;
  elevador->destinoAndar = 0;
  elevador->ultimoAndar = 0;
  
  
  CHAR msg[4];
  
  
  
  while(1)
  {
    
    //Thread aguarda até receber uma mensagem
    tx_queue_receive(elevador->queue_in, msg, TX_WAIT_FOREVER);
    
    
    //Processa a mensagem
    elevador_processa_msg(elevador,msg);
    
    
    //Atua na mensagem
    elevador_update(elevador);
  }
  
  
}
