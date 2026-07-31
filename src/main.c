#include <stdio.h>
#include <stdint.h>
#include "../miniaudio/miniaudio.h"

int main(int argc, char** argv) {
    ma_result result;
    ma_engine engine;

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        puts("failed to initialize audio engine");
        return -1;
    }

    ma_sound beats[16]; // array of sounds holding each of the 16 beats
    uint32_t beat_frames = ma_engine_get_sample_rate(&engine) * 60 / 340; // sample rate divided by tempo (times 2) gives number of frames between each 1/8 note
    for (int i = 0; i < 16; i++) {
        char filepath[18];
        sprintf(filepath, "assets/break%d.wav", i); //
        ma_sound_init_from_file(&engine, filepath, 0, NULL, NULL, &beats[i]); // loads the audio file into the sound...
        ma_sound_set_start_time_in_pcm_frames(&beats[i], beat_frames * i);    // sets that sounds future start time...
        ma_sound_start(&beats[i]);                                            // and sets it to begin playing at that time
    }

    ma_engine_uninit(&engine);
    return 0;

    uint64_t data; // variable to store each word we read but also to find the stack
    uint64_t* start = &data; // pointer to the data variable, which is near the start of the stack
    for (uint64_t i = 0; 1; data = *(start + i++)) {
        // data is accessed in 64 bit chunks, taking from i words after the start
        if (data == 0) { continue; } // if the word is all zeroes we dont care so skip it
        printf("0x%04lX 0x%016lX\n", i, data); // print index and data, 2 and 8 bytes of hex respectively

        #ifdef __x86_64__
            #ifdef __linux__
                if (data == 0x00776f6c66726576) { break; } // this sequence always comes up right before a crash on my machine, so break
            #endif
            #ifdef _WIN32
                if (data == 0x0000000000006B6E) { break; } // same thing but windows (wine)
            #endif
        #endif
    }

    return 0;
}
