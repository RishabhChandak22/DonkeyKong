// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// your name
// your data 
#include <cstdint>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"

uint32_t Index = 0;
uint32_t Counter = 0;
const uint8_t *sound;

void SysTick_IntArm(uint32_t period, uint32_t priority){
  SysTick->CTRL = 0x00;      // disable SysTick during setup
  SysTick->LOAD = period-1;  // reload value
  SCB->SHP[1] = (SCB->SHP[1]&(~0xC0000000))|(priority<<30); // priority
  SysTick->VAL = 0;          
  SysTick->CTRL = 0x07;

}
// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
void Sound_Init(void){
  SysTick->CTRL = 0x00;      // disable SysTick during setup
  // SysTick->LOAD = period-1;  // reload value
  // SCB->SHP[1] = (SCB->SHP[1]&(~0xC0000000))|(priority<<30); // priority
  SysTick->VAL = 0;          
  SysTick->CTRL = 0x07;

  DAC5_Init();
 
}
extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ 
  if(Index > Counter){
    Index = 0;
    SysTick->CTRL = 0;
  }
  DAC5_Out(sound[Index]);  
  Index = (Index+1);  

}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement

uint32_t SoundPlaying(void){
  return Index;
}

void Sound_Start(const uint8_t *pt, uint32_t count){
  Counter = count;
  Index = 0;
  sound = pt;
  SysTick_IntArm(7256, 0);
  
}

void Sound_Shoot(void){
// write this
  Sound_Start( shoot, 4080);
}
void Sound_Killed(void){
// write this
  Sound_Start(invaderkilled, 3377);
}

void Sound_DKIntro(void){
  Sound_Start(DKGameStart, 44156);
}

void Sound_DKWin(void){
  Sound_Start(DKWin, 37452);
}

void Sound_DKGameOver(void){
  Sound_Start(DKGameOver, 33116);
}

void Sound_DKLevelTheme(void){
  Sound_Start(DKLevelThemeDrums, 18236);
}

void Sound_Explosion(void){
// write this

}

void Sound_Fastinvader1(void){

}
void Sound_Fastinvader2(void){

}
void Sound_Fastinvader3(void){

}
void Sound_Fastinvader4(void){

}
void Sound_Highpitch(void){

}
