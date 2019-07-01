#include <cstdio>

#include "Arduino.h"
#include "genesynth.ino"

int main(int argc, char const *argv[]) {
    printf("Running setup.\n");
    setup();
    printf("Starting loop.\n");
    loop();
    return 0;
}
