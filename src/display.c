#include <stdint.h>
#include <stdio.h>
#include <raylib.h>

Font display_init() {
    InitWindow(1280, 720, "track overflow");
    SetTargetFPS(17);
    return LoadFont("assets/PixelOperatorMono8.ttf");
}

void draw(int current_beat, uint64_t data_shown, uint64_t cursor_shown, Font font) {
    ClearBackground(RAYWHITE);

    int box_pos = 1072 - (((current_beat + 14) % 16)) * 60;
    // DrawRectangle(box_pos, 230, 48, 60, WHITE);
    DrawRectangleLines(box_pos, 230, 48, 60, BLACK);

    char data_str[17];
    sprintf(data_str, "%016lX", data_shown);
    DrawTextEx(font, data_str, (Vector2) { 169, 232 }, 64, -4, BLACK);
    DrawText("0x", 90, 237, 64, LIGHTGRAY);

    char cursor_str[32];
    sprintf(cursor_str, "Data at address 0x%012lX:", cursor_shown);
    DrawText(cursor_str, 290, 180, 40, GRAY);

    int triangle_pos = 1096 - (((current_beat + 14) % 16)) * 60;
    DrawTriangle(
        (Vector2) {triangle_pos, 300},
        (Vector2) {triangle_pos - 15, 330},
        (Vector2) {triangle_pos + 15, 330},
        GRAY
    );
}
