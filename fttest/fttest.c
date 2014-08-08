#include <stdio.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define TARGET_WIDTH 64
#define TARGET_HEIGHT 16

unsigned char image[TARGET_HEIGHT][TARGET_WIDTH];

void print_bitmap(FT_Bitmap* bitmap, FT_Int x, FT_Int y)
{
  FT_Int i, j, p, q;
  FT_Int x_max = x + bitmap->width;
  FT_Int y_max = y + bitmap->rows;

  for (i = x, p = 0; i < x_max; i++, p++) {
    for (j = y, q = 0; j < y_max; j++, q++) {
      if (i < 0 || j < 0 || i >= TARGET_WIDTH || j >= TARGET_HEIGHT) {
      	printf("Character chopped\n");
        continue;
      }

      image[j][i] = bitmap->buffer[q * bitmap->width + p];
    }
  }

  for (i = 0; i < TARGET_HEIGHT; i++) {
  	for (j = 0; j < TARGET_WIDTH; j++) {
  		putchar(image[i][j] == 0 ? ' ' : image[i][j] < 128 ? '+' : '*' );
  	}

  	putchar('\n');
  }
}

int main(int argc, const char* argv[])
{
  FT_Error error;
  FT_Library library;
  FT_Face face;
  FT_UInt glyph_index;
  FT_GlyphSlot slot;

  error = FT_Init_FreeType(&library);
  if (error) {
    fprintf(stderr, "Failed to initialize FreeType: %d\n", error);
    goto fail1;
  }

  error = FT_New_Face(library, "fonts/Oswald-Bold.ttf", 0, &face);
  if (error) {
    fprintf(stderr, "Failed to load font: %d\n", error);
    goto fail2;
  }

  error = FT_Set_Char_Size(face, 0, 15*64, 72, 72);
  if (error) {
    fprintf(stderr, "Failed to set font size: %d\n", error);
    goto fail3;
  }

  glyph_index = FT_Get_Char_Index(face, 'B');
  if (glyph_index == 0) {
  	fprintf(stderr, "Failed to get glyph index\n");
  	goto fail3;
  }

  error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
  if (error) {
    fprintf(stderr, "Failed to load glyph: %d\n", error);
  	goto fail3;
  }

  error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
  if (error) {
    fprintf(stderr, "Failed to render glyph: %d\n", error);
  	goto fail3;
  }

  slot = face->glyph;
  print_bitmap(&slot->bitmap, slot->bitmap_left, TARGET_HEIGHT-slot->bitmap_top);

  FT_Done_Face(face);
  FT_Done_FreeType(library);

  return 0;

  fail3:
  FT_Done_Face(face);

  fail2:
  FT_Done_FreeType(library);

  fail1:
  return -1;
}
