#include "led-matrix.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#ifndef __TEXT_H
#define __TEXT_H

struct RenderedChar {
  FT_Int width;
  FT_Int rows;
  FT_Int pitch;
  FT_Int advance_x;
  FT_Int advance_y;
  FT_Int bitmap_left;
  FT_Int bitmap_top;
  unsigned char* buf;
};

struct Pen {
	int x;
	int y;
};

struct Pixel;

class RGBCanvas {
public:
  RGBCanvas(unsigned width, unsigned height) : width_(width), height_(height) {
  	InitCanvas_();
  };

  /**
   * x_offset: A positive value will shift the canvas to the right, negative to the left.
   * y_offset: A positive value will shift the canvas down, negative up.
   * x_wrap: If true, will wrap either the left or right edge of the canvas when appropriate.
   * y_wrap: If true, will wrap either the top or bottom edge of the canvas when appropriate.
   *
   * Wrapping is appropriate when one canvas edge is on display while the opposite edge is not.
   */
  void Display(RGBMatrix* m, int x_offset, int y_offset, bool x_wrap, bool y_wrap);
  void SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
  void Fill(unsigned char r, unsigned char g, unsigned char b);
  void Clear();
  unsigned width() { return width_; };
  unsigned height() { return height_; };
  Pen pen;

private:
  Pixel* pixbuf_;
  unsigned width_;
  unsigned height_;

  void InitCanvas_();

};

class Font {
public:
  Font(const char* fontname, const FT_UInt fontsize) : fontsize_(fontsize) {
    fontpath_ = PathLookup_(fontname);
  	InitCache_();
  }

  void PaintChar(const char c, RGBCanvas* canvas, Pen* pen, unsigned char r, unsigned char g, unsigned char b);
  void PaintString(const char* text, RGBCanvas* canvas, Pen* pen, unsigned char r, unsigned char g, unsigned char b);
  unsigned GetWidth(const char* text);
  unsigned GetWidth(const char c);
  unsigned GetHeight(const char* text); /* Note that this returns the height from the baseline */
  unsigned GetHeight(const char c); /* This returns the height irrespective of the baseline */

private:
  FT_Library library_;
  FT_Face face_;
  RenderedChar char_cache_[256];
  const char* fontpath_;
  FT_UInt fontsize_;

  char* PathLookup_(const char* fontname);
  unsigned char* UnpackMonoBitmap_(FT_Bitmap* bitmap);
  void RenderAndCache_(unsigned char val);
  void InitCache_();
};

#endif
