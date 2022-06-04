
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

uint32_t g_ui32SysClock;

void UARTIntHandler(void)
{
    uint32_t ui32Status = UARTIntStatus(UART0_BASE, true);
    UARTIntClear(UART0_BASE, ui32Status);


    while(UARTCharsAvail(UART0_BASE)) {
      unsigned char chr = UARTCharGetNonBlocking(UART0_BASE);
      if(chr >= 'A' && chr <= 'Z'){
        chr +=32;
      }
      
      
      UARTCharPutNonBlocking(UART0_BASE, chr);
    }
}


void setup(){
  //Set clock to 120 Mhz
   g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);
   
   //Enable UART and GPIOA clocks
   SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
   SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
   
     //Enable UART interrupts
    IntMasterEnable();
    
    
    //Configure UART0 pins
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

   
   
   
   //configure UART to 115200 baudrate and 8-N-1
   UARTConfigSetExpClk(UART0_BASE, g_ui32SysClock, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
   
  
    IntEnable(INT_UART0);
    
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);


}

int main()
{
  setup();
  
  
  while(true){
  }
  
  
  

}
