#include "led-matrix.h"

#include <ft2build.h>
#include FT_FREETYPE_H

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

  void Display(RGBMatrix* m);
  void SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
  unsigned width() { return width_; };
  unsigned height() { return height_; };
  Pen last_pen;

private:
  Pixel* pixbuf_;
  unsigned width_;
  unsigned height_;

  void InitCanvas_();

};

class Font {
public:
  Font(char* fontpath, const FT_UInt fontsize) : fontpath_(fontpath), fontsize_(fontsize) {
  	InitCache_();
  }

  void PaintChar(RenderedChar* rchar, RGBCanvas* canvas, int pen_x, int pen_y, unsigned char r, unsigned char g, unsigned char b);
  void PaintString(const char* text, RGBCanvas* canvas, int pen_x, int pen_y, unsigned char r, unsigned char g, unsigned char b);

private:
  FT_Library library_;
  FT_Face face_;
  RenderedChar char_cache_[256];
  char* fontpath_;
  FT_UInt fontsize_;

  unsigned char* UnpackMonoBitmap_(FT_Bitmap* bitmap);
  void RenderAndCache_(unsigned char val);
  void InitCache_();
};
