#include <cstdint>
#include <machine/_stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "images/images.h"
#include "Sprite.h"
#include <cstdlib>

//#define DKMaxTime 350
#define BarrelAmt 6
#define WaitTime 1500

extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);
void image_add(int16_t x, int16_t y, const uint16_t *image, int16_t w, int16_t h, 
const uint16_t *bg, int8_t flipped);

Sprite mario;
Sprite DK;
Sprite barrels[BarrelAmt];
uint32_t jumpCooldown = 0;
int32_t y;
int32_t x;
int8_t climbable = 0;
uint8_t language = 0; //0 for English, 1 por Espanol
int8_t playerChar = 0;
uint32_t DKMaxTime = 350;
uint8_t barrelVelocity = 1;

// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t M=1;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

SlidePot Sensor(1500,0); // copy calibration from Lab 7

void image_addBLK(int16_t x, int16_t y, const uint16_t *image){
  int ctr = 0;
  for(int posy = 0; posy < 17; posy++){
      for(int posx = 0; posx < 17; posx++){
        if(image[ctr] != 0xFFFF){
          combined_image[ctr] = image[ctr];
        }
        else{
          combined_image[ctr] = 0x0000;
        }
        ctr++;
      }
    }

    ST7735_DrawBitmap(x, y, combined_image, 17, 17);
}

uint8_t StartScreen(){
  ST7735_DrawBitmap(0, 159, englishspanish, 128, 160);
  /*ST7735_SetCursor(5, 2);
  ST7735_OutString((char *)"Donkey Kong");
  ST7735_SetCursor(3, 5);
  ST7735_OutString((char *)"English  Espanol");
  ST7735_SetCursor(4, 7);
  ST7735_OutString((char *)" <-       ->");
  ST7735_DrawBitmap(53, 115, dkStand, 24, 24);*/
  int input = 0;
  while(((input & 1) != 1) && ((input & 4) != 4)){
    input = Switch_In();

    //Spanish
    if((input & 1) == 1){
      language = 0;
    }

    //English
    if((input & 4) == 4){
      language = 1;
    }
  }
  return language;
}

void DirectionsScreen(){
  ST7735_FillScreen(ST7735_BLACK);
  if(language == 1){
    ST7735_SetCursor(1, 4);
    ST7735_OutString((char *)"Move: left, right");

    ST7735_SetCursor(1, 5);
    ST7735_OutString((char *)"Climb: up, down");

    ST7735_SetCursor(1, 6);
    ST7735_OutString((char *)"Jump: up");

    ST7735_SetCursor(1, 7);
    ST7735_OutString((char *)"Reach top to win!");

    ST7735_SetCursor(1, 9);
    ST7735_OutString((char *)"Press to continue");
  }

  else{
    ST7735_SetCursor(1, 2);
    ST7735_OutString((char *)"Mover: ");
    ST7735_SetCursor(1, 3);
    ST7735_OutString((char *)"izquierdo, derecho");

    ST7735_SetCursor(1, 5);
    ST7735_OutString((char *)"Subir: ");
    ST7735_SetCursor(1, 6);
    ST7735_OutString((char *)"arriba, abajo");

    ST7735_SetCursor(1, 8);
    ST7735_OutString((char *)"Saltar:");
    ST7735_SetCursor(1, 9);
    ST7735_OutString((char *)"arriba");

    ST7735_SetCursor(1, 11);
    ST7735_OutString((char *)"Sube para ganar!");

    ST7735_SetCursor(1, 12);
    ST7735_OutString((char *)"Presione para");
    ST7735_SetCursor(1, 13);
    ST7735_OutString((char *)"continuar");

  }

  Clock_Delay1ms(500); 
  int input = 0;
  while(!input){
    input = Switch_In();
  }
}

void PickCharScreen(){
  ST7735_FillScreen(ST7735_BLACK);
  image_addBLK(20, 100, mariostand);
  image_addBLK(90, 100, luigistand);

  if(language == 1){
    ST7735_SetCursor(1, 3);
    ST7735_OutString((char *)"Difficulty: ");
    ST7735_SetCursor(1, 4);
    ST7735_OutString((char *)"Slider");
    ST7735_SetCursor(1, 6);
    ST7735_OutString((char *)"Character:");
  }
  else{
    ST7735_SetCursor(1, 3);
    ST7735_OutString((char *)"Dificultad:");
    ST7735_SetCursor(1, 4);
    ST7735_OutString((char *)"Control deslizante");
    ST7735_SetCursor(1, 6);
    ST7735_OutString((char *)"Personaje:");
  }

  uint32_t diffSlider = Sensor.In();
  Clock_Delay1ms(500); 
  int input = 0;
  while(((input & 1) != 1) && ((input & 4) != 4)){
    diffSlider = Sensor.In();
    input = Switch_In();
    if((input & 1) == 1){
      mario.Init(4, 152, luigistand, luigimove, luigiclimb, luigistand, 0);
      playerChar = 1;
    }
    if((input & 4) == 4){
      mario.Init(4, 152, mariostand, mariomove, marioclimb, mariostand, 0);
      playerChar = 0;
    }
  }

  if(diffSlider < 800){
    DKMaxTime = 900;
    barrelVelocity = 1;
  }
  else if (diffSlider < 3000){
    DKMaxTime = 500;
    barrelVelocity = 2;
  }
  else{
    DKMaxTime = 350;
    barrelVelocity = 2;
  }

}

//level1 ladders
int8_t LadderCheck(){
  int32_t xVal = mario.GetX();
  int32_t yVal = mario.GetY();
  if(xVal > 90 && xVal < 97 && yVal > 132 && yVal < 153) return 1;
  else if(xVal > 10 && xVal < 17 && yVal < 127 && yVal > 106) return 1;
  else if(xVal > 58 && xVal < 65 && yVal < 130 && yVal > 103) return 1;
  else if(xVal > 93 && xVal < 100 && yVal < 101 && yVal > 81) return 1;
  //change bg on last 2 rows
  else if(xVal > 5 && xVal < 12 && yVal < 74 && yVal > 55) return 1;
  else if(xVal > 80 && xVal < 87 && yVal < 51 && yVal > 29) return 1;

  return 0;
}

uint8_t animationCount = 5;
//movement
void PlayerMove(){
  int input = Switch_In();

    //move right
    if((input & 1) == 1){
      mario.SetXVelocity(1);
    }

    //move left
    if((input & 4) == 4){
      mario.SetXVelocity(-1);
    }

    //up button
    climbable = LadderCheck();
    if((input & 8) == 8){
      //climb ladder
      if(climbable == 1){
        mario.SetYVelocity(-8);
      }
      //jump
      else if(mario.CheckGround(bg1, 0x72D9) == 1 && jumpCooldown == 0){
        mario.SetYVelocity(-29);
        jumpCooldown = 100;
      }
    }

    uint16_t fallthrough = mario.FallThroughGet();
    //down button
    if((input & 2) == 2){
      //climb down
      if(climbable == 1){
        mario.SetYVelocity(8);
      }
      //fallthrough
      else if(mario.CheckGround(bg1, 0x72D9) == 1 && fallthrough != 1){
        mario.FallThroughSet(20);
      }
    }

    if(((input & 2) != 2) && ((input & 8) != 8) || ((input & 2) == 2) && ((input & 8) == 8)){
      if(climbable){
        mario.SetYVelocity(0);
      }
    }

    if(jumpCooldown){
      jumpCooldown--;
    }

    
    if(fallthrough){
      mario.FallThroughSet(fallthrough-1);
    }

    //no horizontal movement
    if(((input & 1) != 1) && ((input & 4) != 4) || ((input & 1) == 1) && ((input & 4) == 4)){
      mario.SetXVelocity(0);
    }

    //set mario's direction
    if(mario.GetXVelocity() > 0){
      mario.SetDirection(0);
    }

    if(mario.GetXVelocity() < 0){
      mario.SetDirection(1);
    }
  
    //Apply gravity (if not grounded)
    if(climbable == 0) mario.Gravity(bg1, 0x72D9);

    //animation
    if(mario.CheckGround(bg1, 0x72D9) == 0){
      if(climbable){
        mario.SetImageIndex(2);
      }
      else{
        mario.SetImageIndex(1);
      }
    }
    else if(mario.GetXVelocity() != 0){
      if(mario.GetImageIndex() > 0 && animationCount == 0){
        mario.SetImageIndex(0);
      }
      else if(animationCount == 0){
        mario.SetImageIndex(1);
      }
    }
    else{
      mario.SetImageIndex(0);
    }

    if(animationCount){
      animationCount--;
    }
    else{
      animationCount = 5;
    }

    mario.MoveSprite();

    //correction for bounds
    if(mario.GetX() > 115) mario.SetX(115);
    if(mario.GetX() < -4) mario.SetX(-4);
    if(mario.GetY() > 156) mario.SetY(156);

    if(climbable == 0) mario.GroundCorrect(bg1, 0x72D9);

    x = mario.GetX();
    y = mario.GetY();
}


//DonkeyKong

void BarrelEdges(Sprite &barrel){
  int32_t xVal = barrel.GetX();
  int32_t yVal = barrel.GetY();
  if(xVal > 120) barrel.SetXVelocity(-1 * barrelVelocity);
  else if(yVal > 10  && yVal < 130 && xVal < -6) barrel.SetXVelocity(barrelVelocity);
  else if(yVal > 130 && xVal < -11){
    barrel.SetXVelocity(0);
  }
}

uint16_t DKTimer = DKMaxTime;
uint8_t BarrelCounter = 0;
void DKEngine(void){
  switch(DKTimer){
    case 300:
      DK.SetImageIndex(1);
      break;
    case 200:
      DK.SetImageIndex(2);
      barrels[BarrelCounter].SetX(60);
      barrels[BarrelCounter].SetY(28);
      barrels[BarrelCounter].SetXVelocity(0);
      break;
    case 0:
      DK.SetImageIndex(0);
      barrels[BarrelCounter].SetXVelocity(barrelVelocity);
      DKTimer = DKMaxTime;
      BarrelCounter++;
      break;
    default:
      break;
  }

  for(int i = 0; i < BarrelAmt; i++){
    if(barrels[i].GetXVelocity() != 0){
      barrels[i].Gravity(bg1, 0x72D9);
      barrels[i].MoveSprite();
      barrels[i].GroundCorrect(bg1, 0x72D9);
      BarrelEdges(barrels[i]);
    }
    
  }

  if(BarrelCounter == BarrelAmt) BarrelCounter = 0;
  DKTimer--;
}


// games  engine runs at 30Hz
int gameOver = 0;
// 0 = level 1 gameplay
// 1 = lose
// 2 = after lose waiting for input
// 3 = win 
int gameOverWait = 300;
int gameStart = WaitTime;
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)


    
    if(gameStart){
      gameStart--;
    }

    if(gameOver == 0){
      if(gameStart == 0){  
        PlayerMove();
      }
      DKEngine();
      
      for(int i = 0; i < BarrelAmt; i++){
        if(mario.CheckCollision(barrels[i])){
          gameOver = 1;
          break;
        }
      }

      if(SoundPlaying() == 1){
        Sound_DKLevelTheme();
      }

      if(mario.GetX() > 58 && mario.GetX() < 73 && mario.GetY() < 30){
        gameOver = 3;
      }

    }
    else if(gameOver == 2){
      if(gameOverWait){
        gameOverWait--;
      }
    }
    

    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}
uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};

//add images
void image_add(int16_t x, int16_t y, const uint16_t *image, int16_t w, int16_t h, 
const uint16_t *bg, int8_t flipped){
  int ctr = 0;
  int bg_index;

  if(flipped == 0){
    for(int posy = 0; posy < h; posy++){
      for(int posx = 0; posx < w; posx++){
        if(image[ctr] != 0xFFFF){
          combined_image[ctr] = image[ctr];
        }
        else{
          bg_index = x + posx + (159 - y + posy) * 128;
          combined_image[ctr] = bg[bg_index];
        }
        ctr++;
      }
    }
  }
  else{
    for(int posy = 0; posy < h; posy++){
      for(int posx = 0; posx < w; posx++){
        if(image[16 - posx + (posy * 17)] != 0xFFFF){
          combined_image[ctr] = image[16 - posx + (posy * 17)];
        }
        else{
          bg_index = x + posx + (159 - y + posy) * 128;
          combined_image[ctr] = bg[bg_index];
        }
        ctr++;
      }
    }
  }
  ST7735_DrawBitmap(x, y, combined_image, w, h);
}

// use main1 to observe special characters
int main1(void){ // main1
    char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  ST7735_FillScreen(0x0000);            // set screen to black
  for(int myPhrase=0; myPhrase<= 2; myPhrase++){
    for(int myL=0; myL<= 3; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Clock_Delay1ms(3000);
  ST7735_FillScreen(0x0000);       // set screen to black
  l = 128;
  while(1){
    Clock_Delay1ms(2000);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }
}

// use main2 to observe graphics
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  // ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  // ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  // ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  // ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  // ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom
  // ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  // ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  // ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  // ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  // ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);
  //ST7735_DrawBitmap(0, 159, bg1, 128, 160);
  ST7735_DrawBitmap(0, 159, bg1, 128, 160);
  image_add(0, 159, mariomove, 17, 17, bg1, 0);
  image_add(50, 50, mariostand, 17, 17, bg1, 1);

  while(1);

  for(uint32_t t=500;t>0;t=t-5){
    SmallFont_OutVertical(t,104,6); // top left
    Clock_Delay1ms(50);              // delay 50 msec
  }
  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString((char *)"GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString((char *)"Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString((char *)"Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  while(1){
  }
}

// use main3 to test switches and LEDs
int main3(void){ // main3
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  while(1){
    // write code to test switches and LEDs

  }
}
// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();
  Sound_DKLevelTheme();
  while(1){

    now = Switch_In(); // one of your buttons
    if((last == 0)&&(now == 1)){
      Sound_Shoot(); // call one of your sounds
    }
    if((last == 0)&&(now == 2)){
      Sound_Killed(); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
  }
}


// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  Sensor.Init(); // PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
  
  // initialize all data structures
  __enable_irq();


  
  DK.Init(41, 27, dkStand, dkGrab, dkGrab2, dkGrab, 0);
  for(int i = 0; i < BarrelAmt; i++){
    barrels[i].Init(0, 0, barrelImg, barrelImg, barrelImg, barrelImg, 0);
  }

  StartScreen();
  DirectionsScreen();
  PickCharScreen();

  ST7735_DrawBitmap(0, 159, bg1, 128, 160);


  TimerG12_IntArm(266666 * 2, 1);

  while(1){
    if(gameOver == 1){
      ST7735_FillScreen(0x0000);   // set screen to black
      if(language == 1){
        ST7735_SetCursor(6, 4);
        ST7735_OutString((char *)"GAME OVER");
        ST7735_SetCursor(5, 11); 
        ST7735_OutString((char *)"SCORE: ");
        ST7735_OutUDec(abs(153 - mario.GetY()) * 100);
        ST7735_SetCursor(3, 13);
        ST7735_OutString((char *)"PRESS TO RESTART"); 
      }
      else{
        ST7735_SetCursor(6, 4);
        ST7735_OutString((char *)"TERMINADO");
        ST7735_SetCursor(4, 11); 
        ST7735_OutString((char *)"PUNTAJE: "); 
        ST7735_OutUDec(abs(153 - mario.GetY()) * 100);
        ST7735_SetCursor(4, 13);
        ST7735_OutString((char *)"PRESIONE PARA"); 
        ST7735_SetCursor(6, 14);
        ST7735_OutString((char *)"REINICIAR");
      }
        
      
      ST7735_DrawBitmap(50, 90, dkStand, 24, 24);
      gameOverWait = 300;
      
      Sound_Killed();
      gameOver = 2;
    }
    else if(gameOver == 0){

      if(gameStart == 0) image_add(mario.GetX(), mario.GetY(), mario.GetImage(), 17, 17, bg1, mario.GetDirection());
      else if(gameStart <= (WaitTime / 4)){
        ST7735_SetCursor(2, 14); 
        ST7735_OutString((char *)"1");
      }
      else if(gameStart <= (WaitTime / 4) * 2){
        ST7735_SetCursor(2, 14); 
        ST7735_OutString((char *)"2");
      }
      else if(gameStart <= (WaitTime / 4) * 3){
        ST7735_SetCursor(2, 14); 
        ST7735_OutString((char *)"3");
      }
      
      for(int i = 0; i < BarrelAmt; i++){
        image_add(barrels[i].GetX(), barrels[i].GetY(), barrels[i].GetImage(), 17, 9, bg1, barrels[i].GetDirection());
      }

      ST7735_DrawBitmap(DK.GetX(), DK.GetY(), DK.GetImage(), 24, 24);
    }

    if(gameOver == 2){
      if(Switch_In() && !gameOverWait){
        ST7735_DrawBitmap(0, 159, bg1, 128, 160);
        for(int i = 0; i < BarrelAmt; i++){
          barrels[i].Init(0, 0, barrelImg, barrelImg, barrelImg, barrelImg, 0);
          barrels[i].SetVelocity(0, 0);
        }
        if(playerChar){
          mario.Init(4, 152, luigistand, luigimove, luigiclimb, luigistand, 0);
        }
        else{
          mario.Init(4, 152, mariostand, mariomove, marioclimb, mariostand, 0);
        }
        DKTimer = DKMaxTime;
        gameStart = WaitTime;
        BarrelCounter = 0;
        gameOver = 0;
      }
    }

    if(gameOver == 3){
      ST7735_FillScreen(0x0000);   // set screen to black
      if(language == 1){
        ST7735_SetCursor(7, 4);
        ST7735_OutString((char *)"YOU WIN!");
        ST7735_SetCursor(5, 11); 
        ST7735_OutString((char *)"SCORE: ");
        ST7735_OutUDec(abs(153 - mario.GetY()) * 100);
        ST7735_SetCursor(3, 13);
        ST7735_OutString((char *)"PRESS TO RESTART"); 
      }
      else{
        ST7735_SetCursor(7, 4);
        ST7735_OutString((char *)"TU GANAS");
        ST7735_SetCursor(4, 11); 
        ST7735_OutString((char *)"PUNTAJE: "); 
        ST7735_OutUDec(abs(153 - mario.GetY()) * 100);
        ST7735_SetCursor(4, 13);
        ST7735_OutString((char *)"PRESIONE PARA"); 
        ST7735_SetCursor(6, 14);
        ST7735_OutString((char *)"REINICIAR");
      }

      mario.SetImageIndex(1);
      image_addBLK(55, 81, mario.GetImage());
      gameOverWait = 300;
      gameOver = 2;
    }
  }
}
