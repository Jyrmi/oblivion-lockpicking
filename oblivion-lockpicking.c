/*******************************************************************************************
*
*   raylib oblivion lockpicking
*
*   Welcome to Oblivion Lockpicking!
*
********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
    int lockpickFramesCounter = 0;
    int lockpickAnimationFramesDuration = 30; // TODO: adjust this on the fly to the pace of the pin when pressed
    int lockpickYAxis = screenHeight/2.0f - lockpickTexture.height/2.0f;
    int lockpickStartingXCoordinate = screenWidth/10.0f;
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
	
	// Tumbler pins
	struct Pin
	{
		bool succeeded;
		int motionState;
		int framesCounter;
		int animationFramesDuration; // Speed, in other words. A lower value means the pin will be traveling for less time.
		int maximumTravelDistance; // A constant equal to how far the pin travels in its tunnel.
		float difficulty; // The threshold from the start of the overall travel of the pin where an activation would result in breaking the lockpick and failing to lock the pin, for example, a 90% difficulty would mean activating the pin while it is at any point in the bottom 90% of its travel would result in destroying the lockpick and resetting the pin. Note: 100% difficulty is impossible to succeed on.
		Vector2 position;
	};
    int maxPins = 5;
	int pinYAxis = lockpickYAxis - lockpickTexture.height / 2.0f;
	int pinStartingXCoordinate = lockpickStartingXCoordinate + lockpickTexture.width * 0.95f; // the tip of the lockpick is 95% of the way to the right of the asset
	int pinAnimationFramesDuration = 25;
	float pinDifficulty = 0.95f;
	struct Pin pins[maxPins];
	for (int i = 0; i < maxPins; i++)
	{
		struct Pin pin =
		{
			false,
			0,
			0,
			pinAnimationFramesDuration,
			powf(pinAnimationFramesDuration / 2.0f, 2.0f),
			pinDifficulty,
			{ pinStartingXCoordinate + i * lockpickMoveDistancePerPin, pinYAxis } };
		pins[i] = pin;
	}
	Texture2D pinTexture = LoadTexture("resources/tumbler-pin-shadow.png");

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        switch (lockpickMotionState)
		{
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

		for (int i = 0; i < maxPins; i++)
		{
			switch (pins[i].motionState)
			{
				case 1: // Push the pin up
				{
					pins[i].framesCounter++;
					
					int pinAnimationFramesDuration = pins[i].animationFramesDuration;
					pins[i].position.y = pinYAxis + powf(pins[i].framesCounter - pinAnimationFramesDuration / 2.0f, 2.0f) - pins[i].maximumTravelDistance;
					
					if (pins[i].framesCounter >= pinAnimationFramesDuration)
					{
						pins[i].framesCounter = 0;
						pins[i].motionState = 0;
					}
				}
			}
		}
        //----------------------------------------------------------------------------------
        
        // Controls
        //----------------------------------------------------------------------------------
        // Check for keypresses here
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) // Move the lockpick left or right
        {
            int oldPosition = lockpickPinPosition;
            lockpickPinPosition = IsKeyPressed(KEY_LEFT) ? lockpickPinPosition - 1 : lockpickPinPosition + 1;
            if (lockpickPinPosition < 0) lockpickPinPosition = 0;
            if (lockpickPinPosition >= maxPins) lockpickPinPosition = maxPins - 1;
            if (oldPosition != lockpickPinPosition) lockpickMotionState = IsKeyPressed(KEY_LEFT) ? 1 : 2;
            lockpickVector.y = lockpickYAxis;
            lockpickFramesCounter = 0;
        }
        if (IsKeyPressed(KEY_SPACE) && !pins[lockpickPinPosition].succeeded) // Push the lockpick up OR lock the pin if in motion
        {
			if (pins[lockpickPinPosition].motionState == 0) // Push the lockpick and pin up
			{
				PlaySound(lockpickActivationSoundEffect);
				lockpickVector.x = lockpickStartingXCoordinate + lockpickPinPosition * lockpickMoveDistancePerPin;
				lockpickMotionState = 3;
				lockpickFramesCounter = 0;
				
				pins[lockpickPinPosition].motionState = 1;
				pins[lockpickPinPosition].framesCounter = 0;
			}
			else // Attempt to lock the pin in place
			{
				if (pins[lockpickPinPosition].position.y <= pinYAxis - pins[lockpickPinPosition].difficulty * pins[lockpickPinPosition].maximumTravelDistance)
				{
					PlaySound(lockpickBreakSoundEffect);
					pins[lockpickPinPosition].succeeded = true;
					pins[lockpickPinPosition].motionState = 0;
					pins[lockpickPinPosition].framesCounter = 0;
					pins[lockpickPinPosition].position.y = pinYAxis - pins[lockpickPinPosition].maximumTravelDistance; // Snap the pin to the top of its travel on success
				}
				else
				{
					PlaySound(lockpickBreakSoundEffect);
					lockpickVector.x = lockpickStartingXCoordinate + lockpickPinPosition * lockpickMoveDistancePerPin;
					lockpickVector.y = lockpickYAxis;
					lockpickBrokenLeftHalfVector.x = lockpickVector.x;
					lockpickBrokenRightHalfVector.x = lockpickVector.x + lockpickTextureBrokenLeftHalf.width;
					lockpickBrokenAnimationRotation = 0;
					lockpickScale = 1;
					lockpickMotionState = 4;
					lockpickFramesCounter = 0;
				}
			}
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(DARKBROWN);

			// Lockpick
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
                    DrawText("Broke", 300, 440, 30, RED);
                    DrawTextureEx(lockpickTextureBrokenLeftHalf, lockpickBrokenLeftHalfVector, lockpickBrokenAnimationRotation, lockpickScale, WHITE);
                    DrawTextureEx(lockpickTextureBrokenRightHalf, lockpickBrokenRightHalfVector, -lockpickBrokenAnimationRotation, lockpickScale, WHITE);
                } break;
            }
			
			// Tumbler pins
			int numSucceededPins = 0;
			for (int i = 0; i < maxPins; i++)
			{
				DrawTextureV(pinTexture, pins[i].position, WHITE);
				if (pins[i].succeeded)
				{
					numSucceededPins++;
				}
			}
			
			// Win state
			if (numSucceededPins == maxPins)
			{
				DrawText("Win", screenWidth / 2.0f, screenHeight * 3.0f / 4.0f, 100, GREEN);
			}

            DrawText("lockpick test", 300, 370, 20, WHITE);
            DrawText("Pin:", 300, 400, 30, WHITE);
            
            char result[1];
            sprintf(result, "%d", lockpickPinPosition);
            DrawText(result, 360, 400, 30, WHITE);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadSound(lockpickActivationSoundEffect);     // Unload sound data
    UnloadSound(lockpickBreakSoundEffect);     // Unload sound data
    CloseAudioDevice();     // Close audio device
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}