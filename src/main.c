#include <stdint.h>
#include <stdio.h>
#include "../miniaudio/miniaudio.h"
#include <raylib.h>
#include "audio.c"
#include "display.c"

int main(int argc, char** argv) {
    // miniaudio engine init and sound loading
    ma_result result;
    ma_engine engine;
    ma_sound soundsA[16]; // array of sounds holding each of the 16 beats
    ma_sound soundsB[16]; // same thing but copied for double buffering

    result = audio_init(&engine, soundsA, soundsB);
    if (result != MA_SUCCESS) {
        puts("failed to initialize audio engine");
        return -1;
    }

    Font font = display_init();

    ma_sound_start(&soundsA[0]);

    int ab = 0; // denotes which of the two is currently in use, starting value is A
    uint32_t sample_rate = ma_engine_get_sample_rate(&engine); // sample rate of miniaudio's default device
    int beat_frames = sample_rate * 60 / 340; // number of PCM frames between each 1/8 note
    int beats = 1; // how many beats have been set to start
    int current_beat = 1; // what beat we are currently on, goes up to 15
    uint64_t data; // stores each word we read
    uint64_t data_shown; // stores the value of data shown on the screen, since data switches before the bar does
    uint64_t* cursor = &data + 0x1180; // pointer to the data variable, which is near the end of the stack, plus 0x1180 because the data is boring before that
    uint64_t cursor_shown; // same thing but cursor
    data = *cursor++;
    while (!WindowShouldClose()) { // main loop provided by raylib
        if (data == 0) { data = *cursor++; continue; } // if the word is all zeroes we dont care so load another

        if (current_beat == 16) { // if data is exhausted...
            data = *cursor++; // load another word
            current_beat = 0;
            continue;
        }

        load_beat(&engine, &beats, &current_beat, &ab, soundsA, soundsB, beat_frames, data);
        if (current_beat == 2) { data_shown = data; cursor_shown = (uint64_t) cursor; }

        int beat_shown = (current_beat + 14) % 16;
        float since_last_beat = (float) (ma_engine_get_time_in_pcm_frames(&engine) % beat_frames) / beat_frames;
        BeginDrawing();
            draw(beat_shown, data_shown, cursor_shown, font, since_last_beat);
        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    ma_engine_uninit(&engine);
    return 0;
}
