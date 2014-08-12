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
  int sleep_time = 3;
  LEDDisplay* leddisp = new LEDDisplay();

  if (argc > 1) {
    demo = atoi(argv[1]);
    if (argc > 2) {
      sleep_time = atoi(argv[2]);
      if (sleep_time <= 0) {
        sleep_time = 3;
      }
    }
  }
  fprintf(stderr, "Using demo %d\n", demo);

  switch (demo) {
  case 0:
    while (1) {
      unsigned timer_ms;
      leddisp->SetRGB(0, 0xff, 0xff);

      printf("Left scroll\n");
      leddisp->DisplayString("Shawn & Anne & ", LEDDisplay::kLeft, LEDDisplay::kLeftScroll, LEDDisplay::kVariable);
      sleep(sleep_time);

      printf("Up scroll\n");
      leddisp->DisplayString("Shawn & Anne & ", LEDDisplay::kLeft, LEDDisplay::kUpScroll, LEDDisplay::kVariable);
      sleep(sleep_time);

      printf("Timer\n");
      leddisp->SetRGB(0, 0, 0xff);
      timer_ms = 825000;
      while (timer_ms < 830000) {
        leddisp->DisplayTimer(timer_ms);
        timer_ms += 100;
        usleep(100000);
      }

      printf("Pictograms\n");
      leddisp->SetRGB(0xff, 0, 0);
      leddisp->DisplayX(LEDDisplay::kDupeCenter, LEDDisplay::kDownScroll);
      sleep(2);
      leddisp->SetRGB(0, 0xff, 0);
      leddisp->DisplayCheckmark(LEDDisplay::kDupeCenter, LEDDisplay::kUpScroll);
      sleep(2);
      leddisp->SetRGB(0xff, 0, 0);
      leddisp->DisplayHeart(LEDDisplay::kDupeCenter, LEDDisplay::kRightScroll);
      sleep(2);
      leddisp->DisplayHeart(LEDDisplay::kDupeCenter, LEDDisplay::kLeftScroll);
      sleep(2);
      leddisp->DisplayX(LEDDisplay::kDupeRight, LEDDisplay::kNoScroll);
      sleep(2);
      leddisp->DisplayX(LEDDisplay::kLeft, LEDDisplay::kNoScroll);
      sleep(2);
      leddisp->DisplayX(LEDDisplay::kCenter, LEDDisplay::kNoScroll);
      sleep(2);
      leddisp->SetRGB(0, 0xff, 0xff);

      printf("Display fill\n");
      leddisp->SetRGB(0xff, 0, 0);
      leddisp->DisplayFill();
      sleep(1);
      leddisp->SetRGB(0, 0xff, 0);
      leddisp->DisplayFill();
      sleep(1);
      leddisp->SetRGB(0, 0, 0xff);
      leddisp->DisplayFill();
      sleep(1);
      leddisp->SetRGB(0xff, 0xff, 0);
      leddisp->DisplayFill();
      sleep(1);
      leddisp->SetRGB(0xff, 0, 0xff);
      leddisp->DisplayFill();
      sleep(1);
      leddisp->SetRGB(0, 0xff, 0xff);
      leddisp->DisplayFill();
      sleep(1);
    }
    break;

  case 1:
    delete leddisp;
    return 0;

  default:
    printf("Unknown demo: %d\n", demo);
    break;
  }

  return 0;
}
