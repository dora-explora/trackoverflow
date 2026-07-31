#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "../miniaudio/miniaudio.h"

int main(int argc, char** argv) {
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
    ma_sound_start(&soundsA[0]);

    uint32_t beats = 1; // how many beats have been set to start
    uint64_t data; // variable to store each word we read
    uint64_t* start = &data + 0x1140; // pointer to the data variable, which is near the end of the stack, plus 0xA00 because the data is boring before that
    for (uint64_t i = 0; 1; data = *(start + i++)) { // data is accessed in 64 bit chunks, taking from i words after the start
        if (data == 0) { continue; } // if the word is all zeroes we dont care so skip it
        printf("0x%04lX 0x%016lX\n", i, data); // print index and data, 2 and 8 bytes of hex respectively

        #ifdef __x86_64__
            #ifdef __linux__
                if (data == 0x00776F6C66726576) { break; } // this sequence always comes up right before a crash on my machine, so break
            #endif
            #ifdef _WIN32
                if (data == 0x0000000000006B6E) { break; } // same thing but windows (wine)
            #endif
        #endif

        while (data != 0) { // exits when data has been fully exhausted or the rest is zeroes
            // if a beat doesn't need to be loaded, then sleep for the length of a beat
            // "beats - 1" so that the next beat is loaded before this one is finished
            if (ma_engine_get_time_in_pcm_frames(&engine) < (beats * beat_frames - beat_frames / 2)) {
                nanosleep(&beat_time, &rem_time); // sleeps for 1 beat
            } else {
                int s = data & 0b1111; // the 4 bits that select which of the 16 beats to play
                if (ab == 1) {
                    ma_sound_set_start_time_in_pcm_frames(&soundsA[s], beats * beat_frames);
                    ma_sound_start(&soundsA[s]);
                    ab = 0;
                } else {
                    ma_sound_set_start_time_in_pcm_frames(&soundsB[s], beats * beat_frames);
                    ma_sound_start(&soundsB[s]);
                    ab = 1;
                }
                data >>= 4;
                beats++;
            }
        }
    }

    ma_engine_uninit(&engine);
    return 0;
}
