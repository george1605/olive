#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#ifndef RGB
#define RGB(r,g,b) (r | (g << 8) | (b << 16))
#endif
#define RED RGB(255,0,0)
#define BLUE RGB(0,0,255)
#define GREEN RGB(0,255,0)
#define WHITE RGB(255,255,255)
typedef unsigned char u8;
typedef unsigned int u32;
static int WIDTH, HEIGHT, COLOR = RED;

inline void ol_fastswap(u8* ptr1, u8* ptr2)
{
  *ptr1 ^= *ptr2;
  *ptr2 ^= *ptr1;
  *ptr1 ^= *ptr2; // or xchg ptr1, ptr2
}

typedef struct {
  u32* front, back;
  u32 w, h;
  int mono; // if is monochrome
} OlWindow;

typedef struct {
  u32* img;
  int w, h;
} OlGlyph;

typedef struct {
  void(*click)(int x, int y);
  void(*close)();
} OlEvents; // event handlers

void ol_swapbuf(OlWindow win)
{
  u32* aux = win.front;
  win.front = win.back;
  win.front = aux;
}

void ol_draw(OlWindow win, u8* pixels, u32 off)
{
  if(off > win.w * win.h)
    return;
  memcpy(win.front + off, pixels, win.w * win.h);
}

void ol_fill(OlWindow win, u32 color)
{
  for (size_t i = 0; i < win.w * win.h; ++i) {
    win.front[i] = color;
  }
}

void ol_setwin(OlWindow win, int x, int y, int color)
{
  int c;
  if(win.mono)
   {
      c = y * win.w + x;
      win.front[c/8] |= 1 << (c%8);
   }
}

void ol_setup(int x, int y)
{
  if(x == 0)
    WIDTH = 480;
  else if(y == 0)
    HEIGHT = 320;
  else
    WIDTH = x, HEIGHT = y;
}

void ol_set_pixel(u32* ptr, int x, int y, int color)
{
  ptr[y*WIDTH+x] = color;
}

void ol_draw_rect(u32* ptr, int x, int y, int w, int h)
{
  for(int i = x;i <= x + w;i++)
    for(int j = y;j <= y + h;j++)
      ptr[j*WIDTH+i] = COLOR;
}

void ol_zoom(OlWindow win, int x, int y, int s)
{
  int color = win.front[y * win.w + x];
  for(int i = x;i <= x+s;i++)
    for(int j = y;j <= y+s;j++)
      win.front[j * win.w + i] = color;
}

u8* ol_alloc(size_t size, int bpm)
{
  if(size == 0)
    return malloc(WIDTH * HEIGHT * (bpm/8));
  return malloc(size * (bpm/8));
}

OlWindow ol_new_win()
{
  OlWindow win;
  win.w = WIDTH;
  win.h = HEIGHT;
  win.mono = 0;
  win.front = ol_alloc(WIDTH * HEIGHT, 32);
  return win;
}

void ol_save_file(char* fname, u32* ptr)
{
  FILE* fp = fopen(fname, "wb"); 
  fwrite(ptr, 4, WIDTH*HEIGHT, fp); 
  fclose(fp); 
}

int ol_save_ppm(char* fname, OlWindow win)
{
  FILE *fp = fopen(fname, "wb");
  fprintf(fp, "P6\n%zu %zu 255\n", win.w, win.h);
  int res = 0;
  for (size_t i = 0; i < win.w * win.h; ++i)
  {
    uint32_t pixel = win.front[i];
    uint8_t bytes[3] = {
        (pixel >> (8 * 0)) & 0xFF,
        (pixel >> (8 * 1)) & 0xFF,
        (pixel >> (8 * 2)) & 0xFF,
    };
    fwrite(bytes, sizeof(bytes), 1, fp);
    if (ferror(fp))
    {
      res = errno;
      goto err;
    }
  }
err:
  if(fp) fclose(fp);
  return res;
}

typedef struct {
  u8 r, g, b, a;
} OlColor;

/* Linear Interpolation
 * Used for Gradients
 */
static OlColor lerp(OlColor col1, OlColor col2, float pos)
{
  OlColor t;
  t.r = col1.r + (col2.r - col1.r) * pos;
  t.g = col1.g + (col2.g - col1.g) * pos;
  t.b = col1.b + (col2.b - col1.b) * pos;
  return t;
}

OlColor color(int r, int g, int b)
{
  OlColor x;
  x.r = r, x.g = g, x.b = b;
  x.a = 0;
  return x;
}

void ol_gradient(OlWindow win, OlColor col1, OlColor col2, int width, int height)
{
  OlColor last = lerp(col1, col2, 0);
  for(int i = 1;i <= width;i++)
  {
    for(int j = 1;j <= height;j++)
    {
        win.front[j * win.w + i] = RGB(last.r, last.g, last.b);
    }
    last = lerp(col1, col2, (float)i/(float)width);
  }
}

void ol_load_file(char* fname, u32* ptr)
{
  FILE* fp = fopen(fname, "rb");
  fread(ptr, 4, WIDTH*HEIGHT, fp);
  fclose(fp);
}

void ol_line_bres(OlWindow win, int x1, int y1, int x2, int y2)
{
  int dx, dy, p, x, y;

  dx = x2 - x1;
  dy = y2 - y1;

  x = x1;
  y = y1;

  p = 2 * dy - dx;

  while (x < x2)
  {
    if (p >= 0)
    {
      win.front[y * win.w + x] = COLOR;
      y = y + 1;
      p = p + 2 * dy - 2 * dx;
    }
    else
    {
      win.front[y * win.w + x] = COLOR;
      p = p + 2 * dy;
    }
    x = x + 1;
  }
}

typedef struct { uint16_t x, y; } OlPoint;
void ol_triangle(OlWindow win, OlPoint p1, OlPoint p2, OlPoint p3)
{
  ol_line_bres(win, p1.x, p1.y, p2.x, p2.y);
  ol_line_bres(win, p2.x, p2.y, p3.x, p3.y);
  ol_line_bres(win, p3.x, p3.y, p1.x, p1.y);
}

OlGlyph ol_load_glyph(char* fname, int w, int h)
{
  OlGlyph p;
  p.w = w, p.h = h, p.img = ol_alloc(w*h, 32);
  ol_load_file(fname, p.img);
  return p;
}

void ol_draw_circle(OlWindow win, int x, int y, int r)
{
  for(int i = 1;i <= 359;i+=3)
    win.front[(int)(r*cos(i)*win.w + r*sin(i))] = 1;
}

void ol_assign(FILE* fp, u8* ptr)
{
  setbuf(fp, ptr);
}
