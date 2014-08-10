#include "thread.h"
#include "led-matrix.h"

#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H

using std::min;
using std::max;

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

// -- The following are demo image generators.

// Simple generator that pulses through RGB and White.
class ColorPulseGenerator : public RGBMatrixManipulator {
public:
  ColorPulseGenerator(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run() {
    const int width = matrix_->width();
    const int height = matrix_->height();
    uint32_t count = 0;
    while (running_) {
      usleep(5000);
      ++count;
      int color = (count >> 9) % 6;
      int value = count & 0xFF;
      if (count & 0x100) value = 255 - value;
      int r, g, b;
      switch (color) {
      case 0: r = value; g = b = 0; break;
      case 1: r = g = value; b = 0; break;
      case 2: g = value; r = b = 0; break;
      case 3: g = b = value; r = 0; break;
      case 4: b = value; r = g = 0; break;
      default: r = g = b = value; break;
      }
      for (int x = 0; x < width; ++x)
        for (int y = 0; y < height; ++y)
          matrix_->SetPixel(x, y, r, g, b);
    }
  }
};

class SimpleSquare : public RGBMatrixManipulator {
public:
  SimpleSquare(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run() {
    const int width = matrix_->width();
    const int height = matrix_->height();
    // Diagonaly
    for (int x = 0; x < width; ++x) {
        matrix_->SetPixel(x, x, 255, 255, 255);
        matrix_->SetPixel(height -1 - x, x, 255, 0, 255);
    }
    for (int x = 0; x < width; ++x) {
      matrix_->SetPixel(x, 0, 255, 0, 0);
      matrix_->SetPixel(x, height - 1, 255, 255, 0);
    }
    for (int y = 0; y < height; ++y) {
      matrix_->SetPixel(0, y, 0, 0, 255);
      matrix_->SetPixel(width - 1, y, 0, 255, 0);
    }
  }
};

// Simple class that generates a rotating block on the screen.
class RotatingBlockGenerator : public RGBMatrixManipulator {
public:
  RotatingBlockGenerator(RGBMatrix *m) : RGBMatrixManipulator(m) {}

  uint8_t scale_col(int val, int lo, int hi) {
    if (val < lo) return 0;
    if (val > hi) return 255;
    return 255 * (val - lo) / (hi - lo);
  }

  void Run() {
    const int cent_x = matrix_->width() / 2;
    const int cent_y = matrix_->height() / 2;

    // The square to rotate (inner square + black frame) needs to cover the
    // whole area, even if diagnoal.
    const int rotate_square = min(matrix_->width(), matrix_->height()) * 1.41;
    const int min_rotate = cent_x - rotate_square / 2;
    const int max_rotate = cent_x + rotate_square / 2;

    // The square to display is within the visible area.
    const int display_square = min(matrix_->width(), matrix_->height()) * 0.7;
    const int min_display = cent_x - display_square / 2;
    const int max_display = cent_x + display_square / 2;

    const float deg_to_rad = 2 * 3.14159265 / 360;
    int rotation = 0;
    while (running_) {
      ++rotation;
      usleep(15 * 1000);
      rotation %= 360;
      for (int x = min_rotate; x < max_rotate; ++x) {
        for (int y = min_rotate; y < max_rotate; ++y) {
          float disp_x, disp_y;
          Rotate(x - cent_x, y - cent_y,
                 deg_to_rad * rotation, &disp_x, &disp_y);
          if (x >= min_display && x < max_display &&
              y >= min_display && y < max_display) { // within display square
            matrix_->SetPixel(disp_x + cent_x, disp_y + cent_y,
                              scale_col(x, min_display, max_display),
                              255 - scale_col(y, min_display, max_display),
                              scale_col(y, min_display, max_display));
          } else {
            // black frame.
            matrix_->SetPixel(disp_x + cent_x, disp_y + cent_y, 0, 0, 0);
          }
        }
      }
    }
  }

private:
  void Rotate(int x, int y, float angle,
              float *new_x, float *new_y) {
    *new_x = x * cosf(angle) - y * sinf(angle);
    *new_y = x * sinf(angle) + y * cosf(angle);
  }
};

class ImageScroller : public RGBMatrixManipulator {
public:
  ImageScroller(RGBMatrix *m)
    : RGBMatrixManipulator(m), image_(NULL), horizontal_position_(0) {
  }

  // _very_ simplified. Can only read binary P6 PPM. Expects newlines in headers
  // Not really robust. Use at your own risk :)
  bool LoadPPM(const char *filename) {
    if (image_) {
      delete [] image_;
      image_ = NULL;
    }
    FILE *f = fopen(filename, "r");
    if (f == NULL) return false;
    char header_buf[256];
    const char *line = ReadLine(f, header_buf, sizeof(header_buf));
#define EXIT_WITH_MSG(m) { fprintf(stderr, "%s: %s |%s", filename, m, line); \
      fclose(f); return false; }
    if (sscanf(line, "P6 ") == EOF)
      EXIT_WITH_MSG("Can only handle P6 as PPM type.");
    line = ReadLine(f, header_buf, sizeof(header_buf));
    if (!line || sscanf(line, "%d %d ", &width_, &height_) != 2)
      EXIT_WITH_MSG("Width/height expected");
    int value;
    line = ReadLine(f, header_buf, sizeof(header_buf));
    if (!line || sscanf(line, "%d ", &value) != 1 || value != 255)
      EXIT_WITH_MSG("Only 255 for maxval allowed.");
    const size_t pixel_count = width_ * height_;
    image_ = new Pixel [ pixel_count ];
    assert(sizeof(Pixel) == 3);   // we make that assumption.
    if (fread(image_, sizeof(Pixel), pixel_count, f) != pixel_count) {
      line = "";
      EXIT_WITH_MSG("Not enough pixels read.");
    }
#undef EXIT_WITH_MSG
    fclose(f);
    fprintf(stderr, "Read image with %dx%d\n", width_, height_);
    horizontal_position_ = 0;
    return true;
  }

  void Run() {
    const int screen_height = matrix_->height();
    const int screen_width = matrix_->width();
    while (running_) {
      if (image_ == NULL) {
        usleep(100 * 1000);
        continue;
      }
      usleep(30 * 1000);
      for (int x = 0; x < screen_width; ++x) {
        for (int y = 0; y < screen_height; ++y) {
          const Pixel &p = getPixel((horizontal_position_ + x) % width_, y);
          // Display upside down on my desk. Lets flip :)
          int disp_x = screen_width - x;
          int disp_y = screen_height - y;
          matrix_->SetPixel(disp_x, disp_y, p.red, p.green, p.blue);
        }
      }
      ++horizontal_position_;
    }
  }

private:
  struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };

  // Read line, skip comments.
  char *ReadLine(FILE *f, char *buffer, size_t len) {
    char *result;
    do {
      result = fgets(buffer, len, f);
    } while (result != NULL && result[0] == '#');
    return result;
  }

  const Pixel &getPixel(int x, int y) {
    static Pixel dummy;
    if (x < 0 || x > width_ || y < 0 || y > height_) return dummy;
    return image_[x + width_ * y];
  }

  int width_;
  int height_;
  Pixel *image_;
  uint32_t horizontal_position_;
};

struct RenderedChar {
public:
  FT_Int width;
  FT_Int rows;
  FT_Int pitch;
  FT_Int advance_x;
  FT_Int advance_y;
  FT_Int bitmap_left;
  FT_Int bitmap_top;
  unsigned char* buf;
};

// Simple generator that walks the pixel matrix
class TextRenderer : public RGBMatrixManipulator {
public:
  TextRenderer(RGBMatrix *m) : RGBMatrixManipulator(m) {
    InitCache();
  }

  void DisplayBitmap(RenderedChar* rchar, FT_Int pen_x, FT_Int pen_y) {
    FT_Int i, j, p, q;
    FT_Int x = pen_x + rchar->bitmap_left;
    FT_Int y = pen_y - rchar->bitmap_top;
    FT_Int x_max = x + rchar->width;
    FT_Int y_max = y + rchar->rows;
    unsigned char pixel;

#ifdef DEBUG
    printf("DEBUG: pen_x = %d, pen_y = %d, x = %d, y = %d\n", pen_x, pen_y, x, y);
#endif

    for (i = x, p = 0; i < x_max; i++, p++) {
      for (j = y, q = 0; j < y_max; j++, q++) {
        if (i < 0 || j < 0 || i >= matrix_->width() || j >= matrix_->height()) {
#ifdef DEBUG
          printf("Character chopped\n");
#endif
          continue;
        }

        pixel = rchar->buf[q * rchar->width + p];
        matrix_->SetPixel(i, j, pixel ? 0xff : 0, pixel ? 0xff : 0, pixel ? 0xff : 0);
      }
    }
  }

  void DisplayString(char* text) {
    FT_Int pen_x = 1;
    FT_Int pen_y = matrix_->height() - 2;
    FT_Int i, j, p, q;
    RenderedChar* rchar;
    FT_Int x_max;
    FT_Int y_max;
    unsigned char pixel;
    char c;

    if (text == NULL) {
      return;
    }

    while (c = *text++) {
      rchar = &char_cache_[c];
      DisplayBitmap(rchar, pen_x, pen_y);
      pen_x += rchar->advance_x;
      pen_y += rchar->advance_y;
    }
  }

  void Run() {
    FT_GlyphSlot slot;
    FT_Error error;
    RenderedChar* rchar;
    unsigned char val = '!';

    // slot = face_->glyph;
    // while (running_) {
    //   rchar = &char_cache_[val];
    //   DisplayBitmap(rchar, 1, (matrix_->height() - 1));
    //   val += 1;
    //   if (val > '~') {
    //     val = '!';
    //   }
    //   usleep(500000);
    //   matrix_->ClearScreen();
    // }

    DisplayString("Woot!");
    while (running_) {
      sleep(1);
    }
  }

private:
  FT_Library library_;
  FT_Face face_;
  RenderedChar char_cache_[256];

  // bool GetCharPixel_(const FT_Bitmap* bitmap, const int x, const int y)
  // {
  //   int pitch = abs(bitmap->pitch);
  //   unsigned char *row = &bitmap->buffer[pitch * y];
  //   char cValue = row[x >> 3];

  //   return (cValue & (128 >> (x & 7))) != 0;
  // }

  void InitCache()
  {
    const int width = matrix_->width();
    const int height = matrix_->height();
    unsigned i;
    FT_GlyphSlot slot;
    FT_Error error;

    error = FT_Init_FreeType(&library_);
    if (error) {
      fprintf(stderr, "Failed to initialize FreeType: %d\n", error);
      goto fail1;
    }

    error = FT_New_Face(library_, "fonts/Oswald-Bold.ttf", 0, &face_);
    if (error) {
      fprintf(stderr, "Failed to load font: %d\n", error);
      goto fail2;
    }

    error = FT_Set_Char_Size(face_, 0, 15*64, 72, 72);
    if (error) {
      fprintf(stderr, "Failed to set font size: %d\n", error);
      goto fail3;
    }

    for (i = 0; i < sizeof(char_cache_) / sizeof(char_cache_[0]); i++) {
      RenderAndCache_(i);
    }

    return;

    fail3:
    FT_Done_Face(face_);

    fail2:
    FT_Done_FreeType(library_);

    fail1:
    return;
  }

  void RenderAndCache_(unsigned char val)
  {
    FT_UInt glyph_index;
    FT_GlyphSlot slot;
    FT_Bitmap* bitmap;
    FT_Error error;

    // printf("Rendering %d (%c)\n", val, val);

    // Currently only support caching ASCII chars
    if (val > 255) {
      fprintf(stderr, "Don't support caching non-ASCII characters\n");
      return;
    }

    glyph_index = FT_Get_Char_Index(face_, val);
    if (glyph_index == 0) {
      fprintf(stderr, "Failed to get glyph index\n");
      return;
    }

    error = FT_Load_Glyph(face_, glyph_index, FT_LOAD_MONOCHROME);
    if (error) {
      fprintf(stderr, "Failed to load glyph: %d\n", error);
      return;
    }

    error = FT_Render_Glyph(face_->glyph, FT_RENDER_MODE_MONO);
    if (error) {
      fprintf(stderr, "Failed to render glyph: %d\n", error);
      return;
    }

    slot = face_->glyph;
    bitmap = &slot->bitmap;
    char_cache_[val].width = bitmap->width;
    char_cache_[val].rows = bitmap->rows;
    char_cache_[val].pitch = bitmap->pitch;
    char_cache_[val].advance_x = slot->advance.x >> 6;
    char_cache_[val].advance_y = slot->advance.y >> 6;
    char_cache_[val].bitmap_left = slot->bitmap_left;
    char_cache_[val].bitmap_top = slot->bitmap_top;
    char_cache_[val].buf = UnpackMonoBitmap_(bitmap);

#ifdef DEBUG
    printf("DEBUG: Rendered %d '%c': width=%d, rows=%d, pitch=%d\n", val, val, bitmap->width, bitmap->rows, bitmap->pitch);
#endif

  }

  unsigned char* UnpackMonoBitmap_(FT_Bitmap* bitmap)
  {
    unsigned char* result;
    int y, x, byte_index, num_bits_done, rowstart, bits, bit_index;
    unsigned char byte_value;
    
    result = (unsigned char*)malloc(bitmap->rows * bitmap->width);
    
    for (y = 0; y < bitmap->rows; y++) {
      for (byte_index = 0; byte_index < bitmap->pitch; byte_index++) {
        byte_value = bitmap->buffer[y * bitmap->pitch + byte_index];
        num_bits_done = byte_index * 8;
        rowstart = y * bitmap->width + byte_index * 8;

        bits = 8;
        if ((bitmap->width - num_bits_done) < 8) {
          bits = bitmap->width - num_bits_done;
        }
            
        for (bit_index = 0; bit_index < bits; bit_index++) {
          int bit;
          bit = byte_value & (1 << (7 - bit_index));
          result[rowstart + bit_index] = bit;
        }
      }
    }
    
    return result;
  }
};

// Simple generator that walks the pixel matrix
class PixelIterator : public RGBMatrixManipulator {
public:
  PixelIterator(RGBMatrix *m, uint8_t r, uint8_t g, uint8_t b) : RGBMatrixManipulator(m), r_(r), g_(g), b_(b) {}
  void Run() {
    const int width = matrix_->width();
    const int height = matrix_->height();
    while (running_) {
      for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
          matrix_->SetPixel(x, y, r_, g_, b_);
          usleep(10000);
        }
      }

      for (int x = width-1; x >= 0; --x) {
        for (int y = height-1; y >= 0; --y) {
          matrix_->SetPixel(x, y, 0, 0, 0);
          usleep(10000);
        }
      }
    }
  }

private:
  uint8_t r_;
  uint8_t g_;
  uint8_t b_;
};

class PWMTest : public RGBMatrixManipulator {
public:
  PWMTest(RGBMatrix *m) : RGBMatrixManipulator(m) {}
  void Run() {
    const int width = matrix_->width();
    const int height = matrix_->height();
    unsigned char val = 0xff;
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        matrix_->SetPixel(x, y, val--, 0, 0);
      }
    }

    while (running_) {
      sleep(1);
    }
  }
};

int main(int argc, char *argv[]) {
  int demo = 0;
  if (argc > 1) {
    demo = atoi(argv[1]);
  }
  fprintf(stderr, "Using demo %d\n", demo);

  GPIO io;
  if (!io.Init())
    return 1;

  RGBMatrix m(&io);
    
  RGBMatrixManipulator *image_gen = NULL;
  switch (demo) {
  case 0:
    image_gen = new RotatingBlockGenerator(&m);
    break;

  case 1:
    if (argc > 2) {
      ImageScroller *scroller = new ImageScroller(&m);
      if (!scroller->LoadPPM(argv[2]))
        return 1;
      image_gen = scroller;
    } else {
      fprintf(stderr, "Demo %d Requires PPM image as parameter", demo);
      return 1;
    }
    break;

  case 2:
    image_gen = new SimpleSquare(&m);
    break;

  case 3:
    image_gen = new PixelIterator(&m, 0xff, 0xff, 0xff);
    break;

  case 4:
    image_gen = new TextRenderer(&m);
    break;

  case 5:
    image_gen = new PWMTest(&m);
    break;

  default:
    image_gen = new ColorPulseGenerator(&m);
    break;
  }

  if (image_gen == NULL)
    return 1;

  RGBMatrixManipulator *updater = new DisplayUpdater(&m);
  updater->Start(10);  // high priority

  image_gen->Start();

  // Things are set up. Just wait for <RETURN> to be pressed.
  printf("Press <RETURN> to exit and reset LEDs\n");
  getchar();

  // Stopping threads and wait for them to join.
  delete image_gen;
  delete updater;

  // Final thing before exit: clear screen and update once, so that
  // we don't have random pixels burn
  m.ClearScreen();
  m.UpdateScreen();

  return 0;
}
