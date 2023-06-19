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
    int lockpickMotionState = 0;
    int lockpickFrameWidth = lockpickTexture.width;
    int lockpickFrameHeight = lockpickTexture.height;
    int lockpickFramesCounter = 0;
    int lockpickAnimationFramesDuration = 30;
    int lockpickYAxis = screenHeight/2.0f - lockpickTexture.height/2.0f;
    int lockpickPinPosition = 0;
    int lockpickMoveDistancePerPin = 40;
    int maxPins = 5;
    Rectangle lockpickSourceRec = { 0.0f, 0.0f, (float)lockpickFrameWidth, (float)lockpickFrameHeight };
    Vector2 lockpickVector = (Vector2){ screenWidth/2.0f - lockpickTexture.width/2.0f, lockpickYAxis };
    Vector2 lockpickBrokenLeftHalfVector = (Vector2){ lockpickVector.x, lockpickYAxis };
    Vector2 lockpickBrokenRightHalfVector = (Vector2){ lockpickVector.x + lockpickTextureBrokenLeftHalf.width, lockpickYAxis };
    float lockpickBrokenRotation = 0;
    float lockpickBrokenRotationMultiplier = 3;
    float lockpickBrokenScale = 1;
    float lockpickBrokenExplosionMultiplier = 10;

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        switch (lockpickMotionState) {
            case 1: // Push the lockpick up
            {
                lockpickFramesCounter++;
                lockpickVector.y = EaseSineOut((float)lockpickFramesCounter, lockpickYAxis, lockpickFrameHeight, -lockpickAnimationFramesDuration/2);
                
                if (lockpickFramesCounter >= lockpickAnimationFramesDuration)
                {
                    lockpickFramesCounter = 0;
                    lockpickMotionState = 0;
                }
            } break;
            case 2: // Break the lockpick
            {
                lockpickFramesCounter++;
                int lockpickBrokenExplosionDistance = lockpickBrokenExplosionMultiplier * lockpickFramesCounter;
                int lockpickBrokenYCoordinate = EaseQuadIn((float)lockpickFramesCounter, lockpickYAxis, 1, 1);
                lockpickBrokenLeftHalfVector.x = lockpickVector.x - lockpickBrokenExplosionDistance;
                lockpickBrokenRightHalfVector.x = lockpickVector.x + lockpickTexture.width/2.0f + lockpickBrokenExplosionDistance;
                lockpickBrokenLeftHalfVector.y = lockpickBrokenYCoordinate;
                lockpickBrokenRightHalfVector.y = lockpickBrokenYCoordinate;
                lockpickBrokenRotation = lockpickBrokenRotationMultiplier * lockpickFramesCounter;
                
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
                lockpickVector.x -= lockpickMoveDistancePerPin;
                lockpickVector.y = lockpickYAxis;
                lockpickFramesCounter = 0;
                lockpickMotionState = 0;
            }
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            int newLockpickPinPosition = lockpickPinPosition+1;
            if (newLockpickPinPosition < maxPins) {
                lockpickPinPosition = newLockpickPinPosition;
                lockpickVector.x += lockpickMoveDistancePerPin;
                lockpickVector.y = lockpickYAxis;
                lockpickFramesCounter = 0;
                lockpickMotionState = 0;
            }
        }
        if (IsKeyPressed(KEY_SPACE)) 
        {
            PlaySound(lockpickActivationSoundEffect);
            lockpickFramesCounter = 0;
            lockpickMotionState = 1;
        }
        if (IsKeyPressed(KEY_B))
        {
            PlaySound(lockpickBreakSoundEffect);
            lockpickVector.y = lockpickYAxis;
            lockpickFramesCounter = 0;
            lockpickBrokenRotation = 0;
            lockpickBrokenScale = 1;
            lockpickMotionState = 2;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            switch (lockpickMotionState) {
                case 0:
                case 1:
                {
                    DrawTextureV(lockpickTexture, lockpickVector, WHITE);
                } break;
                case 2:
                {
                    DrawText("Broke", 360, 440, 30, RED);
                    DrawTextureEx(lockpickTextureBrokenLeftHalf, lockpickBrokenLeftHalfVector, lockpickBrokenRotation, lockpickBrokenScale, WHITE);
                    DrawTextureEx(lockpickTextureBrokenRightHalf, lockpickBrokenRightHalfVector, -lockpickBrokenRotation, lockpickBrokenScale, WHITE);
                } break;
            }

            DrawText("lockpick test", 360, 370, 20, DARKGRAY);
            DrawText("Pin:", 360, 400, 30, DARKGRAY);
            
            char result[1];
            sprintf(result, "%d", lockpickPinPosition);
            DrawText(result, 420, 400, 30, DARKGRAY);

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