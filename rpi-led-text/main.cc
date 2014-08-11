#include "thread.h"
#include "led-matrix.h"
#include "led-display.h"

#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <time.h>

using std::min;
using std::max;

int main(int argc, char *argv[]) {
  int demo = 0;
  LEDDisplay* leddisp = new LEDDisplay();

  if (argc > 1) {
    demo = atoi(argv[1]);
  }
  fprintf(stderr, "Using demo %d\n", demo);

  switch (demo) {
  case 0:
    leddisp->DisplayString("DSA", LEDDisplay::kLeft, LEDDisplay::kNoScroll, LEDDisplay::kVariable);
    break;

  default:
    printf("Unknown demo: %d\n", demo);
    break;
  }

  // Things are set up. Just wait for <RETURN> to be pressed.
  printf("Press <RETURN> to exit and reset LEDs\n");
  getchar();

  // Stopping threads and wait for them to join.
  delete leddisp;

  return 0;
}
