#include "include/raylib.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <numeric>

#include "include/rlgl.h"           // Required for: Vertex definition
#include "include/raymath.h"

#define MAX_POINTS  11      // 10 points and back to the start


void swap(int *x_pointer, int *y_pointer)
{
    int temp = *x_pointer;
    *x_pointer = *y_pointer;
    *y_pointer = temp;
}


// Draw textured polygon, defined by vertex and texture coordinates
void DrawTexturePoly(Texture2D texture, Vector2 center, Vector2 *points, Vector2 *texcoords, int pointCount, Color tint);


int main(void)
{
   

    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Brainrot Sorting Visualizer");

    SetTargetFPS(60);
    double curTime = GetTime();

    ///////SETUP RANDOM VECTOR///////
    std::vector<int> v(screenHeight);
    std::iota(std::begin(v), std::end(v), 0);
    auto rng = std::default_random_engine{};
    std::shuffle(std::begin(v), std::end(v), rng);
    int n = v.size();
    ////////////////////////////////

    int min_val = screenHeight + 1;
    int index = 0;
    int cur = 0;

    bool selection_start = false;
    bool insertion_start = false;
    bool bubble_start = false;

    bool render = false;
    bool menu = true;
    bool done = false;

    // Brainrot
    Texture2D scarfy = LoadTexture("assets/scarfy.png");
    float frameWidth = (float)(scarfy.width / 6);
    int maxFrames = (int)(scarfy.width / (int)frameWidth);
    float timer = 0.0f;
    int frame = 0;

    InitAudioDevice();
    Music music = LoadMusicStream("assets/come.mp3");
    PlayMusicStream(music);
    float timePlayed = 0.0f;        // Time played normalized [0.0f..1.0f]
    bool pause = false;             // Music playing paused

    Music menuMusic = LoadMusicStream("assets/baka.mp3");
    PlayMusicStream(menuMusic);




    // Add x-axis movement variables
    float x_scarfy = 0.0f;      // Initial x position for scarfy
    float speed = 200.0f; // Movement speed in pixels per second


    



    //cat

      // Define texture coordinates to map our texture to poly
    Vector2 texcoords[MAX_POINTS] = {
        (Vector2){ 0.75f, 0.0f },
        (Vector2){ 0.25f, 0.0f },
        (Vector2){ 0.0f, 0.5f },
        (Vector2){ 0.0f, 0.75f },
        (Vector2){ 0.25f, 1.0f},
        (Vector2){ 0.375f, 0.875f},
        (Vector2){ 0.625f, 0.875f},
        (Vector2){ 0.75f, 1.0f},
        (Vector2){ 1.0f, 0.75f},
        (Vector2){ 1.0f, 0.5f},
        (Vector2){ 0.75f, 0.0f}  // Close the poly
    };

    // Define the base poly vertices from the UV's
    // NOTE: They can be specified in any other way
    float scale = 0.7f;
    Vector2 points[MAX_POINTS] = { 0 };
    for (int i = 0; i < MAX_POINTS; i++)
    {
        points[i].x = (texcoords[i].x - 0.5f)*256.0f *scale;
        points[i].y = (texcoords[i].y - 0.5f)*256.0f *scale;
    }
    
    // Define the vertices drawing position
    // NOTE: Initially same as points but updated every frame
    Vector2 positions[MAX_POINTS] = { 0 };
    for (int i = 0; i < MAX_POINTS; i++) positions[i] = points[i];

    // Load texture to be mapped to poly
    Texture texture = LoadTexture("assets/cat2.png");

    float angle = 0.0f;             // Rotation angle (in degrees)

    HideCursor(); 

    
    // std::cout<<GetClipboardText();

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        PauseMusicStream(music);

        UpdateMusicStream(menuMusic);
        PauseMusicStream(menuMusic);

         // Update points rotation with an angle transform
        // NOTE: Base points position are not modified
        angle++;
        for (int i = 0; i < MAX_POINTS; i++) positions[i] = Vector2Rotate(points[i], angle*DEG2RAD);
        
        BeginDrawing();
        ClearBackground(BLACK);
         DrawFPS(25, 750);
        /////////////
        /*MAIN MENU*/
        /////////////
        if (menu)
        {
            
            DrawText("Brainrot Sorting", 40, 200, 80, LIGHTGRAY);
            DrawText("Press Enter to Start", 270, 400, 20, LIGHTGRAY);
        }

        // KEY PRESS
        if (IsKeyPressed(KEY_V))
        {
            OpenURL("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
            SetMusicVolume(menuMusic, 0.0f);
            
        }


        if (IsKeyPressed(KEY_ONE))
        {
            selection_start = true;
            curTime = GetTime();
            render = false;
        }
        if (IsKeyPressed(KEY_TWO))
        {
            insertion_start = true;
            curTime = GetTime();
            render = false;
        }
        if (IsKeyPressed(KEY_THREE))
        {
            bubble_start = true;
            curTime = GetTime();
            render = false;
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            SetMusicVolume(menuMusic, 1.0f);

            done = false;
            render = true;
            auto rng = std::default_random_engine{};
            std::shuffle(std::begin(v), std::end(v), rng);
            menu = false;
            selection_start = false;
            insertion_start = false;
            bubble_start = false;
            min_val = screenHeight + 1;
            cur = 0;
        }

        //////////////////
        /*SHUFFLE MENU*/ //
        //////////////////
        if (render)
        {
            for (int i = 0; i < n; i++)
            {
                DrawRectangle(i, 0, 1, v[i], RED);
            }
            ResumeMusicStream(menuMusic);

            DrawText("Press Enter to shuffle", 200, 300, 30, RAYWHITE);
            DrawText("Press 1 for selection sort", 200, 340, 30, RAYWHITE);
            DrawText("Press 2 for insertion sort", 200, 380, 30, RAYWHITE);
            DrawText("Press 3 for bubble sort", 200, 420, 30, RAYWHITE);
            DrawText("Press V to see a cool trick", 200, 460, 30, RAYWHITE);
        }





        //////////////////
        /*SELECTION SORT*/
        //////////////////
        if ((GetTime() > curTime) && selection_start)
        {
            ResumeMusicStream(music);

            // selection sort
            if (cur < n)
            {
                for (int i = cur + 1; i < n; i++)
                {
                    if (v[i] < min_val)
                    {
                        index = i;
                        min_val = v[i];
                    }
                }
                v[index] = v[cur];
                v[cur] = min_val;
                cur++;
                min_val = screenHeight + 1;
            }
            else
            {
                done = true;
            }
            // render
            for (int i = 0; i < n; i++)
            {
                DrawRectangle(i, 0, 1, v[i], RED);
            }
        }
        //////////////////
        /*INSERTION SORT*/
        //////////////////
        if ((GetTime() > curTime) && insertion_start)
        {
            // insertion sort
            if (cur < n)
            {
                int key, j;
                key = v[cur];
                j = cur - 1;
                while (j >= 0 && v[j] > key)
                {
                    v[j + 1] = v[j];
                    j = j - 1;
                }
                v[j + 1] = key;
                cur++;
            }
            else
            {
                done = true;
            }

            // render
            for (int i = 0; i < n; i++)
            {
                DrawRectangle(i, 0, 1, v[i], GREEN);
            }
        }
        ///////////////
        /*BUBBLE SORT*/
        ///////////////
        if ((GetTime() > curTime) && bubble_start)
        {
            // bubble sort
            if (cur < n)
            {
                for (int j = 0; j < n - cur - 1; j++)
                {
                    if (v[j] > v[j + 1])
                    {
                        swap(&v[j], &v[j + 1]);
                    }
                }
                cur++;
            }
            else
            {
                done = true;
            }

            // render
            for (int i = 0; i < n; i++)
            {
                DrawRectangle(i, 0, 1, v[i], BLUE);
            }
        }

        if (done)
        {
            for (int i = 0; i < n; i++)
            {
                DrawRectangle(i, 0, 1, v[i], PURPLE);
            }
            DrawText("Dew it again!", 50, 400, 20, GREEN);
        }

        // Brainrot
        timer += GetFrameTime();

        frame = frame % maxFrames;

        if (timer >= 0.2f)
        {
            timer = 0.0f;
            frame += 1;
        }

        // Update x-axis position for scarfy
        x_scarfy += speed * GetFrameTime(); // Update position
        if (x_scarfy > 820)
        {
            x_scarfy = -100; // Reset to 0 if it goes over 800px
        }

        // Draw the texture with updated x position
        DrawTextureRec(
            scarfy,
            Rectangle{
                frameWidth * frame,
                0,
                frameWidth,
                (float)scarfy.height},
            Vector2{x_scarfy, 650}, // Use x for horizontal position
            RAYWHITE);


                // ClearBackground(RAYWHITE);

            DrawText("Made* by Yun", 20, 20, 20, DARKGRAY);

            DrawTexturePoly(texture, (Vector2){ 650, 70 },
                            positions, texcoords, MAX_POINTS, RAYWHITE);



        EndDrawing();
    }


    


    UnloadTexture(texture);
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}


void DrawTexturePoly(Texture2D texture, Vector2 center, Vector2 *points, Vector2 *texcoords, int pointCount, Color tint)
{
    rlBegin(RL_TRIANGLES);
    
    rlSetTexture(texture.id);

        rlColor4ub(tint.r, tint.g, tint.b, tint.a);

        for (int i = 0; i < pointCount - 1; i++)
        {
            rlTexCoord2f(0.5f, 0.5f);
            rlVertex2f(center.x, center.y);

            rlTexCoord2f(texcoords[i].x, texcoords[i].y);
            rlVertex2f(points[i].x + center.x, points[i].y + center.y);

            rlTexCoord2f(texcoords[i + 1].x, texcoords[i + 1].y);
            rlVertex2f(points[i + 1].x + center.x, points[i + 1].y + center.y);
        }
    rlEnd();

    rlSetTexture(0);
}