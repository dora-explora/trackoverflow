#include <stdio.h>
#include <stdint.h>
#include "../miniaudio/miniaudio.h"

ma_result audio_init(ma_engine *engine, ma_sound soundsA[16], ma_sound soundsB[16]) {
    ma_result result;

    result = ma_engine_init(NULL, engine);
    for (int i = 0; i < 16; i++) {
        char filepath[19];
        sprintf(filepath, "assets/break%d.wav", i); //
        ma_sound_init_from_file(engine, filepath, 0, NULL, NULL, &soundsA[i]); // loads the audio file into sounds
        ma_sound_init_from_file(engine, filepath, 0, NULL, NULL, &soundsB[i]); // same here, duplicated for double buffering
    }

    return result;
}

void load_beat(ma_engine *engine, int *beats, int *current_beat, int *ab, ma_sound soundsA[16], ma_sound soundsB[16], int beat_frames, uint64_t data) {
    if (ma_engine_get_time_in_pcm_frames(engine) < (*beats - 1) * beat_frames) { return; }
    int s = (data >> *current_beat * 4) & 0b1111; // the 4 bits that select which of the 16 beats to play
    if (*ab == 1) {
        ma_sound_set_start_time_in_pcm_frames(&soundsA[s], *beats * beat_frames);
        ma_sound_start(&soundsA[s]);
        *ab = 0;
    } else {
        ma_sound_set_start_time_in_pcm_frames(&soundsB[s], *beats * beat_frames);
        ma_sound_start(&soundsB[s]);
        *ab = 1;
    }
    (*beats)++;
    (*current_beat)++;
}
