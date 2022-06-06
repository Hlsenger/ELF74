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

#define THREAD_STACK_SIZE         512
#define BYTE_POOL_SIZE     4096
#define QUEUE_SIZE      128




/* Define the ThreadX object control blocks...  */

TX_THREAD               thread_0;
TX_THREAD               thread_1;
TX_BYTE_POOL            byte_pool_0;

TX_QUEUE                queue_e;
TX_QUEUE                queue_c;
TX_QUEUE                queue_d;
TX_QUEUE                queue_out;


/* Define byte pool memory.  */

UCHAR                   byte_pool_memory[BYTE_POOL_SIZE];








uint32_t sysClock;


#define MAX_QUEUE_SIZE 64
#define CHAR_BUFFER_SIZE 128

//Char buffer definitiosn
typedef struct CharBuffer{
  uint32_t head;
  uint32_t tail;
  uint8_t data[CHAR_BUFFER_SIZE];
} CharBuffer; 


void charBufferAdd(CharBuffer *buffer,uint8_t data){
  buffer->data[buffer->head] = data;
  buffer->head = (buffer->head + 1) % CHAR_BUFFER_SIZE;
}


bool charBufferGet(CharBuffer *buffer, uint8_t *data){
  if(buffer->tail == buffer->head){
    return false;
  }
  *data = buffer->data[buffer->tail];
  buffer->tail = (buffer->tail + 1) % CHAR_BUFFER_SIZE;
  return true;
}

bool charBufferIsEmpty(CharBuffer *buffer){
  if(buffer->tail == buffer->head){
    return true;
  }
  return false;
}








enum Direcao{parado, subindo, descendo};

typedef struct Elevador{
  enum Direcao direcao;
  UINT ultimoAndar;
  UINT destinoAndar;
  
  bool andaresPressionados[16];
  
} Elevador;

CharBuffer uart0Buffer;


bool serial_flag;
CHAR serial_in_data[32];


Elevador elevador_e;


/* Define thread prototypes.  */
void thread_serial_service(ULONG thread_input);
void thread_elevador_esquerdo(ULONG thread_input);

void UART0IntHandler(void)
{
  _tx_thread_context_save();
  
  uint32_t ui32Status = UARTIntStatus(UART0_BASE, true);
  UARTIntClear(UART0_BASE, ui32Status);
  
  
  
  
  uint32_t i =0;
  while(UARTCharsAvail(UART0_BASE)) {
    char a = UARTCharGetNonBlocking(UART0_BASE);
    charBufferAdd(&uart0Buffer,a);
    serial_flag = true;
  }
  
  _tx_thread_context_restore();
}



void tiva_setup();
void uart0_setup();


void uart0_setup(){
  
  serial_flag = false;
  
  
  //Enable UART and GPIOA clocks
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}
  
  //UARTFIFODisable(UART0_BASE);
  
  //Enable UART interrupts
  IntMasterEnable();
  
  IntEnable(INT_UART0);
  
  UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
  
  //Configure UART0 pins
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  
  //configure UART to 15200 baudrate and 8-N-1
  UARTConfigSetExpClk(UART0_BASE, sysClock, 115200,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
  
  
  UARTEnable(UART0_BASE);
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
  tiva_setup();
  
  
  
  
  
  
  
  
  
  
  tx_kernel_enter();
}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{
  
  CHAR    *pointer = TX_NULL;
  
  tx_byte_pool_create(&byte_pool_0, "byte pool 0", byte_pool_memory, BYTE_POOL_SIZE);
  
  
  
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
  tx_thread_create(&thread_0, "Serial Service", thread_serial_service, 0,  
                   pointer, THREAD_STACK_SIZE, 
                   1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
  
  
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
  tx_thread_create(&thread_1, "Elevador esquerdo", thread_elevador_esquerdo, 1,  
                   pointer, THREAD_STACK_SIZE, 
                   1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
  
  
  
  
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, QUEUE_SIZE, TX_NO_WAIT);
  tx_queue_create(&queue_e, "queue elevador esquerdo", TX_1_ULONG, pointer, QUEUE_SIZE);
  
  tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, QUEUE_SIZE, TX_NO_WAIT);
  tx_queue_create(&queue_out, "queue saida serial", TX_1_ULONG, pointer, QUEUE_SIZE);
  
  
}


void parse_cmd(CHAR *cmd){
  
  
  
  CHAR elevador = serial_in_data[0];
  
  
  switch(elevador){
  case 'e':
    
    tx_queue_send(&queue_e, &(serial_in_data[1]), TX_WAIT_FOREVER);
    break;
    
  case 'c':
    
    //!!TEMPORARIO
    uart0_string_put("er\x0D",3);
    uart0_string_put("cr\x0D",3);
    uart0_string_put("dr\x0D",3);
    break;
    
  case 'd':
    break;
    
  case 'i':
    uart0_string_put("er\x0D",3);
    uart0_string_put("cr\x0D",3);
    uart0_string_put("dr\x0D",3);
    break;
  } 
}



void  thread_serial_service(ULONG thread_input)
{
  UINT status;
  CHAR a;
  
  
  while(1)
  {
    if(serial_flag){
      
       a = 0x00;
      uint32_t i = 0;
      
      while(!charBufferIsEmpty(&uart0Buffer) && a != '\n')
      {
        charBufferGet(&uart0Buffer,&a);
        serial_in_data[i++] = a;
      }
      
      
      parse_cmd(serial_in_data);
      
      serial_flag = false;
    }
    
    
    CHAR msg[4];
    status = tx_queue_receive(&queue_out, msg, TX_NO_WAIT);
    
    if(status == TX_SUCCESS){
      for(UINT i=0;i<3;i++){
        UARTCharPut(UART0_BASE,msg[i]);
      }
    }
    
    
    
    
    tx_thread_sleep(1);
  }
}


void  thread_elevador_esquerdo (ULONG thread_input)
{
  
  //Inicialzia todos os elevadores
  elevador_e.direcao = parado;
  elevador_e.destinoAndar = 0;
  elevador_e.ultimoAndar = 0;
  
  
  CHAR msg[4];
  while(1)
  {
    
    //Elevador so atua ao receber uma nova atualização
    tx_queue_receive(&queue_e, msg, TX_WAIT_FOREVER);
    
    
    
    //Processa mensagem
    
    //Botao externo
    if(msg[0] == 'E'){
      // tx_queue_send(&queue_out, "er\x0D", TX_WAIT_FOREVER);
      tx_queue_send(&queue_out, "ef\x0D", TX_WAIT_FOREVER);
      tx_thread_sleep(100);
      tx_queue_send(&queue_out, "ea\x0D", TX_WAIT_FOREVER);
    }
    
    
    //Botao interno
    else if(msg[0] == 'I'){
      
      elevador_e.andaresPressionados[msg[1]-97] =  true;
      elevador_e.destinoAndar = msg[1]-97;
      
      /* tx_queue_send(&queue_out, "ef\x0D", TX_WAIT_FOREVER);
      tx_thread_sleep(100);
      tx_queue_send(&queue_out, "es\x0D", TX_WAIT_FOREVER);*/
    }
    
    //Status do elevador
    else{
      
      
      //Porta aberta ou fechada
      if(msg[0] == 'A' || msg[0] == 'F'){
        
      }
      else{
        elevador_e.ultimoAndar= atoi(msg);
        //tx_queue_send(&queue_out, "ep\x0D", TX_WAIT_FOREVER);
      }
    }
    
    
    //Atua na mensagem
    
    if(elevador_e.direcao == parado){
      if(elevador_e.destinoAndar > elevador_e.ultimoAndar){
        elevador_e.direcao = subindo;
        tx_queue_send(&queue_out, "ef\x0D", TX_WAIT_FOREVER);
        tx_thread_sleep(100);
        tx_queue_send(&queue_out, "es\x0D", TX_WAIT_FOREVER);
      }else if(elevador_e.destinoAndar < elevador_e.ultimoAndar){
        elevador_e.direcao = subindo;
        tx_queue_send(&queue_out, "ef\x0D", TX_WAIT_FOREVER);
        tx_thread_sleep(100);
        tx_queue_send(&queue_out, "ed\x0D", TX_WAIT_FOREVER);
      }
    }
    else if(elevador_e.direcao == subindo){
      if(elevador_e.destinoAndar == elevador_e.ultimoAndar){
        elevador_e.direcao = parado;
        tx_queue_send(&queue_out, "ep\x0D", TX_WAIT_FOREVER);
      }
    }
    else if(elevador_e.direcao == descendo){
      if(elevador_e.destinoAndar == elevador_e.ultimoAndar){
        elevador_e.direcao = parado;
        tx_queue_send(&queue_out, "ep\x0D", TX_WAIT_FOREVER);
      }
    }
    
    
    tx_thread_sleep(1);
    
  }
}
