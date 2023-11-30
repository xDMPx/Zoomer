#include "raylib.h"
#include <stdio.h>

#define TARGET_FPS 120
#define MAX_ZOOM 7.0f
#define MIN_ZOOM 1.0f
#define SCROLL_SMOTHING_FACTOR 0.10f

typedef struct Zoomer {
    Shader shader;
    int mousePos;
    int screenSize;
} Zoomer;

Zoomer load_zoomer_shader() {
    Shader shader = LoadShader(0, "zoomer.fs");
    int mousePos = GetShaderLocation(shader, "mousePos");
    int screenSize = GetShaderLocation(shader, "screenSize");

    Zoomer zoomer = {shader, mousePos, screenSize};

    return zoomer;
}

int main(void) {

    InitWindow(0, 0, "Zoomer");
    ToggleFullscreen();

    Image img = LoadImage("test.png");
    Texture2D texture = LoadTextureFromImage(img);
    UnloadImage(img);

    SetTargetFPS(TARGET_FPS);

    Camera2D camera = {0};
    camera.zoom = 1.0f;

    Vector2 screen_size = {GetRenderWidth(), GetRenderHeight()};

    Zoomer zoomer = load_zoomer_shader();
    SetShaderValue(zoomer.shader, zoomer.screenSize, &screen_size,
                   SHADER_UNIFORM_VEC2);

    texture.width = screen_size.x;
    texture.height = screen_size.y;

    while (!WindowShouldClose()) {
        int screenWidth = GetRenderWidth();
        int screenHeight = GetRenderHeight();

        if (screen_size.x != screenWidth || screen_size.y != screenHeight) {
            printf("INFO: Screen: %ix%i\n", screenWidth, screenHeight);
            screen_size = (Vector2){screenWidth, screenHeight};
            SetShaderValue(zoomer.shader, zoomer.screenSize, &screen_size,
                           SHADER_UNIFORM_VEC2);
            texture.width = screenWidth;
            texture.height = screenHeight;
        }

        // Shader reloading
        if (IsKeyPressed(KEY_R)) {
            UnloadShader(zoomer.shader);
            zoomer = load_zoomer_shader();
            SetShaderValue(zoomer.shader, zoomer.screenSize, &screen_size,
                           SHADER_UNIFORM_VEC2);
        }

        camera.zoom += ((float)GetMouseWheelMove() * SCROLL_SMOTHING_FACTOR);
        if (camera.zoom > MAX_ZOOM)
            camera.zoom = MAX_ZOOM;
        else if (camera.zoom < MIN_ZOOM)
            camera.zoom = MIN_ZOOM;

        Vector2 mouse_pos = {(float)GetMouseX(), (float)GetMouseY()};
        camera.target = (Vector2){mouse_pos.x, mouse_pos.y};
        camera.offset = (Vector2){mouse_pos.x, mouse_pos.y};

        SetShaderValue(zoomer.shader, zoomer.mousePos, &mouse_pos,
                       SHADER_UNIFORM_VEC2);

        BeginDrawing();
        {

            ClearBackground(WHITE);
            BeginMode2D(camera);
            BeginShaderMode(zoomer.shader);
            { DrawTexture(texture, 0, 0, WHITE); }
            EndShaderMode();
            EndMode2D();

            DrawFPS(0, 0);
        }
        EndDrawing();
    }

    UnloadShader(zoomer.shader);

    CloseWindow();

    return 0;
}
