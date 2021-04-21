// First demo. You are not expected to understand it 
// (don't spend too much time trying and read on).
// But if you do understand (as a newbie): wow!
#include <deque>
#include <math.h>
#include <cstring>
#include <random>
#include <gba_input.h>

#define WALL_COLOR RGBToColor(0xbbafa4)
#define GRASS_COLOR RGBToColor(0x7da42d)
#define SNEK_HEAD_COLOR RGBToColor(0x225918)
#define SNEK_BODY_COLOR RGBToColor(0x2f690c)
#define APPLE_COLOR RGBToColor(0x82211d)

typedef struct Vector2 {
    unsigned short x;
    unsigned short y;
} Vector2;
unsigned short* buffer = (unsigned short*)0x06000000;

unsigned short randint(unsigned short a, unsigned short b){
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return (unsigned short)(a + r);
}

unsigned short RGBToColor(unsigned short r, unsigned short g, unsigned short b){
    return b/8*1024 + g/8*32 + r/8;
}

unsigned short RGBToColor(int rgb){
    unsigned short r = (rgb >> 16) & 0xff;
    unsigned short g = (rgb >> 8) & 0xff;
    unsigned short b = rgb & 0xff;
    return b/8*1024 + g/8*32 + r/8;
}

void drawNxN(unsigned short xoffset, unsigned short yoffset, unsigned short width, unsigned short height, unsigned short color){
    for (unsigned short x = 0; x < width; x++){
        for (unsigned short y = 0; y < height; y++){
            buffer[(xoffset+x)+(yoffset+y)*240] = color;
        }
    }
}

bool isPressed(unsigned short keys, unsigned short key){
    return (keys & key) == key;
}

bool inRange(int low, int high, int x) // https://www.geeksforgeeks.org/how-to-check-whether-a-number-is-in-the-rangea-b-using-one-comparison/
{
    return (low <= x) && (x <= high);
}

bool isColliding(std::deque<Vector2> snakePos, Vector2 colliderPos){
    snakePos.pop_front();
    for (auto i : snakePos){
        if (i.x == colliderPos.x && i.y == colliderPos.y) return true;
    }
    return false;
}

int main(){
        // Setup
        // ------------------------------------------------------------------------
        *(unsigned short*)0x04000000 = 0x0403; // Set mode, idk why but only this one works
        // ------------------------------------------------------------------------

    
    reset:
        // Setup Field
        // Field layout:
        //  t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t
        //  t w w w w w w w w w w w w w w w w w w w w w w w w w w w w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w g g g g g g g g g g g g g g g g g g g g g g g g g g w t
        //  t w w w w w w w w w w w w w w w w w w w w w w w w w w w w t
        //  t t t t t t t t t t t t t t t t t t t t t t t t t t t t t t
        //
        // t) text area (if i can figure that out)
        // w) wall
        // g) grass area/playable field
        //
        // ------------------------------------------------------------------------
        drawNxN(8, 8, 240-8*2, 160-8*2, WALL_COLOR); // Wall
        drawNxN(16, 16, 240-8*4, 160-8*4, GRASS_COLOR); // Grass
        // ------------------------------------------------------------------------

        // The game
        // ------------------------------------------------------------------------
        bool exitGame = false;
        Vector2 foodPos = {randint(0, 21), randint(0, 10)};
        int snakeDir = 0;
        std::deque<Vector2> snakePos;
        snakePos.push_back( {11, 10} );
        unsigned short framesdone = 0;

        while (!exitGame){ // Run until game should exit
            scanKeys(); // Populate keys pressed
            unsigned short keys = keysDown(); // Gets pressed keys 
            if (isPressed(keys, KEYPAD_BITS::KEY_UP)) snakeDir = 0; // if up is pressed set direction to 0
            if (isPressed(keys, KEYPAD_BITS::KEY_DOWN)) snakeDir = 1; // if down is pressed set direction to 1
            if (isPressed(keys, KEYPAD_BITS::KEY_LEFT)) snakeDir = 2; // if left is pressed set direction to 2
            if (isPressed(keys, KEYPAD_BITS::KEY_RIGHT)) snakeDir = 3; // if right is pressed set direction to 3
            if (isPressed(keys, KEYPAD_BITS::KEY_START)) exitGame = true; // if start is pressed end game loop

            if (framesdone == 32000) { // after 32000 iterations
                framesdone = 0; // Reset framesdone
                Vector2 tmpPos = snakePos[0]; // Get copy of current head position

                switch (snakeDir){ // Update snake location based on last input
                    case 0: 
                        tmpPos.y--; // up
                        break;

                    case 1:
                        tmpPos.y++; // down
                        break;
                    
                    case 2:
                        tmpPos.x--; // left
                        break;

                    case 3:
                        tmpPos.x++; // right
                        break;
                }

                snakePos.push_front(tmpPos); // Add new head location to snake body

                if ((tmpPos.x == foodPos.x) && (tmpPos.y == foodPos.y)){ // If head on food
                    foodPos = {randint(0, 21), randint(0, 10)}; // Randomize food location
                    while (isColliding(snakePos, foodPos)){ // If food is touching snake
                        foodPos = {randint(0, 25), randint(0, 15)}; // Then reset game
                    }
                } else { // head isn't on food, default to this
                    drawNxN(snakePos.back().x*8+16, snakePos.back().y*8+16, 8, 8, GRASS_COLOR); // Remove extra tail piece
                    snakePos.pop_back(); // Remove end of tail as snake must stay same length
                }

                if (!inRange(0, 25, snakePos[0].x) || !inRange(0, 15, snakePos[0].y) || isColliding(snakePos, snakePos[0])){ // If collided with wall or self
                    goto reset; // Go to start of program, easiest way to reset 
                }

                //  for (auto i : snakePos){
                //      drawNxN(i.x*8+16, i.y*8+16, 8, 8, SNEK_HEAD_COLOR);
                //  }
                drawNxN(snakePos[0].x*8+16, snakePos[0].y*8+16, 8, 8, SNEK_HEAD_COLOR); // Draw head
                if (snakePos.size() > 1) drawNxN(snakePos[1].x*8+16, snakePos[1].y*8+16, 8, 8, SNEK_BODY_COLOR); // If there should be body, redraw where head is to be body color
                //drawNxN(snakePos.back().x*8+16, snakePos.back().y*8+16, 8, 8, SNEK_BODY_COLOR); // Draw tail
                drawNxN(foodPos.x*8+16, foodPos.y*8+16, 8, 8, APPLE_COLOR); // Draw apple
            }
            framesdone++; // Increase iteration counter
        }
        // ------------------------------------------------------------------------

    return 0; // Have to return 0
}
