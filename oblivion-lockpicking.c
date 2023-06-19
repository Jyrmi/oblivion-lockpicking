/*******************************************************************************************
*
*   raylib oblivion lockpicking
*
*   Welcome to Oblivion Lockpicking!
*
********************************************************************************************/

#include<stdio.h>
#include "raylib.h"
#include "reasings.h"            // Required for easing functions

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1200;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "raylib oblivion lockpicking");
    InitAudioDevice();

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Lockpick
    Texture2D lockpickTexture = LoadTexture("resources/lockpick-shadow-with-animation-space.png");
    Sound lockpickActivationSoundEffect = LoadSound("resources/freesound-metal-tick-3.ogg");
    int lockpickMotionState = 0;
    int lockpickFrameWidth = lockpickTexture.width;
    int lockpickFrameHeight = lockpickTexture.height;
    int lockpickFramesCounter = 0;
    int lockpickAnimationFramesDuration = 20;
    int lockpickPinPosition = 0;
    int maxPins = 5;
    int pinXCoordinates[5] = { 0, 100, 200, 300, 400 };
    Rectangle lockpickSourceRec = { 0.0f, 0.0f, (float)lockpickFrameWidth, (float)lockpickFrameHeight };
    Rectangle lockpickBoundsRec = { screenWidth/2.0f - lockpickTexture.width/2.0f, screenHeight/2.0f - lockpickTexture.height/2.0f, (float)lockpickTexture.width, lockpickFrameHeight };
    Vector2 lockpickBoundsVector = (Vector2){ lockpickBoundsRec.x, lockpickBoundsRec.y };

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        switch (lockpickMotionState) {
            case 1:
            case 2:
            {
                /*
                lockpickFramesCounter++;
                int newXCoordinate = pinXCoordinates[lockpickPinPosition];
                lockpickSourceRec.x = EaseLinearIn((float)lockpickFramesCounter, lockpickSourceRec.x, newXCoordinate, newXCoordinate*3);
                
                if (lockpickFramesCounter >= lockpickAnimationFramesDuration)
                {
                    lockpickFramesCounter = 0;
                    lockpickMotionState = 0;
                }
                */
            } break;
            case 3:
            {
                lockpickFramesCounter++;
                lockpickSourceRec.y = EaseSineInOut((float)lockpickFramesCounter, 0, lockpickFrameHeight/2.0f, 10);
                
                if (lockpickFramesCounter >= lockpickAnimationFramesDuration)
                {
                    lockpickFramesCounter = 0;
                    lockpickMotionState = 0;
                }
            } break;
        }
        //----------------------------------------------------------------------------------
        
        // Controls
        //----------------------------------------------------------------------------------
        // Check for keypresses here
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_LEFT)) {
            int newLockpickPinPosition = lockpickPinPosition-1;
            if (newLockpickPinPosition >= 0) {
                lockpickPinPosition = newLockpickPinPosition;
                lockpickSourceRec.x -= 40;
                lockpickBoundsRec.x -= 40;
                lockpickBoundsVector.x -= 40;
                lockpickMotionState = 1;
            }
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            int newLockpickPinPosition = lockpickPinPosition+1;
            if (newLockpickPinPosition < maxPins) {
                lockpickPinPosition = newLockpickPinPosition;
                lockpickSourceRec.x += 40;
                lockpickBoundsRec.x += 40;
                lockpickBoundsVector.x += 40;
                lockpickMotionState = 2;
            }
        }
        if (IsKeyPressed(KEY_SPACE)) 
        {
            PlaySound(lockpickActivationSoundEffect);
            lockpickMotionState = 3;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTextureRec(lockpickTexture, lockpickSourceRec, lockpickBoundsVector, WHITE);

            DrawText("this IS a lockpick test!", 360, 370, 20, DARKGRAY);
            
            char result[1];
            sprintf(result, "%d", lockpickPinPosition);
            DrawText(result, 360, 400, 30, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadSound(lockpickActivationSoundEffect);     // Unload sound data
    CloseAudioDevice();     // Close audio device
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}