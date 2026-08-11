#include <stdint.h>
#include <stdio.h>
#include "../miniaudio/miniaudio.h"
#include <raylib.h>

Font display_init() {
    InitWindow(1280, 720, "track overflow");
    SetTargetFPS(30);
    return LoadFont("assets/PixelOperatorMono8.ttf");
}

void DrawCircleStroke(int centerX, int centerY, float outerRadius, float strokeSize, Color bgColor) {
    DrawCircle(centerX, centerY, outerRadius, BLACK);
    DrawCircle(centerX, centerY, outerRadius - strokeSize, bgColor);
}

const int beat_colors[16][4] = { // intensity of kick, snare, ride, and crash from 0-3 in that order for each beat
//   k, s, r, c
    {3, 0, 1, 0}, // beat 0
    {0, 0, 3, 0}, // beat 1
    {0, 3, 2, 0}, // beat 2
    {0, 1, 3, 0}, // beat 3
    {0, 1, 3, 0}, // beat 4
    {0, 3, 2, 0}, // beat 5
    {0, 1, 3, 0}, // beat 6
    {0, 1, 3, 0}, // beat 7
    {3, 0, 1, 0}, // beat 8
    {0, 0, 3, 0}, // beat 9
    {0, 3, 2, 0}, // beat A
    {0, 1, 3, 0}, // beat B
    {2, 0, 2, 0}, // beat C
    {3, 0, 0, 3}, // beat D
    {0, 0, 3, 2}, // beat E
    {0, 2, 2, 1}, // beat F
};

void draw(int beat_shown, uint64_t data_shown, uint64_t cursor_shown, Font font, float since_last_beat) {
    ClearBackground(RAYWHITE);

    int s = (data_shown >> beat_shown * 4) & 0b1111;

    int si = since_last_beat * 255; // aka since_intensity
    int rect_pos = s * 80;
    DrawRectangleGradientV(rect_pos, 0, 80, 720, (Color) {si, si, si, 128}, (Color) {si/2, si/2, si/2, 128});
    char beat_hex[2];
    sprintf(beat_hex, "%X", s);
    DrawText(beat_hex, rect_pos + 20, 10, 80, (Color) {255, 255, 255, si});

    int box_pos = 1072 - beat_shown * 60;
    // DrawRectangle(box_pos, 230, 48, 60, WHITE);
    DrawRectangleLines(box_pos, 230, 48, 60, BLACK);

    char data_str[17];
    sprintf(data_str, "%016lX", data_shown);
    DrawTextEx(font, data_str, (Vector2) { 169, 232 }, 64, -4, BLACK);
    DrawText("0x", 90, 237, 64, LIGHTGRAY);

    char cursor_str[32];
    sprintf(cursor_str, "Data at address 0x%012lX:", cursor_shown);
    DrawText(cursor_str, 290, 180, 40, GRAY);

    int triangle_pos = 1096 - beat_shown * 60;
    DrawTriangle(
        (Vector2) {triangle_pos, 300},
        (Vector2) {triangle_pos - 15, 330},
        (Vector2) {triangle_pos + 15, 330},
        GRAY
    );

    Color colors[4];
    for (int i = 0; i < 4; i++) {
        int value = 255 - 64 * beat_colors[s][i] * (1 - 0.2*since_last_beat);
        colors[i] = (Color) {value, value, value, 255};
    }

    DrawRectanglePro((Rectangle) {1090, 460, 100, 60}, (Vector2) {0, 0}, 20, BLACK); // draw kick
    DrawRectanglePro((Rectangle) {1093.4, 466.5, 90, 50}, (Vector2) {0, 0}, 20, colors[0]); // draw kick infill
    DrawCircleStroke(1030, 545, 35, 4.5, colors[1]); // draw snare
    DrawCircleStroke(1200, 530, 45, 4, colors[2]); // draw ride
    DrawCircleStroke(1050, 470, 50, 4, colors[3]); // draw crash
    DrawCircleStroke(970, 530, 40, 3, RAYWHITE); // draw HH
    DrawCircleStroke(1060, 625, 30, 2, (Color) {160, 32, 32, 255}); // draw seat
}
