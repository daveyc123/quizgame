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

  void SetVariableFont(const char* path, unsigned sz);
  void SetMonoFont(const char* path, unsigned sz);
  void SetPictoFont(const char* path, unsigned sz);

  /* To map the X and checkmark icons from the picto font */
  void SetPictoXChar(char c);
  void SetPictoCheckmarkChar(char c);
  void SetPictoHeartChar(char c);

  /* Used to shrink down the white space used by the '.' and ':' chars in the timer */
  void SetTimerPunctuationWidth(unsigned width);

  void DisplayString(char* text, text_pos_t pos, text_scrolling_t scrolling, text_font_type_t font_type);
  void DisplayFill();
  void DisplayX(text_pos_t pos, text_scrolling_t scrolling);
  void DisplayCheckmark(text_pos_t pos, text_scrolling_t scrolling);
  void DisplayHeart(text_pos_t pos, text_scrolling_t scrolling);
  void DisplayTimer(unsigned ms);

  void SetScrollInterval(unsigned usec); /* The smaller the interval, the faster the scrolling */

  void SetRGB(unsigned char r, unsigned char g, unsigned char b);

protected:
  RGBMatrix* matrix_;
  volatile bool running_;  // TODO: use mutex, but this is good enough for now.
  volatile bool started_;
  Font* variable_font;
  Font* mono_font;
  Font* picto_font;
  volatile char picto_x;
  volatile char picto_checkmark;
  volatile char picto_heart;
  RGBCanvas* canvas_;
  RGBCanvas* next_canvas_; /* Used for transitions */
  volatile unsigned char r_;
  volatile unsigned char g_;
  volatile unsigned char b_;

  volatile unsigned scrolling_interval_;
  volatile text_scrolling_t scrolling_type_;
  volatile text_pos_t text_pos_;
  pthread_mutex_t mutex;
  pthread_cond_t cond;

  unsigned timer_punctuation_width_;

private:
  GPIO* io;

  void DisplayPicto_(char c, text_pos_t pos, text_scrolling_t scrolling);
};
