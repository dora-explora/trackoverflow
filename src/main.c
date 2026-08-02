#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "../miniaudio/miniaudio.h"
#include <raylib.h>

int main(int argc, char** argv) {
    // miniaudio engine init and sound loading
    ma_result result;
    ma_engine engine;

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        puts("failed to initialize audio engine");
        return -1;
    }

    ma_sound soundsA[16]; // array of sounds holding each of the 16 beats
    ma_sound soundsB[16]; // same thing but copied for double buffering
    int ab = 0; // denotes which of the two is currently in use, starting value is A
    uint32_t sample_rate = ma_engine_get_sample_rate(&engine);
    uint64_t beat_frames = sample_rate * 60 / 340; // sample rate divided by tempo (times 2) gives number of frames between each 1/8 note
    struct timespec beat_time;
    struct timespec rem_time; // stores the remainder of the sleep left after an interruption, purposefully unused
    beat_time.tv_sec = 0;
    beat_time.tv_nsec = 1000000000 * beat_frames / sample_rate;
    for (int i = 0; i < 16; i++) {
        char filepath[18];
        sprintf(filepath, "assets/break%d.wav", i); //
        ma_sound_init_from_file(&engine, filepath, 0, NULL, NULL, &soundsA[i]); // loads the audio file into the sound
        ma_sound_init_from_file(&engine, filepath, 0, NULL, NULL, &soundsB[i]);
    }

    // raylib init
    const int width = 1280;
    const int height = 720;
    InitWindow(width, height, "track overflow");
    SetTargetFPS(17);
    Font font = LoadFont("assets/PixelOperatorMono8.ttf");

    ma_sound_start(&soundsA[0]);

    uint32_t beats = 1; // how many beats have been set to start
    int current_beat = 1; // what beat we are currently on, goes up to 15
    uint64_t data; // stores each word we read
    uint64_t data_shown; // stores the value of data shown on the screen, since data switches before the bar does
    uint64_t* cursor = &data + 0x1180; // pointer to the data variable, which is near the end of the stack, plus 0x1140 because the data is boring before that
    uint64_t* cursor_shown; // same thing but cursor
    data = *cursor++;
    while (!WindowShouldClose()) { // main loop provided by raylib
        if (data == 0) { data = *cursor++; continue; } // if the word is all zeroes we dont care so load another

        // ive determined its funnier if it crashes
        // #ifdef __x86_64__
            // #ifdef __linux__
                // if (data == 0x00776F6C66726576) { break; } // this sequence always comes up right before a crash on my machine, so break
            // #endif
            // #ifdef _WIN32
                // if (data == 0x0000000000006B6E) { break; } // same thing but windows (wine)
            // #endif
        // #endif

        if (current_beat == 16) { // if data is exhausted...
            data = *cursor++; // load another word
            current_beat = 0;
            continue;
        }

        // "beats - 1" so that the next beat is loaded before this one is finished
        if (ma_engine_get_time_in_pcm_frames(&engine) > (beats - 1) * beat_frames) {
            int s = (data >> current_beat * 4) & 0b1111; // the 4 bits that select which of the 16 beats to play
            if (ab == 1) {
                ma_sound_set_start_time_in_pcm_frames(&soundsA[s], beats * beat_frames);
                ma_sound_start(&soundsA[s]);
                ab = 0;
            } else {
                ma_sound_set_start_time_in_pcm_frames(&soundsB[s], beats * beat_frames);
                ma_sound_start(&soundsB[s]);
                ab = 1;
            }
            beats++;
            current_beat++;
            if (current_beat == 2) { data_shown = data; cursor_shown = cursor; }
        }

        BeginDrawing(); {

            ClearBackground(RAYWHITE);

            int box_pos = 1072 - (((current_beat + 14) % 16)) * 60;
            // DrawRectangle(box_pos, 230, 48, 60, WHITE);
            DrawRectangleLines(box_pos, 230, 48, 60, BLACK);

            char data_str[17];
            sprintf(data_str, "%016lX", data_shown);
            DrawTextEx(font, data_str, (Vector2) { 169, 232 }, 64, -4, BLACK);
            DrawText("0x", 90, 237, 64, LIGHTGRAY);

            char cursor_str[32];
            sprintf(cursor_str, "Data at address 0x%012lX:", (uint64_t) cursor_shown);
            DrawText(cursor_str, 290, 180, 40, GRAY);

            int triangle_pos = 1096 - (((current_beat + 14) % 16)) * 60;
            DrawTriangle(
                (Vector2) {triangle_pos, 300},
                (Vector2) {triangle_pos - 15, 330},
                (Vector2) {triangle_pos + 15, 330},
                GRAY
            );

        } EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    ma_engine_uninit(&engine);
    return 0;
}
