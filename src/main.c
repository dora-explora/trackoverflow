#include <stdio.h>
#include <stdint.h>

int main(int argc, char** argv) {
    uint64_t data; // variable to store each word we read but also to find the stack
    uint64_t* start = &data; // pointer to the data variable, which is near the start of the stack
    for (uint64_t i = 0; 1; data = *(start + i++)) { // i = -80 because data is interesting for ~80 bytes before &data
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
