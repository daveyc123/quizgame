#include "text.h"

struct Pixel {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

void RGBCanvas::InitCanvas_()
{
  pixbuf_ = (Pixel*)malloc(width_ * height_ * sizeof(Pixel));
  if (pixbuf_ == NULL) {
    fprintf(stderr, "Failled to allocate memory for the BitmapCanvas\n");
  }

  memset(pixbuf_, 0, (width_ * height_ * sizeof(Pixel)));
}

void RGBCanvas::SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
  Pixel* pixel = NULL;

  if (x >= width_ || x < 0 || y >= height_ || y < 0) {
#ifdef DEBUG
    printf("Pixel (%d, %d) outside canvas\n", x, y);
#endif
    return;
  }

  pixel = &pixbuf_[(y * width_) + x];
  pixel->r = r;
  pixel->g = g;
  pixel->b = b;
}

void RGBCanvas::Clear()
{
  memset(pixbuf_, 0, width_ * height_ * sizeof(*pixbuf_));
}

void RGBCanvas::Display(RGBMatrix* m, int x_offset, int y_offset, bool x_wrap, bool y_wrap)
{
  int i; /* x position on the display */
  int j; /* y position on the display */
  int pix_i; /* x position on the canvas */
  int pix_j; /* y_position on the canvas */

  for (i = 0; i < m->width(); i++) {
    pix_i = i - x_offset;
    if (pix_i < 0 || pix_i >= width_) {
      /* Wrapping */
      if (x_wrap) {
        if (pix_i < 0) {
          pix_i = pix_i + width_;
        } else if (pix_i >= width_) {
          pix_i = pix_i - width_;
        }
      } else {
        continue;
      }
    }

    for (j = 0; j < m->height(); j++) {
      pix_j = j - y_offset;
      if (pix_j < 0 || pix_j >= height_) {
        // /* Wrapping */
        if (y_wrap) {
          if (pix_j < 0) {
            pix_j = pix_j + height_;
          } else if (pix_j >= height_) {
            pix_j = pix_j - height_;
          }
        } else {
          continue;
        }
      }

      pix_j %= height_;
      pix_i %= width_;
      Pixel* pixel = &pixbuf_[pix_j * width_ + pix_i];
      m->SetPixel(i, j, pixel->r, pixel->g, pixel->b);
    }
  }
}

void Font::PaintChar(char c, RGBCanvas* canvas, int pen_x, int pen_y, unsigned char r, unsigned char g, unsigned char b)
{
    RenderedChar* rchar = &char_cache_[(unsigned char)c];
    FT_Int i, j, p, q;
    FT_Int x = pen_x + rchar->bitmap_left;
    FT_Int y = pen_y - rchar->bitmap_top;
    FT_Int x_max = x + rchar->width;
    FT_Int y_max = y + rchar->rows;
    unsigned char char_pixel;

// #ifdef DEBUG
//     printf("DEBUG: pen_x = %d, pen_y = %d, x = %d, y = %d\n", pen_x, pen_y, x, y);
// #endif

    for (i = x, p = 0; i < x_max; i++, p++) {
      for (j = y, q = 0; j < y_max; j++, q++) {
        if (i < 0 || j < 0 || i >= canvas->width() || j >= canvas->height()) {
// #ifdef DEBUG
//           printf("Character chopped\n");
// #endif
          continue;
        }

        char_pixel = rchar->buf[q * rchar->width + p];
        canvas->SetPixel(i, j, char_pixel ? r : 0, char_pixel ? g : 0, char_pixel ? b : 0);
      }
    }

    canvas->pen.x = pen_x + rchar->advance_x;
    canvas->pen.y = pen_y + rchar->advance_y;
}

void Font::PaintString(const char* text, RGBCanvas* canvas, int pen_x, int pen_y, unsigned char r, unsigned char g, unsigned char b)
{
  char c;

  if (text == NULL) {
    return;
  }

  canvas->pen.x = pen_x;
  canvas->pen.y = pen_y;
  while ((c = *text++)) {
    PaintChar(c, canvas, canvas->pen.x, canvas->pen.y, r, g, b);
  }
}

unsigned Font::GetWidth(const char* text)
{
  char c;
  unsigned width = 0;
  RenderedChar* rchar;

  if (text == NULL) {
    return 0;
  }

  while ((c = *text++)) {
    rchar = &char_cache_[(unsigned char)c];
    width += rchar->advance_x;
  }

  return width;
}

unsigned Font::GetWidth(const char c)
{
  return char_cache_[(unsigned char)c].advance_x;
}

unsigned char* Font::UnpackMonoBitmap_(FT_Bitmap* bitmap)
{
  unsigned char* result;
  int y, byte_index, num_bits_done, rowstart, bits, bit_index;
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

void Font::RenderAndCache_(unsigned char val)
{
  FT_UInt glyph_index;
  FT_GlyphSlot slot;
  FT_Bitmap* bitmap;
  FT_Error error;

  // printf("Rendering %d (%c)\n", val, val);

  glyph_index = FT_Get_Char_Index(face_, val);
  if (glyph_index == 0) {
    // Reasonably common scenario, not worth printing an error
    //fprintf(stderr, "Failed to get glyph index for glyph '%d'\n", val);
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

// #ifdef DEBUG
//   printf("DEBUG: Rendered %d '%c': width=%d, rows=%d, pitch=%d\n", val, val, bitmap->width, bitmap->rows, bitmap->pitch);
// #endif

}

void Font::InitCache_()
{
  unsigned i;
  FT_Error error;

  error = FT_Init_FreeType(&library_);
  if (error) {
    fprintf(stderr, "Failed to initialize FreeType: %d\n", error);
    goto fail1;
  }

  error = FT_New_Face(library_, fontpath_, 0, &face_);
  if (error) {
    fprintf(stderr, "Failed to load font '%s': %d\n", fontpath_, error);
    goto fail2;
  }

  error = FT_Set_Char_Size(face_, 0, fontsize_*64, 72, 72);
  if (error) {
    fprintf(stderr, "Failed to set font size '%u': %d\n", fontsize_, error);
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
