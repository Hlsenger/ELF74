/*
  Autores: Felipe Priotto e Henrique Lopes
  Laboratório 2 modificado
  Professor: Paulo D. G. da Luz
  Turma S12, 2022/1
*/

// Este laboratório consiste na implementação de um projeto para teste de reflexo.
/*
1) Deve-se ler uma tecla: Tecla_1 (PJ0/USR_SW1) para iniciar a contagem do tempo e
enquanto a Tecla_1 não for pressionada nada acontece;
2) O disparo do “tempo de jogo” deve ser feito pela Tecla_2 (PJ1/USR_SW2) somente
após ter sido dado início do jogo na Tecla_1;
3) Caso o tempo exceda de 3s, travar o uso da segunda tecla: Tecla_2 e indicar que o
jogo acabou acendendo o LED:D4 (PF0);
4) O programa pode deve reiniciar todo o processo caso a Tecla_1 (PJ0/USR_SW1)
seja pressionada novamente, mesmo estando no estado do “tempo de jogo”;
5) A reposta do “tempo de jogo” deve ser fornecida em [ms];
*/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "inc/tm4c1294ncpdt.h"

//button status and timers for software deboucing
bool bt1flag = false, bt2flag = false;
unsigned int bt1time = 0, bt2time = 0;
//systick timer
unsigned int SysTicks1ms = 0;
//led status
bool led1 = false;
bool led3 = false;
//game variables
unsigned int StartTimer;
unsigned int StopTimer;
bool pressed = false;
bool gameStatus = false;

void systickhandler();
void portjHandler();
void configLEDs();
void configButtons();
void configsystick();
void systickhandler();

int main(void){
  // Sets system clock to 5MHz with PLL
  uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);
  //configures all LEDs as outputs
  configLEDs();
  //configures all buttons as inputs
  configButtons();
  //configures systick
  configsystick();
  
  while(1){
    if(bt1flag){
       //Botão1 liberado !!!
      if (GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0) && (SysTicks1ms>=bt1time)){
        //botão liberado
        bt1flag=false;
        //55ms para liberar estado do botão ... tempo anti-debouncing
        bt1time=SysTicks1ms+55;
        //enables interrupts to happen to that specific port again
        GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_INT_PIN_0);
      }
    }
    if(bt2flag){
       //Botão2 liberado !!!
      if (GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_1) && (SysTicks1ms>=bt2time)){
        //botão liberado
        bt2flag=false;
        //55ms para liberar estado do botão ... tempo anti-debouncing
        bt2time=SysTicks1ms+55;
      }
    }
    
    if(pressed){
      printf("Tempo de reacao: %dms\n", (StopTimer - StartTimer));
      pressed = false;
    }
    
    
  }
  
}

void portjHandler(){
  // the masked interrupt status is returned
  int status = GPIOIntStatus(GPIO_PORTJ_BASE, true);
  GPIOIntClear(GPIO_PORTJ_BASE, (GPIO_INT_PIN_0 | GPIO_INT_PIN_1));
  
  //button 1
  if((!bt1flag) && (status == 1) && (SysTicks1ms > bt1time)){
    //disables interrupt on button 1 until it is released on main loop to avoid locking
    GPIOIntDisable(GPIO_PORTJ_BASE, GPIO_INT_PIN_0);
    
    
    led1 = true;
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, led1<<1);
    led3 = false;
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, led3<<4);
    //Starts the timer
    StartTimer = SysTicks1ms;
    //begins the game
    gameStatus = true;
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_INT_PIN_1);

    
    //botão pressionado
    bt1flag=true;
    //55ms para liberar estado do botão ... tempo anti-debouncing
    bt1time=SysTicks1ms+55;
  }
  //button 2
  else if((!bt2flag) && (status == 2) && (SysTicks1ms > bt2time)){
    //disables interrupt on button 2 until it is released on main loop to avoid locking
    GPIOIntDisable(GPIO_PORTJ_BASE, GPIO_INT_PIN_1);
    
    
    StopTimer = SysTicks1ms;
    pressed = true;
    led1 = false;
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, led1<<1);
    gameStatus = false;
    
    //botão pressionado
    bt2flag=true;
    //55ms para liberar estado do botão ... tempo anti-debouncing
    bt2time=SysTicks1ms+55;
  }
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

void configButtons(){
  //Enables GPIO PORT J (LEDs)
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
  
  // Wait for the GPIO module to be ready.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
  {
  }
  
  //Sets the direction and mode of the specified pins
  GPIODirModeSet(GPIO_PORTJ_BASE, (GPIO_PIN_0 | GPIO_PIN_1), GPIO_DIR_MODE_IN);
  
  //Sets the pad configuration for the specified pins
  GPIOPadConfigSet(GPIO_PORTJ_BASE, (GPIO_PIN_0 | GPIO_PIN_1), GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  
  //Sets the interrupt type for the specified pins
  GPIOIntTypeSet(GPIO_PORTJ_BASE, (GPIO_PIN_0 | GPIO_PIN_1), GPIO_LOW_LEVEL);
  //Enables only the button 1, which will control button 2
  GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_INT_PIN_0);
  
  GPIOIntRegister(GPIO_PORTJ_BASE, portjHandler);
  
  //Enables an interrupt.
  IntEnable(INT_GPIOJ);
  //Enables the processor interrupt
  IntMasterEnable();
}

void configsystick(){
  //desliga o SysTick para poder configurar
  SysTickDisable();
  //clock 5MHz
  SysTickPeriodSet(120000);
  //handler function
  SysTickIntRegister(systickhandler);
  //enables systick interrupt
  SysTickIntEnable();
  //enables systick
  SysTickEnable();
}

void systickhandler(){
  SysTicks1ms++;
  
  if(((SysTicks1ms - StartTimer) > 3000) && gameStatus){
    gameStatus = false;
    GPIOIntDisable(GPIO_PORTJ_BASE, GPIO_INT_PIN_1);
    led3 = true;
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, led3<<4);
    led1 = false;
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, led1<<1);
  }
}