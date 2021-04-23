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
#define BLACK 0x0000
#define GREY RGBToColor(0x808080)

#define GREEN_1 RGBToColor(0x518822)
#define GREEN_2 RGBToColor(0x2f690c)
#define GREEN_3 RGBToColor(0x225918)
#define GREEN_4 RGBToColor(0x174a1b)

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
        // ✧ Setup ✧
        // ------------------------------------------------------------------------
        *(unsigned short*)0x04000000 = 0x0403; // Set mode, idk why but only this one works
        // ------------------------------------------------------------------------

        // ✧ Title screen ✧
        // ------------------------------------------------------------------------
        bool exitStart = false;
        unsigned short keys;

        // S
        // ------------------------------------------------------------------------
            drawNxN(12, 12, 16, 8, SNEK_BODY_COLOR);
            drawNxN(28, 12, 8, 8, SNEK_HEAD_COLOR);
            drawNxN(36, 20, 8, 8, APPLE_COLOR);
            drawNxN(4, 20, 8, 16, GRASS_COLOR);
            drawNxN(12, 36, 24, 8, GRASS_COLOR);
            drawNxN(36, 44, 8, 24, GRASS_COLOR);
            drawNxN(4, 60, 8, 8, GRASS_COLOR);
            drawNxN(12, 68, 24, 8, GRASS_COLOR);
        // n
        // ------------------------------------------------------------------------
            drawNxN(52, 28, 8, 48, SNEK_BODY_COLOR);
            drawNxN(60, 28, 8, 8, SNEK_HEAD_COLOR);
            drawNxN(68, 28, 8, 8, GRASS_COLOR);
            drawNxN(76, 28, 8, 8, APPLE_COLOR);
            drawNxN(84, 36, 8, 40, GRASS_COLOR);
        // a
        // ------------------------------------------------------------------------
            drawNxN(100, 36, 8, 32, GRASS_COLOR);
            drawNxN(108, 28, 16, 8, GRASS_COLOR);
            drawNxN(108, 68, 16, 8, GRASS_COLOR);
            drawNxN(124, 28, 16, 8, SNEK_BODY_COLOR);
            drawNxN(132, 36, 8, 32, SNEK_BODY_COLOR);
            drawNxN(132, 68, 8, 8, SNEK_HEAD_COLOR);
            drawNxN(124, 60, 8, 8, APPLE_COLOR);
        // k
        // ------------------------------------------------------------------------
            drawNxN(148, 4, 8, 8, APPLE_COLOR);
            drawNxN(148, 12, 8, 24, GRASS_COLOR);
            drawNxN(148, 36, 8, 8, SNEK_HEAD_COLOR);
            drawNxN(148, 44, 8, 16, SNEK_BODY_COLOR);
            drawNxN(148, 60, 8, 16, GRASS_COLOR);
            drawNxN(156, 52, 16, 8, SNEK_BODY_COLOR);
            drawNxN(164, 44, 8, 8, GRASS_COLOR);
            drawNxN(172, 36, 8, 8, GRASS_COLOR);
            drawNxN(180, 28, 8, 8, GRASS_COLOR);
            drawNxN(172, 60, 8, 8, GRASS_COLOR);
            drawNxN(180, 68, 8, 8, GRASS_COLOR);
        // e
        // ------------------------------------------------------------------------
            drawNxN(204, 28, 24, 8, GRASS_COLOR);
            drawNxN(196, 36, 8, 8, GRASS_COLOR);
            drawNxN(228, 36, 8, 8, APPLE_COLOR);
            drawNxN(196, 44, 32, 8, SNEK_BODY_COLOR);
            drawNxN(196, 52, 8, 16, SNEK_BODY_COLOR);
            drawNxN(204, 68, 24, 8, GRASS_COLOR);
            drawNxN(228, 60, 8, 8, GRASS_COLOR);
            drawNxN(228, 44, 8, 8, APPLE_COLOR);
        // ------------------------------------------------------------------------

        // Press
        // ------------------------------------------------------------------------
            drawNxN(62, 78, 4, 32, GREY);
            drawNxN(66, 78, 12, 4, GREY);
            drawNxN(66, 94, 12, 4, GREY);
            drawNxN(78, 82, 4, 12, GREY);
            drawNxN(86, 86, 4, 24, GREY);
            drawNxN(90, 86, 12, 4, GREY);
            drawNxN(102, 90, 4, 4, GREY);
            drawNxN(110, 90, 4, 16, GREY);
            drawNxN(114, 86, 12, 4, GREY);
            drawNxN(114, 94, 12, 4, GREY);
            drawNxN(114, 106, 12, 4, GREY);
            drawNxN(126, 90, 4, 8, GREY);
            drawNxN(126, 102, 4, 4, GREY);
            drawNxN(138, 86, 16, 4, GREY);
            drawNxN(134, 106, 16, 4, GREY);
            drawNxN(134, 90, 4, 4, GREY);
            drawNxN(138, 94, 12, 4, GREY);
            drawNxN(150, 98, 4, 8, GREY);
            drawNxN(138+24, 86, 16, 4, GREY);
            drawNxN(134+24, 106, 16, 4, GREY);
            drawNxN(134+24, 90, 4, 4, GREY);
            drawNxN(138+24, 94, 12, 4, GREY);
            drawNxN(150+24, 98, 4, 8, GREY);
        // ------------------------------------------------------------------------

        // Start
        // ------------------------------------------------------------------------
            drawNxN(46, 112, 12, 44, GREEN_1);
            drawNxN(50, 116, 8, 36, BLACK);
            drawNxN(66, 120, 12, 4, GREEN_2);
            drawNxN(66, 132, 12, 4, GREEN_2);
            drawNxN(66, 148, 12, 4, GREEN_2);
            drawNxN(62, 124, 4, 8, GREEN_2);
            drawNxN(78, 136, 4, 12, GREEN_2);
            drawNxN(78, 124, 4, 4, GREEN_2);
            drawNxN(62, 144, 4, 4, GREEN_2);
            drawNxN(86, 120, 20, 4, GREEN_3);
            drawNxN(94, 124, 4, 28, GREEN_3);
            drawNxN(114, 120, 12, 4, GREEN_4);
            drawNxN(110, 124, 4, 28, GREEN_4);
            drawNxN(126, 124, 4, 28, GREEN_4);
            drawNxN(114, 136, 12, 4, GREEN_4);
            drawNxN(134, 120, 4, 32, GREEN_3);
            drawNxN(138, 120, 12, 4, GREEN_3);
            drawNxN(138, 132, 12, 4, GREEN_3);
            drawNxN(150, 124, 4, 8, GREEN_3);
            drawNxN(146, 136, 4, 4, GREEN_3);
            drawNxN(150, 140, 4, 12, GREEN_3);
            drawNxN(158, 120, 20, 4, GREEN_2);
            drawNxN(166, 124, 4, 28, GREEN_2);
            drawNxN(182, 112, 12, 44, GREEN_1);
            drawNxN(182, 116, 8, 36, BLACK);
        // ------------------------------------------------------------------------
        
        while (!exitStart){ // Run until menu should exit to game
            scanKeys(); // Populate keys pressed
            unsigned short keys = keysDown(); // Gets pressed keys 
            if (isPressed(keys, KEYPAD_BITS::KEY_START)) exitStart = true; // if start is pressed end game loop
        }
        // ------------------------------------------------------------------------

    
    reset:
        // ✧ Setup Field ✧
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
        drawNxN(0, 0, 240, 160, BLACK); // Clear Back
        drawNxN(8, 8, 240-8*2, 160-8*2, WALL_COLOR); // Wall
        drawNxN(16, 16, 240-8*4, 160-8*4, GRASS_COLOR); // Grass
        // ------------------------------------------------------------------------

        // ✧ The game ✧
        // ------------------------------------------------------------------------
        bool firstRan = false, pause = false;
        Vector2 foodPos = {randint(0, 21), randint(0, 10)};
        int snakeDir = 0;
        std::deque<Vector2> snakePos;
        snakePos.push_back( {11, 10} );
        unsigned short framesdone = 0;

        while (true){ // Run until game should exit
            scanKeys(); // Populate keys pressed
            keys = keysDown(); // Gets pressed keys 
            if (isPressed(keys, KEYPAD_BITS::KEY_UP)) snakeDir = 0; // if up is pressed set direction to 0
            if (isPressed(keys, KEYPAD_BITS::KEY_DOWN)) snakeDir = 1; // if down is pressed set direction to 1
            if (isPressed(keys, KEYPAD_BITS::KEY_LEFT)) snakeDir = 2; // if left is pressed set direction to 2
            if (isPressed(keys, KEYPAD_BITS::KEY_RIGHT)) snakeDir = 3; // if right is pressed set direction to 3
            if (isPressed(keys, KEYPAD_BITS::KEY_SELECT) || isPressed(keys, KEYPAD_BITS::KEY_START)) pause = !pause; // if selecto or start is pressed pause game loop
            
            if (!pause || !firstRan) {
                if (framesdone >= 32000) { // after 32000 iterations
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

                    firstRan = true; // Reset framesdone
                }
                framesdone++; // Increase iteration counter
            } else {
                framesdone = 0; // Reset framesdone
            }
        }
        // ------------------------------------------------------------------------

    return 0; // Have to return 0
}
