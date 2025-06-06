#ifndef SPRITE_H_
#define SPRITE_H_


#include <cstdint>
#include <stdint.h>


class Sprite {
private:
    int32_t x;      // x-coordinate (fixed point for sub-pixel precision)
    int32_t y;      // y-coordinate
    int32_t vx;     // velocity in x (pixels per 1/30 sec)
    int32_t vy;     // velocity in y
    const uint16_t* images[4]; // pointers to 4 different images (states)
    int32_t currentImageIndex;       // 0–3: current image to draw
    int32_t lifeStatus;              // 1 = alive, 0 = dead
    uint32_t needDraw;               // flag indicating whether to redraw
    uint8_t direction;              // Left is 1, right is 0
    uint8_t zone;                   // sprite's zone
    uint16_t fallThroughCooldown;    //fallthrough
public:
    // Constructor
    Sprite();


    // Initialize the sprite
    void Init(int32_t xInit, int32_t yInit,
              const unsigned short* img0,
              const unsigned short* img1,
              const unsigned short* img2,
              const unsigned short* img3,
              uint8_t zoneInit);

    void MoveSprite();



    // Set velocity
    void SetVelocity(int32_t vxNew, int32_t vyNew);

    void SetXVelocity(int32_t vxNew);

    void SetYVelocity(int32_t vyNew);

    int32_t GetXVelocity();

    int32_t GetYVelocity();

    // Draw sprite to the screen using the current image
    void Draw();


    // Kill or revive the sprite
    void SetLife(int32_t status); // 1 = alive, 0 = dead


    // Check if sprite is alive
    int IsAlive();


    // Set the current image index (0–3)
    void SetImageIndex(int32_t index);


    // Get the current image index
    int32_t GetImageIndex();

    //Get image
    const uint16_t* GetImage();

    // Check for collision with another sprite (AABB)
    int CheckCollision(const Sprite& other) const;


    //set direction
    void SetDirection(uint8_t newDirection);

    //get direction
    uint8_t GetDirection(void);

    // Getters for position
    int32_t GetX();
    int32_t GetY();

    void SetY(int32_t newY);
    void SetX(int32_t newX);


    // Mark sprite for redraw
    void MarkForDraw();


    // Clear draw flag
    void ClearDrawFlag();


    // Check if sprite needs to be drawn
    int NeedsDraw();

    uint8_t getZone();

    void Gravity(const uint16_t *bg, uint16_t color);
    void GroundCorrect(const uint16_t *bg, uint16_t color);
    int8_t CheckGround(const uint16_t *bg, uint16_t color);

    uint16_t FallThroughGet(void);
    void FallThroughSet(uint16_t);
};


#endif // SPRITE_H_


