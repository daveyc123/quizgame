#include "led-display.h"
#include "led-matrix.h"

#include <unistd.h>

// Base-class for a Thread that does something with a matrix.
class RGBMatrixManipulator : public Thread {
public:
  RGBMatrixManipulator(RGBMatrix *m) : running_(true), matrix_(m) {}
  virtual ~RGBMatrixManipulator() { running_ = false; }

  // Run() implementation needs to check running_ regularly.

protected:
  volatile bool running_;  // TODO: use mutex, but this is good enough for now.
  RGBMatrix *const matrix_;
};

// Pump pixels to screen. Needs to be high priority real-time because jitter
// here will make the PWM uneven.
class DisplayUpdater : public RGBMatrixManipulator {
public:
  DisplayUpdater(RGBMatrix *m) : RGBMatrixManipulator(m) {}

  void Run() {
    while (running_) {
      matrix_->UpdateScreen();
    }
  }
};

LEDDisplay::LEDDisplay()
{
  running_ = false;
  started_ = false;
  io = new GPIO();
  if (io == NULL || !io->Init()) {
  	fprintf(stderr, "Error initializing GPIOs\n");
  	return;
  }

  matrix_ = new RGBMatrix(io);
  if (matrix_ == NULL) {
  	fprintf(stderr, "Error initializing RGBMatrix\n");
  	return;
  }

  SetVariableFont("fonts/Oswald-Bold.ttf", 15);
  SetMonoFont("fonts/UbuntuMono-Bold.ttf", 17);
  SetPictoFont("fonts/modernpics.otf", 18);
  SetPictoXChar(88);
  SetPictoCheckmarkChar(37);
  SetPictoHeartChar(106);
  SetTimerPunctuationWidth(6);
  SetScrollInterval(50000);
  SetRGB(0xff, 0, 0);

  pthread_mutex_init(&mutex_, NULL);
  pthread_cond_init(&cond_, NULL);

  canvas_ = NULL;
  next_canvas_ = NULL;
  free_canvas_ = true;

  running_ = true;
}

LEDDisplay::~LEDDisplay()
{
  pthread_mutex_lock(&mutex_);
  running_ = false;
  pthread_cond_broadcast(&cond_);
  pthread_mutex_unlock(&mutex_);

  // Final thing before exit: clear screen and update once, so that
  // we don't have random pixels burn
  matrix_->ClearScreen();
  matrix_->UpdateScreen();
}

void LEDDisplay::Run()
{
  unsigned i = 0;
  unsigned j = 0;
  int x_offset = 0;
  bool x_scroll = false;
  bool y_scroll = false;

  pthread_mutex_lock(&mutex_);
  started_ = 1;
  pthread_mutex_unlock(&mutex_);

  RGBMatrixManipulator *updater = new DisplayUpdater(matrix_);
  updater->Start(10);  // high priority

  while (running_) {
  	pthread_mutex_lock(&mutex_);
    while (true) {
  	  if (scrolling_type_ != kNoScroll || next_canvas_ != NULL || !running_) {
  	    break;
  	  }
#ifdef DEBUG
      printf("LEDDisplay blocking\n");
#endif
      pthread_cond_wait(&cond_, &mutex_);
#ifdef DEBUG
      printf("LEDDisplay unblocking\n");
#endif
    }

    if (next_canvas_ != NULL) {
      /* Animation sequence changed (ex: new string), reinit */
      i = 0;
      j = 0;

      if (canvas_ && canvas_ != next_canvas_ && free_canvas_) {
        delete canvas_;
      }
      canvas_ = next_canvas_;
      next_canvas_ = NULL;

      /* We need to clear the screen in case the incoming canvas is smaller than the old one */
      matrix_->ClearScreen();

  	  switch (text_pos_) {
  	    case kLeft:
  	    case kDupeLeft:
  	      x_offset = 0;
  	      break;
  	    case kRight:
  	    case kDupeRight:
  	      x_offset = matrix_->width() - canvas_->width();
  	      break;
  	    case kCenter:
  	    case kDupeCenter:
  	      x_offset = (matrix_->width() / 2) - (canvas_->width() / 2);
  	      break;
  	    default:
  	      fprintf(stderr, "Unknown text position\n");
  	      break;
  	  }
    }

    pthread_mutex_unlock(&mutex_);

    switch (scrolling_type_) {
      case kNoScroll:
        x_scroll = false;
        y_scroll = false;
        break;
      case kLeftScroll:
        x_scroll = true;
        y_scroll = false;
        i -= 1;
        break;
      case kRightScroll:
        x_scroll = true;
        y_scroll = false;
        i += 1;
        break;
      case kUpScroll:
        x_scroll = false;
        y_scroll = true;
        j -= 1;
        break;
      case kDownScroll:
        x_scroll = false;
        y_scroll = true;
        j += 1;
        break;
    }

    canvas_->Display(matrix_, x_offset + (i % canvas_->width()), (j % canvas_->width()), x_scroll, y_scroll);

    if (scrolling_type_ != kNoScroll) {
    	usleep(scrolling_interval_);
    }
  }

  delete updater;
}

void LEDDisplay::SetVariableFont(const char* path, unsigned sz)
{
  variable_font_ = new Font(path, sz);
}

void LEDDisplay::SetMonoFont(const char* path, unsigned sz)
{
  mono_font_ = new Font(path, sz);
}

void LEDDisplay::SetPictoFont(const char* path, unsigned sz)
{
  picto_font_ = new Font(path, sz);
}


void LEDDisplay::SetPictoXChar(char c)
{
  picto_x_ = c;
}

void LEDDisplay::SetPictoCheckmarkChar(char c)
{
  picto_checkmark_ = c;
}

void LEDDisplay::SetPictoHeartChar(char c)
{
  picto_heart_ = c;
}

void LEDDisplay::SetTimerPunctuationWidth(unsigned width)
{
  timer_punctuation_width_ = width;
}

void LEDDisplay::DisplayString(const char* text, text_pos_t pos, text_scrolling_t scrolling, text_font_type_t font_type)
{
  Font* font;
  RGBCanvas* canvas;
  unsigned canvas_width;
  Pen pen;

  switch (font_type) {
  	case kVariable:
  	  font = variable_font_;
  	  break;
  	case kMono:
  	  font = mono_font_;
  	  break;
  	case kPicto:
  	  font = picto_font_;
  	  break;
  	default:
  	  fprintf(stderr, "Unknown font type\n");
  	  return;
  }

  canvas_width = font->GetWidth(text);
  canvas = new RGBCanvas(canvas_width, matrix_->height());

  pen.x = 1;
  pen.y = matrix_->height() - 2;
  font->PaintString(text, canvas, &pen, r_, g_, b_);
  SwapCanvas(canvas, pos, scrolling);
}

void LEDDisplay::DisplayFill()
{
  RGBCanvas* canvas;

  canvas = new RGBCanvas(matrix_->width(), matrix_->height());
  canvas->Fill(r_, g_, b_);
  SwapCanvas(canvas, kLeft, kNoScroll);
}

void LEDDisplay::DisplayPicto_(char c, text_pos_t pos, text_scrolling_t scrolling)
{
  RGBCanvas* canvas;
  unsigned picto_width = picto_font_->GetWidth(c);
  Pen pen = {0, (int)(matrix_->height() / 2 + picto_font_->GetHeight(c) / 2)};

  canvas = new RGBCanvas(matrix_->width(), matrix_->height());

  switch (pos) {
    case kLeft:
      pen.x = 0;
      break;
    case kRight:
      pen.x = matrix_->width() - picto_width;
      break;
    case kCenter:
      pen.x = matrix_->width() / 2 - picto_width / 2;
      break;
    case kDupeLeft:
      pen.x = 0;
      break;
    case kDupeRight:
      pen.x = matrix_->width() / 2 - picto_width;
      break;
    case kDupeCenter:
      pen.x = matrix_->width() / 4 - picto_width / 2;
      break;
  }

  picto_font_->PaintChar(c, canvas, &pen, r_, g_, b_);
  if (pos == kDupeLeft || pos == kDupeRight || pos == kDupeCenter) {
    pen.x += matrix_->width() / 2;
    picto_font_->PaintChar(c, canvas, &pen, r_, g_, b_);
  }

  SwapCanvas(canvas, kLeft, scrolling);
}

void LEDDisplay::DisplayX(text_pos_t pos, text_scrolling_t scrolling)
{
  DisplayPicto_(picto_x_, pos, scrolling);
}

void LEDDisplay::DisplayCheckmark(text_pos_t pos, text_scrolling_t scrolling)
{
  DisplayPicto_(picto_checkmark_, pos, scrolling);
}

void LEDDisplay::DisplayHeart(text_pos_t pos, text_scrolling_t scrolling)
{
  DisplayPicto_(picto_heart_, pos, scrolling);
}

void LEDDisplay::DisplayTimer(unsigned ms)
{
  RGBCanvas* canvas = new RGBCanvas(matrix_->width(), matrix_->height());
  unsigned minutes;
  unsigned seconds;
  unsigned tenthsofseconds;
  unsigned remainder;
  char timestr[16];
  unsigned punctuation_correction;
  Pen pen;

  minutes = ms / 60000;
  remainder = ms % 60000;
  seconds = remainder / 1000;
  remainder = remainder % 1000;
  tenthsofseconds = remainder / 100;
  punctuation_correction = (mono_font_->GetWidth(':') - timer_punctuation_width_) / 2;

  pen.x = 1;
  pen.y = matrix_->height() - 2;

  /* Minutes */
  snprintf(timestr, sizeof(timestr), "%02d", minutes);
  mono_font_->PaintString(timestr, canvas, &pen, r_, g_, b_);
  canvas->pen.x -= punctuation_correction;
  mono_font_->PaintString(":", canvas, NULL, r_, g_, b_);
  canvas->pen.x -= punctuation_correction;

  /* Seconds */
  snprintf(timestr, sizeof(timestr), "%02d", seconds);
  mono_font_->PaintString(timestr, canvas, NULL, r_, g_, b_);
  canvas->pen.x -= punctuation_correction;
  mono_font_->PaintString(".", canvas, NULL, r_, g_, b_);
  canvas->pen.x -= punctuation_correction;

  /* 1/10th of seconds */
  snprintf(timestr, sizeof(timestr), "%d", tenthsofseconds);
  mono_font_->PaintString(timestr, canvas, NULL, r_, g_, b_);

  SwapCanvas(canvas, kLeft, kNoScroll);
}

void LEDDisplay::DisplayClear()
{
  RGBCanvas* canvas = new RGBCanvas(matrix_->width(), matrix_->height());
  SwapCanvas(canvas, kLeft, kNoScroll);
}

void LEDDisplay::SwapCanvas(RGBCanvas* canvas, text_pos_t pos, text_scrolling_t scrolling, bool free)
{
  pthread_mutex_lock(&mutex_);
  next_canvas_ = canvas;
  text_pos_ = pos;
  scrolling_type_ = scrolling;
  free_canvas_ = free;

  if (!started_) {
    Start();
  }

  pthread_cond_broadcast(&cond_);
  pthread_mutex_unlock(&mutex_);
}

void LEDDisplay::SwapCanvas(RGBCanvas* canvas, text_pos_t pos, text_scrolling_t scrolling)
{
  SwapCanvas(canvas, pos, scrolling, true);
}

void LEDDisplay::SetScrollInterval(unsigned usec)
{
  scrolling_interval_ = usec;
}

void LEDDisplay::SetRGB(unsigned char r, unsigned char g, unsigned char b)
{
  r_ = r;
  g_ = g;
  b_ = b;
}
