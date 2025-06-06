
#ifndef SOUND_H
#define SOUND_H
#include <stdint.h>

void Sound_Init(void);

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
void Sound_Start(const uint8_t *pt, uint32_t count);

// following 8 functions do not output to the DAC
// they configure pointers/counters and initiate the sound by calling Sound_Start
void Sound_Shoot(void);
uint32_t SoundPlaying(void);
void Sound_Killed(void);
void Sound_DKIntro(void);
void Sound_DKWin(void);
void Sound_DKGameOver(void);
void Sound_DKLevelTheme(void);
void Sound_Explosion(void);
void Sound_Fastinvader1(void);
void Sound_Fastinvader2(void);
void Sound_Fastinvader3(void);
void Sound_Fastinvader4(void);
void Sound_Highpitch(void);

#endif
