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
    Texture2D lockpickTextureBrokenLeftHalf = LoadTexture("resources/lockpick-shadow-with-animation-space-broken-left-half.png");
    Texture2D lockpickTextureBrokenRightHalf = LoadTexture("resources/lockpick-shadow-with-animation-space-broken-right-half.png");
    Sound lockpickActivationSoundEffect = LoadSound("resources/freesound-tink.ogg");
    Sound lockpickBreakSoundEffect = LoadSound("resources/freesound-snap-1.ogg");
    int maxPins = 5;
    int lockpickMotionState = 0;
    int lockpickFramesCounter = 0;
    int lockpickAnimationFramesDuration = 30; // TODO: adjust this on the fly to the pace of the pin when pressed
    int lockpickYAxis = screenHeight/2.0f - lockpickTexture.height/2.0f;
    int lockpickStartingXCoordinate = screenWidth/2.0f - lockpickTexture.width/2.0f;
    int lockpickPinPosition = 0;
    int lockpickMoveDistancePerPin = 60;
    Vector2 lockpickVector = (Vector2){ lockpickStartingXCoordinate, lockpickYAxis };
    Vector2 lockpickBrokenLeftHalfVector = (Vector2){ lockpickVector.x, lockpickYAxis };
    Vector2 lockpickBrokenRightHalfVector = (Vector2){ lockpickVector.x + lockpickTextureBrokenLeftHalf.width, lockpickYAxis };
    float lockpickBrokenAnimationRotation = 0;
    float lockpickBrokenAnimationRotationMultiplier = 3;
    float lockpickBrokenAnimationDistanceMultiplier = 10;
    float lockpickMoveAnimationSpeedMultiplier = 10;
    float lockpickScale = 1;

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        switch (lockpickMotionState) {
            case 1: // Move the lockpick left
            case 2: // Move the lockpick right
            {
                lockpickFramesCounter++;
                
                bool directionToMove = lockpickVector.x <= lockpickStartingXCoordinate + lockpickPinPosition * lockpickMoveDistancePerPin;
                int moveAmount = (directionToMove ? 1 : -1) * (lockpickMoveDistancePerPin / (lockpickAnimationFramesDuration / lockpickMoveAnimationSpeedMultiplier));
                lockpickVector.x += moveAmount;
                
                if (lockpickFramesCounter >= lockpickAnimationFramesDuration / lockpickMoveAnimationSpeedMultiplier)
                {
                    lockpickFramesCounter = 0;
                    lockpickMotionState = 0;
                }
            } break;
            case 3: // Push the lockpick up
            {
                lockpickFramesCounter++;
                
                lockpickVector.y = EaseSineOut((float)lockpickFramesCounter, lockpickYAxis, lockpickTexture.height, -lockpickAnimationFramesDuration/2);
                
                if (lockpickFramesCounter >= lockpickAnimationFramesDuration)
                {
                    lockpickFramesCounter = 0;
                    lockpickMotionState = 0;
                }
            } break;
            case 4: // Break the lockpick
            {
                lockpickFramesCounter++;
                
                int lockpickBrokenExplosionDistance = lockpickBrokenAnimationDistanceMultiplier * lockpickFramesCounter;
                int lockpickBrokenYCoordinate = EaseQuadIn((float)lockpickFramesCounter, lockpickYAxis, 1, 1);
                
                lockpickBrokenLeftHalfVector.x = lockpickVector.x - lockpickBrokenExplosionDistance;
                lockpickBrokenRightHalfVector.x = lockpickVector.x + lockpickTextureBrokenLeftHalf.width + lockpickBrokenExplosionDistance;
                lockpickBrokenLeftHalfVector.y = lockpickBrokenYCoordinate;
                lockpickBrokenRightHalfVector.y = lockpickBrokenYCoordinate;
                lockpickBrokenAnimationRotation = lockpickBrokenAnimationRotationMultiplier * lockpickFramesCounter;
                
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
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) // Move the lockpick left or right
        {
            if (IsKeyPressed(KEY_LEFT) && lockpickPinPosition > 0)
            {
                lockpickPinPosition -= 1;
                lockpickMotionState = 1;
            }
            
            if (IsKeyPressed(KEY_RIGHT) && lockpickPinPosition < maxPins - 1)
            {
                lockpickPinPosition += 1;
                lockpickMotionState = 2;
            }
            
            lockpickVector.y = lockpickYAxis;
            lockpickFramesCounter = 0;
        }
        if (IsKeyPressed(KEY_SPACE)) // Push the lockpick up
        {
            PlaySound(lockpickActivationSoundEffect);
            lockpickVector.x = lockpickStartingXCoordinate + lockpickPinPosition * lockpickMoveDistancePerPin;
            lockpickFramesCounter = 0;
            lockpickMotionState = 3;
        }
        if (IsKeyPressed(KEY_B)) // Break the lockpick
        {
            PlaySound(lockpickBreakSoundEffect);
            lockpickVector.x = lockpickStartingXCoordinate + lockpickPinPosition * lockpickMoveDistancePerPin;
            lockpickVector.y = lockpickYAxis;
            lockpickBrokenLeftHalfVector.x = lockpickVector.x;
            lockpickBrokenRightHalfVector.x = lockpickVector.x + lockpickTextureBrokenLeftHalf.width;
            lockpickBrokenAnimationRotation = 0;
            lockpickScale = 1;
            lockpickFramesCounter = 0;
            lockpickMotionState = 4;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            switch (lockpickMotionState) {
                case 0:
                case 1:
                case 2:
                case 3:
                {
                    DrawTextureV(lockpickTexture, lockpickVector, WHITE);
                } break;
                case 4:
                {
                    DrawText("Broke", 360, 440, 30, RED);
                    DrawTextureEx(lockpickTextureBrokenLeftHalf, lockpickBrokenLeftHalfVector, lockpickBrokenAnimationRotation, lockpickScale, WHITE);
                    DrawTextureEx(lockpickTextureBrokenRightHalf, lockpickBrokenRightHalfVector, -lockpickBrokenAnimationRotation, lockpickScale, WHITE);
                } break;
            }

            DrawText("lockpick test", 360, 370, 20, DARKGRAY);
            DrawText("Pin:", 360, 400, 30, DARKGRAY);
            
            char result[1];
            sprintf(result, "%d", lockpickPinPosition);
            DrawText(result, 420, 400, 30, DARKGRAY);
            
            char xCoordinate[5];
            sprintf(xCoordinate, "%f", lockpickVector.x);       
            DrawText("X: ", 500, 400, 30, DARKGRAY);
            DrawText(xCoordinate, 540, 400, 30, DARKGRAY);

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