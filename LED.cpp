/*
 * LED.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table

// initialize your LEDs
void LED_Init(void){
    // write this
    // PINCM
    //   bit 25 is HiZ
    //   bit 20 is drive strength
    //   bit 18 is input enable control
    //   bit 17 is pull up control
    //   bit 16 is pull down control
    //   bit 7 is PC peripheral connected, enable transparent data flow
    //   bit 0 selects GPIO function
      IOMUX->SECCFG.PINCM[PA26INDEX] = (uint32_t) 0x00000081;
      IOMUX->SECCFG.PINCM[PA25INDEX] = (uint32_t) 0x00000081;
      IOMUX->SECCFG.PINCM[PA24INDEX] = (uint32_t) 0x00000081;
    // DOE31_0 Data output enable
      GPIOA->DOE31_0 |= (1<<26)|(1<<25)|(1<<24);
      GPIOA->DOUTCLR31_0 = (1<<26)|(1<<25)|(1<<24); // LED1 off
}
// data specifies which LED to turn on
void LED_On(uint32_t data){
    // write this
    // use DOUTSET31_0 register so it does not interfere with other GPIO
  GPIOA->DOUTSET31_0 = data;
}

// data specifies which LED to turn off
void LED_Off(uint32_t data){
    // write this
    // use DOUTCLR31_0 register so it does not interfere with other GPIO
  GPIOA->DOUTCLR31_0 = data;
}

// data specifies which LED to toggle
void LED_Toggle(uint32_t data){
    // write this
    // use DOUTTGL31_0 register so it does not interfere with other GPIO
  GPIOA->DOUTTGL31_0 = data;
}
