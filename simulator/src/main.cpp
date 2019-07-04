#include <cstdio>

#include "genesynth.ino"
#include "simulator.h"

int main(int argc, char const *argv[]) {
  if (!simulator::initialize()) {
    printf("Failed to initialize simulator, exiting.\n");
    simulator::teardown();
    return 1;
  }

  printf("Running Arduino setup.\n");
  setup();

  printf("Entering Arduino loop.\n");
  simulator::loop(&loop);

  simulator::teardown();
  return 0;
}
