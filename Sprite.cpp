#include "Sprite.h"
#include <stdint.h>
#include "../inc/ST7735.h"
#include <cstdint>
#include <cstdlib>

#define VXMULT 5
#define VYMULT 35


// Constructor
Sprite::Sprite() {
    x = y = vx = vy = 0;
    for (int i = 0; i < 4; i++) {
        images[i] = nullptr;
    }
    currentImageIndex = 0;
    lifeStatus = 1;
    needDraw = 0;
    direction = 0;
}


// Initialize the sprite with position and 4 image frames
void Sprite::Init(int32_t xInit, int32_t yInit,
                  const uint16_t* img0,
                  const uint16_t* img1,
                  const uint16_t* img2,
                  const uint16_t* img3,
                  uint8_t zoneInit) {
    x = xInit * VXMULT;
    y = yInit * VYMULT;
    images[0] = img0;
    images[1] = img1;
    images[2] = img2;
    images[3] = img3;
    currentImageIndex = 0;
    lifeStatus = 1; // alive
    needDraw = 1;
    direction = 0;
    zone = zoneInit;
    fallThroughCooldown = 0;
}


// Movement functions
void Sprite::MoveSprite(){
    x += vx;
    y += vy;
}

// Set sprite velocity
void Sprite::SetVelocity(int32_t vxNew, int32_t vyNew) {
    vx = vxNew;
    vy = vyNew;
}

void Sprite::SetXVelocity(int32_t vxNew){
    vx = vxNew;
}

void Sprite::SetYVelocity(int32_t vyNew){
    vy = vyNew;
}

int32_t Sprite::GetXVelocity(){
    return vx;
}

int32_t Sprite::GetYVelocity(){
    return vy;
}

// Draw the sprite to the screen
void Sprite::Draw() {
    if (lifeStatus && images[currentImageIndex]) {
        // No need for fixed-point conversion; use integer coordinates
        ST7735_DrawBitmap(x, y, images[currentImageIndex], 0, 0);
        needDraw = 0;
    }
}


// Set sprite life (1 = alive, 0 = dead)
void Sprite::SetLife(int32_t status) {
    lifeStatus = status;
    needDraw = 1;
}


// Check if sprite is alive
int Sprite::IsAlive() {
    return (lifeStatus != 0);
}


// Set the current image index (0–3)
void Sprite::SetImageIndex(int32_t index) {
    if (index >= 0 && index < 4) {
        currentImageIndex = index;
        needDraw = 1;
    }
}


// Get the current image index
int32_t Sprite::GetImageIndex(){
    return currentImageIndex;
}

const uint16_t* Sprite::GetImage(){
    return images[currentImageIndex];
}


int Sprite::CheckCollision(const Sprite& other) const {
    int32_t dx = abs(x/VXMULT - other.x/VXMULT);  // Horizontal distance
    int32_t dy = abs(y/VYMULT - other.y/VYMULT);  // Vertical distance
    int32_t dyH = abs((y/VYMULT - 9) - other.y/VYMULT);

    //int32_t dy2 = abs(y - other.y) - 20; // head collision
   
    // Use the smaller of the two distances (ECE319K distance)
    // int32_t ece319kDistance;
    // if (dx < dy) {
    //     ece319kDistance = dx;  // Use horizontal distance if it's smaller
    // } else {
    //     ece319kDistance = dy;  // Use vertical distance if it's smaller
    // }
    //number for too close
    int32_t tooClose = 4;


    // Check if the sprites are too close
    if (dx < tooClose && (dy < tooClose || dyH < tooClose)) {
        return 1;  // The sprites are too close
    }
    return 0;  // The sprites are not too close
}


//direction of sprite
void Sprite::SetDirection(uint8_t newDirection){
    direction = newDirection;
}

uint8_t Sprite::GetDirection(void){
    return direction;
}


// Get x and y positions
int32_t Sprite::GetX() {
    return x / VXMULT;
}


int32_t Sprite::GetY() {
    return y / VYMULT;
}

void Sprite::SetY(int32_t newY){
    y = newY * VYMULT;
}

void Sprite::SetX(int32_t newX){
    x = newX * VXMULT;
}


// Redraw management
void Sprite::MarkForDraw() {
    needDraw = 1;
}
void Sprite::ClearDrawFlag() {
    needDraw = 0;
}
int Sprite::NeedsDraw() {
    if(needDraw != 0){
        return 1;
    }
    return 0;
}

// uint8_t Sprite::getZone (void){
//        if(y < 130 && y > 109 && x < 105){
//           zone = 5;
//        }


//       else if(y < 132 && y > 107 && x > 6){
//           zone = 4;
//        }


//        else if(y < 107 && y > 81 && x < 105){
//           zone = 3;
//        }


//        else if(y < 81 && y > 55 && x > 6){
//           zone = 2;
//        }


//        else if(y < 55 && y > 35 && x < 105){
//           zone = 1;
//        }


//        else if(y < 29 && x > 6){
//           zone = 0;
//        }

//        else{
//         zone = 6;
//        }
//         return zone;
// }

int8_t Sprite::CheckGround(const uint16_t *bg, uint16_t color){
    if(fallThroughCooldown) return 0;

    int pixelX = 7 + x / VXMULT;
    int pixelY = (y / VYMULT) - 1;

    // Clamp to screen bounds
    if(pixelX < 0) pixelX = 0;
    if(pixelX > 120) pixelX = 122;  // leave room for +7
    if(pixelY < 0) pixelY = 0;
    if(pixelY > 159) pixelY = 159;

    int index = pixelX + (159 - pixelY) * 128;
    
    for(int i = 0; i < 5; i++){
        if(bg[index + i] == color){
            return 1;
        }
    }
    return 0;
}

void Sprite::GroundCorrect(const uint16_t *bg, uint16_t color){
    while(CheckGround(bg, color) == 1){
        y -= 1;
    }
    y++;
}

void Sprite::Gravity(const uint16_t *bg, uint16_t color){
    if(CheckGround(bg, color) == 0){
        if(vy <= 10){
            vy += 1;
        }
    }
    else if(y < 0){
        vy = 0;
    }
}

//fallthrough
uint16_t Sprite::FallThroughGet(void){
    return fallThroughCooldown;
}

void Sprite::FallThroughSet(uint16_t set){
    fallThroughCooldown = set;
}
