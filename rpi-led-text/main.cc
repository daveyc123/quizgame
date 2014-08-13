#include "thread.h"
#include "led-matrix.h"
#include "led-display.h"
#include "text.h"

#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <time.h>

using std::min;
using std::max;

void leddisplay_demo(LEDDisplay* leddisp)
{
  while (1) {
    unsigned timer_ms;
    leddisp->SetRGB(0, 0xff, 0xff);

    printf("Left scroll\n");
    leddisp->DisplayString("Shawn & Anne & ", LEDDisplay::kLeft, LEDDisplay::kLeftScroll, LEDDisplay::kVariable);
    sleep(3);

    printf("Up scroll\n");
    leddisp->DisplayString("Shawn & Anne & ", LEDDisplay::kLeft, LEDDisplay::kUpScroll, LEDDisplay::kVariable);
    sleep(3);

    leddisp->SetRGB(0xff, 0, 0);
    printf("Left\n");
    leddisp->DisplayString("L", LEDDisplay::kLeft, LEDDisplay::kNoScroll, LEDDisplay::kVariable);
    sleep(1);

    printf("Right\n");
    leddisp->DisplayString("R", LEDDisplay::kRight, LEDDisplay::kNoScroll, LEDDisplay::kVariable);
    sleep(1);

    printf("Center\n");
    leddisp->DisplayString("C", LEDDisplay::kCenter, LEDDisplay::kNoScroll, LEDDisplay::kVariable);
    sleep(1);

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
    leddisp->SetRGB(0, 0xff, 0);
    leddisp->DisplayCheckmark(LEDDisplay::kLeft, LEDDisplay::kRightScroll);
    sleep(2);

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

    printf("Display clear\n");
    leddisp->DisplayClear();
    sleep(1);
  }
}

void canvas_invert_demo(LEDDisplay* leddisp)
{
  const char* str = "Winning!";
  unsigned str_width;
  Font font("fonts/Oswald-Bold.ttf", 15);
  RGBCanvas* canvases[2];
  Pen pen;
  unsigned char r = 0;
  unsigned char g = 0xff;
  unsigned char b = 0;
  int i = 0;

  str_width = font.GetWidth(str);
  pen.x = leddisp->GetWidth() / 2 - str_width / 2;
  pen.y = leddisp->GetHeight() - 2;
  canvases[0] = new RGBCanvas(leddisp->GetWidth(), leddisp->GetHeight());
  canvases[1] = new RGBCanvas(leddisp->GetWidth(), leddisp->GetHeight());
  font.PaintString(str, canvases[0], &pen, r, g, b);
  canvases[1]->Fill(r, g, b);
  font.PaintString(str, canvases[1], &pen, 0, 0, 0);

  while (1) {
    leddisp->SwapCanvas(canvases[i++ % 2], LEDDisplay::kLeft, LEDDisplay::kNoScroll, false);
    usleep(200000);
  }
}

void canvas_picto_demo(LEDDisplay* leddisp)
{
  Font pictofont("fonts/modernpics.otf", 16);
  Font font("fonts/Oswald-Bold.ttf", 15);
  char index_str[8];
  char c;
  int i;
  RGBCanvas* canvas;
  Pen num_pen = {1, (int)(leddisp->GetHeight() - 2)};
  Pen picto_pen = {(int)(leddisp->GetWidth() / 2 + 1), (int)(leddisp->GetHeight() - 2)};

  while (1) {
    for (c=' ', i=0; c <= '}'; c++, i++) {
      snprintf(index_str, sizeof(index_str), "%d", c);
      canvas = new RGBCanvas(leddisp->GetWidth(), leddisp->GetHeight());
      font.PaintString(index_str, canvas, &num_pen, 0xff, 0, 0);
      pictofont.PaintChar(c, canvas, &picto_pen, 0, 0xff, 0xff);
      leddisp->SwapCanvas(canvas, LEDDisplay::kLeft, LEDDisplay::kNoScroll);
      usleep(700000);
    }
  }
}

int main(int argc, char *argv[]) {
  int demo = 0;
  LEDDisplay* leddisp = new LEDDisplay();

  if (argc > 1) {
    demo = atoi(argv[1]);
  }
  fprintf(stderr, "Using demo %d\n", demo);

  switch (demo) {
  case 0:
    leddisplay_demo(leddisp);
    break;

  case 1:
    canvas_invert_demo(leddisp);
    break;

  case 2:
    canvas_picto_demo(leddisp);
    break;

  case 3:
    delete leddisp;
    return 0;

  default:
    printf("Unknown demo: %d\n", demo);
    break;
  }

  return 0;
}
