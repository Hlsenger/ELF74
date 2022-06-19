/*
  Autores: Felipe Priotto e Henrique Lopes
  Laboratório 4
  Professor: Paulo D. G. da Luz
  Turma S12, 2022/1
*/

/*
Programar um protocolo serial simples, sem paridade, checksum, C.R.C. e
criptografia. Este protocolo foi desenvolvido para comandar 04 x Relés na
placa do Kit TIVA EK-TM4C1294XL. Simular os Relés nos 4 leds da placa:
LED1 (PN1), LED2 (PN0), LED3 (PF4) e LED4 (PF0). O LED ligado significa
Relé ligado e LED desligado significa Relé desligado. O programa deve ser no
estilo “Super Loop”. Deve ficar em loop infinito sempre que receber um
pacote válido na serial, deve atuar no hardware.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/uart.h"

// defines GPIOS
#define GPIO_PA0_U0RX 0x00000001
#define GPIO_PA1_U0TX 0x00000401

//leds
bool led1 = false;
bool led2 = false;
bool led3 = false;
bool led4 = false;
//message buffer
unsigned char msg[4] = {0};

//functions
void configLEDs();
void configUART();
void UARTHandler();
void UARTStringSend();

int main()
{
  //configures clock
  uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);
  //configures leds
  configLEDs();
  //configures UART
  configUART(ui32SysClock);
  //serial comms
  while(1){
    //turns on
    if(msg[0] == '#' && msg[1] == 'R' && (msg[2] == '1' || msg[2] == '2' || msg[2] == '3' || msg[2] == '4') && msg[3] == '1'){
      switch(msg[2]){
      case '1':
        led1 = true;
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, led1<<1);
        UARTStringSend("@R11", 4);
        break;
      case '2':
        led2 = true;
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, led2);
        UARTStringSend("@R21", 4);
        break;
      case '3':
        led3 = true;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, led3<<4);
        UARTStringSend("@R31", 4);
        break;
      case '4':
        led4 = true;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, led4);
        UARTStringSend("@R41", 4);
        break;
      }
      //clears msg
      for(int i=0; i<4; i++){
        msg[i] = 0;
      }
    }
    //turns off
    if(msg[0] == '#' && msg[1] == 'R' && (msg[2] == '1' || msg[2] == '2' || msg[2] == '3' || msg[2] == '4') && msg[3] == '0'){
      switch(msg[2]){
      case '1':
        led1 = false;
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, led1<<1);
        UARTStringSend("@R10", 4);
        break;
      case '2':
        led2 = false;
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, led2);
        UARTStringSend("@R20", 4);
        break;
      case '3':
        led3 = false;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, led3<<4);
        UARTStringSend("@R30", 4);
        break;
      case '4':
        led4 = false;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, led4);
        UARTStringSend("@R40", 4);
        break;
      }
      //clears msg
      for(int i=0; i<4; i++){
        msg[i] = 0;
      }
    }
    //turns all LEDs on
    if(msg[0] == '#' && msg[1] == 'T' && msg[2] == 'X' && msg[3] == '1'){
      led1 = true;
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, led1<<1);
      led2 = true;
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, led2);
      led3 = true;
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, led3<<4);
      led4 = true;
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, led4);
      UARTStringSend("@TX1", 4);
      //clears msg
      for(int i=0; i<4; i++){
        msg[i] = 0;
      }
    }
    //turns all LEDs off
    if(msg[0] == '#' && msg[1] == 'T' && msg[2] == 'X' && msg[3] == '0'){
      led1 = false;
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, led1<<1);
      led2 = false;
      GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, led2);
      led3 = false;
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, led3<<4);
      led4 = false;
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, led4);
      UARTStringSend("@TX0", 4);
      //clears msg
      for(int i=0; i<4; i++){
        msg[i] = 0;
      }
    }
  }
}

void UARTStringSend(const uint8_t *String, uint32_t tamanho){
  while(tamanho--){
    UARTCharPut(UART0_BASE, *String++);
  }
}

void configUART(uint32_t ui32SysClock){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
  {
  }
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
  {
  }
  
  UARTConfigSetExpClk(UART0_BASE, ui32SysClock, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
  UARTFIFODisable(UART0_BASE);
  UARTIntEnable(UART0_BASE, UART_INT_RX);
  UARTIntRegister(UART0_BASE, UARTHandler);
  GPIOPinTypeUART(GPIO_PORTA_BASE,(GPIO_PIN_0|GPIO_PIN_1));
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
}

void UARTHandler(){
  unsigned char var;
  UARTIntClear(UART0_BASE, UART_INT_RX);
  if(UARTCharsAvail(UART0_BASE)){
    var = (unsigned char)UARTCharGetNonBlocking(UART0_BASE);
  }
  msg[0]=msg[1];
  msg[1]=msg[2];
  msg[2]=msg[3];
  msg[3]=var;
}

void configLEDs(){
  //Configure all board LEDs (LED1, LED2, LED3, LED4)
  //Enables GPIO PORT N (LEDs)
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
  
  // Wait for the GPIO module to be ready.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
  {
  }
  
  //Enables GPIO PORT F (LEDs)
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  
  // Wait for the GPIO module to be ready.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
  {
  }
  
  //Sets the direction and mode of the specified pins
  GPIODirModeSet(GPIO_PORTN_BASE, (GPIO_PIN_1 | GPIO_PIN_0), GPIO_DIR_MODE_OUT);
  GPIODirModeSet(GPIO_PORTF_BASE, (GPIO_PIN_4 | GPIO_PIN_0), GPIO_DIR_MODE_OUT);
  
  //Sets the pad configuration for the specified pins
  GPIOPadConfigSet(GPIO_PORTN_BASE, (GPIO_PIN_1 | GPIO_PIN_0), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  GPIOPadConfigSet(GPIO_PORTF_BASE, (GPIO_PIN_4 | GPIO_PIN_0), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}