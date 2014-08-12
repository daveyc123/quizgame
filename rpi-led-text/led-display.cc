#include "led-display.h"
#include "led-matrix.h"

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
  SetPictoFont("fonts/modernpics.otf", 16);
  SetPictoXChar(56);
  SetPictoCheckmarkChar(68);
  SetScrollInterval(50000);
  SetRGB(0xff, 0, 0);

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  canvas_ = NULL;
  next_canvas_ = NULL;

  running_ = true;
}

LEDDisplay::~LEDDisplay()
{
  pthread_mutex_lock(&mutex);
  running_ = false;
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mutex);

  // Final thing before exit: clear screen and update once, so that
  // we don't have random pixels burn
  matrix_->ClearScreen();
  matrix_->UpdateScreen();
}

void LEDDisplay::Run()
{
  unsigned i = 0;
  unsigned j = 0;
  bool x_scroll;
  bool y_scroll;

  pthread_mutex_lock(&mutex);
  started_ = 1;
  pthread_mutex_unlock(&mutex);

  RGBMatrixManipulator *updater = new DisplayUpdater(matrix_);
  updater->Start(10);  // high priority

  while (running_) {
  	pthread_mutex_lock(&mutex);
    while (true) {
  	  if (scrolling_type_ != kNoScroll || next_canvas_ != NULL || !running_) {
  	    break;
  	  }
#ifdef DEBUG
      printf("LEDDisplay blocking\n");
#endif
      pthread_cond_wait(&cond, &mutex);
#ifdef DEBUG
      printf("LEDDisplay unblocking\n");
#endif
    }

    if (canvas_ != NULL || next_canvas_ != NULL) {
      /* Animation sequence changed (ex: new string), reinit */
      int x_offset;
      i = 0;
      j = 0;

      if (canvas_) {
        free((void*)canvas_);
      }
      canvas_ = next_canvas_;
      next_canvas_ = NULL;

	  switch (text_pos_) {
	    case kLeft:
	    case kDupeLeft:
	      x_offset = 1;
	      break;
	    case kRight:
	    case kDupeRight:
	      x_offset = matrix_->width() - canvas_->width() - 1;
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

    pthread_mutex_unlock(&mutex);

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

    canvas_->Display(matrix_, (i % canvas_->width()), (j % canvas_->width()), x_scroll, y_scroll);
  }

  delete updater;
}

void LEDDisplay::SetVariableFont(const char* path, unsigned sz)
{
  variable_font = new Font(path, sz);
}

void LEDDisplay::SetMonoFont(const char* path, unsigned sz)
{
  mono_font = new Font(path, sz);
}

void LEDDisplay::SetPictoFont(const char* path, unsigned sz)
{
  picto_font = new Font(path, sz);
}


void LEDDisplay::SetPictoXChar(char c)
{
  picto_x = c;
}

void LEDDisplay::SetPictoCheckmarkChar(char c)
{
  picto_checkmark = c;
}

void LEDDisplay::DisplayString(char* text, text_pos_t pos, text_scrolling_t scrolling, text_font_type_t font_type)
{
  Font* font;
  RGBCanvas* canvas;
  unsigned canvas_width;
  int x;
  int y;

  switch (font_type) {
  	case kVariable:
  	  font = variable_font;
  	  break;
  	case kMono:
  	  font = mono_font;
  	  break;
  	case kPicto:
  	  font = picto_font;
  	  break;
  	default:
  	  fprintf(stderr, "Unknown font type\n");
  	  return;
  }

  canvas_width = font->GetWidth(text);
  canvas = new RGBCanvas(canvas_width, matrix_->height());

  font->PaintString(text, canvas, 1, matrix_->height() - 2, r_, g_, b_);
  pthread_mutex_lock(&mutex);
  next_canvas_ = canvas;
  scrolling_type_ = scrolling;
  text_pos_ = pos;

  if (!started_) {
  	Start();
  }

  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mutex);
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