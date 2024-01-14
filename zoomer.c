#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define TARGET_FPS 120
#define MAX_ZOOM 7.0f
#define MIN_ZOOM 1.0f
#define SCROLL_SMOTHING_FACTOR 0.10f
#define ZOOMER_SCREENSHOT_CMD_ENV "ZOOMER_SCREENSHOT_CMD"
#define SCREENSHOT_FILE_NAME "temp_screenshoot.png"
#define CIRCLE_SHADER_KEY KEY_C

typedef struct Zoomer {
    Shader shader;
    int mousePos;
    int screenSize;
    int circleSizeModifier;
    bool enabled;
} Zoomer;

Zoomer load_zoomer_shader() {
    Shader shader = LoadShader(0, "zoomer.fs");
    int mousePos = GetShaderLocation(shader, "mousePos");
    int screenSize = GetShaderLocation(shader, "screenSize");
    int circleSizeModifier = GetShaderLocation(shader, "circleSizeModifier");
    bool enabled = true;

    Zoomer zoomer = {shader, mousePos, screenSize, circleSizeModifier, enabled};

    return zoomer;
}

int main(void) {

    const char* SCREENSHOT_CMD_ENV = getenv(ZOOMER_SCREENSHOT_CMD_ENV);
    if (SCREENSHOT_CMD_ENV == NULL) {
        printf("zoomer: Please set %s environment variable\n",
               ZOOMER_SCREENSHOT_CMD_ENV);
        return 1;
    }

    int buff_size = 256;
    char cmd[buff_size];
    int char_count = snprintf(cmd, buff_size, "%s %s", SCREENSHOT_CMD_ENV,
                              SCREENSHOT_FILE_NAME);
    if (char_count < 0 || char_count > buff_size) {
        printf("zoomer: Error, could not process provided screnshot cmd\n");
        return 1;
    }

    printf("INFO: ZOOMER_SCREENSHOT_CMD_ENV: %s\n", cmd);
    int rc = system(cmd);
    if (rc != 0) {
        printf("zoomer: Error %s:%i\n", cmd, rc);
    }

    Image img = LoadImage(SCREENSHOT_FILE_NAME);
    if (remove(SCREENSHOT_FILE_NAME) != 0) {
        printf("zoomer: Unable to delete the file\n");
        return 1;
    }

    InitWindow(0, 0, "Zoomer");
    Texture2D texture = LoadTextureFromImage(img);
    UnloadImage(img);

    ToggleFullscreen();
    SetTargetFPS(TARGET_FPS);

    Camera2D camera = {0};
    camera.zoom = 1.0f;

    Vector2 screen_size = {GetRenderWidth(), GetRenderHeight()};

    Zoomer zoomer = load_zoomer_shader();
    bool modify_circle_size = false;
    float circle_size_modifier = 1.0f;
    SetShaderValue(zoomer.shader, zoomer.screenSize, &screen_size,
                   SHADER_UNIFORM_VEC2);
    SetShaderValue(zoomer.shader, zoomer.circleSizeModifier,
                   &circle_size_modifier, SHADER_UNIFORM_FLOAT);

    texture.width = screen_size.x;
    texture.height = screen_size.y;

    while (!WindowShouldClose()) {

        if (IsKeyPressed(CIRCLE_SHADER_KEY)) {
            zoomer.enabled = !zoomer.enabled;
        }

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
            bool enabled = zoomer.enabled;
            UnloadShader(zoomer.shader);
            zoomer = load_zoomer_shader();
            zoomer.enabled = enabled;
            SetShaderValue(zoomer.shader, zoomer.screenSize, &screen_size,
                           SHADER_UNIFORM_VEC2);
            SetShaderValue(zoomer.shader, zoomer.circleSizeModifier,
                           &circle_size_modifier, SHADER_UNIFORM_FLOAT);
        }

        if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)) {
            modify_circle_size = true;
        } else if (IsKeyReleased(KEY_LEFT_SHIFT) ||
                   IsKeyReleased(KEY_RIGHT_SHIFT)) {
            modify_circle_size = false;
        }

        if (modify_circle_size) {
            circle_size_modifier +=
                ((float)GetMouseWheelMove() * SCROLL_SMOTHING_FACTOR);
            if (circle_size_modifier > 2.0f)
                circle_size_modifier = 2.0f;
            else if (circle_size_modifier < 0.2f)
                circle_size_modifier = 0.2f;
        } else {
            camera.zoom +=
                ((float)GetMouseWheelMove() * SCROLL_SMOTHING_FACTOR);
            if (camera.zoom > MAX_ZOOM)
                camera.zoom = MAX_ZOOM;
            else if (camera.zoom < MIN_ZOOM)
                camera.zoom = MIN_ZOOM;
        }

        Vector2 mouse_pos = {(float)GetMouseX(), (float)GetMouseY()};
        camera.target = (Vector2){mouse_pos.x, mouse_pos.y};
        camera.offset = (Vector2){mouse_pos.x, mouse_pos.y};

        SetShaderValue(zoomer.shader, zoomer.mousePos, &mouse_pos,
                       SHADER_UNIFORM_VEC2);
        SetShaderValue(zoomer.shader, zoomer.circleSizeModifier,
                       &circle_size_modifier, SHADER_UNIFORM_FLOAT);

        BeginDrawing();
        {
            ClearBackground(WHITE);
            BeginMode2D(camera);
            if (zoomer.enabled) {
                BeginShaderMode(zoomer.shader);
                { DrawTexture(texture, 0, 0, WHITE); }
                EndShaderMode();
            } else {
                { DrawTexture(texture, 0, 0, WHITE); }
            }
            EndMode2D();

            DrawFPS(0, 0);
        }
        EndDrawing();
    }

    UnloadShader(zoomer.shader);

    CloseWindow();

    return 0;
}
