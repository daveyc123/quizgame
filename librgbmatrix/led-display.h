#include "text.h"
#include "thread.h"
#include "gpio.h"

class LEDDisplay : public Thread {
public:
  LEDDisplay();
  virtual ~LEDDisplay();

  typedef enum {
    kLeft,
    kRight,
    kCenter,

    /* To duplicate pictograms across all panels */
    kDupeLeft,
    kDupeRight,
    kDupeCenter
  } text_pos_t;

  typedef enum {
  	kNoScroll,
  	kLeftScroll,
  	kRightScroll,
  	kUpScroll,
  	kDownScroll
  } text_scrolling_t;

  typedef enum {
  	kVariable,
  	kMono,
  	kPicto
  } text_font_type_t;

  void Run(); // Run() implementation needs to check running_ regularly.

  unsigned GetWidth() { return matrix_->width(); };
  unsigned GetHeight() { return matrix_->height(); };

  void SetVariableFont(const char* path, unsigned sz);
  void SetMonoFont(const char* path, unsigned sz);
  void SetPictoFont(const char* path, unsigned sz);

  /* To map the X and checkmark icons from the picto font */
  void SetPictoXChar(char c);
  void SetPictoCheckmarkChar(char c);
  void SetPictoHeartChar(char c);

  void SetScrollInterval(unsigned usec); /* The smaller the interval, the faster the scrolling */
  void SetRGB(unsigned char r, unsigned char g, unsigned char b);

  /* Used to shrink down the white space used by the '.' and ':' chars in the timer */
  void SetTimerPunctuationWidth(unsigned width);

  void DisplayString(const char* text, text_pos_t pos, text_scrolling_t scrolling, text_font_type_t font_type);
  void DisplayFill();
  void DisplayX(text_pos_t pos, text_scrolling_t scrolling);
  void DisplayCheckmark(text_pos_t pos, text_scrolling_t scrolling);
  void DisplayHeart(text_pos_t pos, text_scrolling_t scrolling);
  void DisplayTimer(unsigned ms);
  void DisplayClear();
  void SwapCanvas(RGBCanvas* canvas, text_pos_t pos, text_scrolling_t scrolling, bool free);
  void SwapCanvas(RGBCanvas* canvas, text_pos_t pos, text_scrolling_t scrolling);

protected:
  RGBMatrix* matrix_;
  volatile bool running_;  // TODO: use mutex, but this is good enough for now.
  volatile bool started_;
  Font* variable_font_;
  Font* mono_font_;
  Font* picto_font_;
  volatile char picto_x_;
  volatile char picto_checkmark_;
  volatile char picto_heart_;
  RGBCanvas* canvas_;
  RGBCanvas* next_canvas_; /* Used for transitions */
  bool free_canvas_;
  volatile unsigned char r_;
  volatile unsigned char g_;
  volatile unsigned char b_;

  volatile unsigned scrolling_interval_;
  volatile text_scrolling_t scrolling_type_;
  volatile text_pos_t text_pos_;
  pthread_mutex_t mutex_;
  pthread_cond_t cond_;

  unsigned timer_punctuation_width_;

private:
  GPIO* io;

  void DisplayPicto_(char c, text_pos_t pos, text_scrolling_t scrolling);
};
