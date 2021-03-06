#include "text.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

void RGBCanvas::Fill(unsigned char r, unsigned char g, unsigned char b)
{
  int i;
  int j;
  Pixel* pixel = NULL;

  for (i = 0; i < width_; i++) {
    for (j = 0; j < height_; j++) {
      pixel = &pixbuf_[(j * width_) + i];
      pixel->r = r;
      pixel->g = g;
      pixel->b = b;
    }
  }
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

void Font::PaintChar(char c, RGBCanvas* canvas, Pen* pen, unsigned char r, unsigned char g, unsigned char b)
{
    FT_Int i, j, p, q;
    FT_Int x, y, x_max, y_max;
    unsigned char char_pixel;
    RenderedChar* rchar = &char_cache_[(unsigned char)c];

    if (canvas == NULL) {
      return;
    }

    if (pen == NULL) {
      pen = &canvas->pen;
    }

    x = pen->x + rchar->bitmap_left;
    y = pen->y - rchar->bitmap_top;
    x_max = x + rchar->width;
    y_max = y + rchar->rows;

// #ifdef DEBUG
//     printf("DEBUG: pen->x = %d, pen->y = %d, x = %d, y = %d\n", pen->x, pen->y, x, y);
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
        if (char_pixel) {
          canvas->SetPixel(i, j, r, g, b);
        }
      }
    }

    canvas->pen.x = pen->x + rchar->advance_x;
    canvas->pen.y = pen->y + rchar->advance_y;
}

void Font::PaintString(const char* text, RGBCanvas* canvas, Pen* pen, unsigned char r, unsigned char g, unsigned char b)
{
  char c;

  if (text == NULL || canvas == NULL) {
    return;
  }

  if (pen != NULL) {
    canvas->pen.x = pen->x;
    canvas->pen.y = pen->y;
  }

  while ((c = *text++)) {
    PaintChar(c, canvas, NULL, r, g, b);
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

unsigned Font::GetHeight(const char* text)
{
  char c;
  RenderedChar* rchar;
  unsigned height = 0;
  unsigned max_height = 0;

  if (text == NULL) {
    return 0;
  }

  while ((c = *text++)) {
    rchar = &char_cache_[(unsigned char)c];
    height = rchar->rows - rchar->bitmap_top;
    if (height > max_height) {
      max_height = height;
    }
  }

  return max_height;
}

unsigned Font::GetHeight(const char c)
{
  return char_cache_[(unsigned char)c].rows;
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

char* Font::PathLookup_(const char* fontname)
{
  const char* lookup[] = {
    "fonts",
    "/usr/local/share/fonts",
    "/usr/share/fonts"
  };
  int i, rc;
  char path[256];
  struct stat statbuf;

  for (i = 0; i < sizeof(lookup) / sizeof(*lookup); i++) {
    snprintf(path, sizeof(path), "%s/%s", lookup[i], fontname);
    rc = stat(path, &statbuf);
    if (rc == 0) {
#ifdef DEBUG
      printf("Found font: %s\n", path);
#endif
      return strdup(path);
    } else {
#ifdef DEBUG
      printf("No font: %s\n", path);
#endif
      continue;
    }
  }

  fprintf(stderr, "Couldn't find font: %s\n", fontname);
  return NULL;
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
