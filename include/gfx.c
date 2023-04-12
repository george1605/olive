#include "gfx.h"
#ifndef RGB
#define RGB(r,g,b) (r | (g << 8) | (b << 16))
#endif
#define GETBYTE(x, y) (x >> (8 * y)) & 0xFF
#define RED RGB(255,0,0)
#define BLUE RGB(0,0,255)
#define GREEN RGB(0,255,0)
#define WHITE RGB(255,255,255)

inline void ol_fastswap(u8* ptr1, u8* ptr2)
{
  *ptr1 ^= *ptr2;
  *ptr2 ^= *ptr1;
  *ptr1 ^= *ptr2; // or xchg ptr1, ptr2
}

EXPORT void ol_swapbuf(OlWindow win)
{
  u32* aux = win.front;
  win.front = win.back;
  win.front = aux;
}

EXPORT void ol_draw(OlWindow win, u8* pixels, u32 off)
{
  if(off > win.w * win.h)
    return;
  memcpy(win.front + off, pixels, win.w * win.h);
}

EXPORT void ol_fill(OlWindow win, u32 color)
{
  for (size_t i = 0; i < win.w * win.h; ++i) {
    win.front[i] = color;
  }
}

EXPORT void ol_setwin(OlWindow win, int x, int y, int color)
{
  int c;
  if(win.mono)
   {
      c = y * win.w + x;
      win.front[c/8] |= 1 << (c%8);
   }
}

EXPORT void ol_setup(int x, int y)
{
  if(x == 0)
    WIDTH = 480;
  else if(y == 0)
    HEIGHT = 320;
  else
    WIDTH = x, HEIGHT = y;
}

EXPORT void ol_set_pixel(u32* ptr, int x, int y, int color)
{
  ptr[y*WIDTH+x] = color;
}

EXPORT void ol_draw_rect(u32* ptr, int x, int y, int w, int h)
{
  for(int i = x;i <= x + w;i++)
    for(int j = y;j <= y + h;j++)
      ptr[j*WIDTH+i] = COLOR;
}

EXPORT void ol_zoom(OlWindow win, int x, int y, int s)
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

EXPORT OlWindow ol_new_win()
{
  OlWindow win;
  win.w = WIDTH;
  win.h = HEIGHT;
  win.mono = 0;
  win.front = ol_alloc(WIDTH * HEIGHT, 32);
  return win;
}

EXPORT void ol_save_file(char* fname, u32* ptr)
{
  FILE* fp = fopen(fname, "wb"); 
  fwrite(ptr, 4, WIDTH*HEIGHT, fp); 
  fclose(fp); 
}

EXPORT int ol_save_ppm(char* fname, OlWindow win)
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

static OlColor blerp(OlColor cols[4], float tx, float ty)
{
  return lerp(lerp(cols[0], cols[1], tx), lerp(cols[2], cols[3], tx), ty);
}

EXPORT OlColor color(int r, int g, int b)
{
  OlColor x;
  x.r = r, x.g = g, x.b = b;
  x.a = 0;
  return x;
}

static int ol_linear( int n1 , int n2 , float perc )
{
    int diff = n2 - n1;

    return n1 + ( diff * perc );
}    

EXPORT void ol_gradient(OlWindow win, OlColor col1, OlColor col2, int width, int height)
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

EXPORT void ol_quad_bezier(OlWindow win, OlPoint points[3])
{
  int xa, xb, ya, yb, x, y;
  for( float i = 0 ; i < 1 ; i += 0.01 )
  {
    // The Green Line
    xa = ol_linear( points[0].x , points[1].x , i );
    ya = ol_linear( points[0].y , points[1].y , i );
    xb = ol_linear( points[1].x , points[2].x , i );
    yb = ol_linear( points[1].y , points[2].y , i );

    // The Black Dot
    x = ol_linear( xa , xb , i );
    y = ol_linear( ya , yb , i );

    win.front[y * win.w + x] = COLOR;
  }
}

void ol_load_ppm(char* fname, OlWindow* win)
{
  FILE* fp = fopen(fname, "r");
  if(fgetc(fp) != 'P' || strcmp(fgetc(fp), "63"))
    return;
  unsigned int _newl = fgetc(fp), c = 0;
  fscanf("%d %d %d\n", &(win->w), &(win->h), &_newl);
  for(int i = 0;i <= win->w * win->h;i++)
  {
    fread(win->front + c, 1, 3, fp);
    c += 3;
  }
  fclose(fp);
}

EXPORT void ol_bgradient(OlWindow win, OlColor cols[4], int width, int height)
{
  OlColor last = blerp(cols, 0, 0);
  for(int i = 1;i <= width;i++)
  {
    for(int j = 1;j <= height;j++)
    {       
        last = blerp(cols, (float)i/(float)width, (float)j/(float)height);
        win.front[j * win.w + i] = RGB(last.r, last.g, last.b);
    }
  }
}

void ol_load_file(char* fname, u32* ptr)
{
  FILE* fp = fopen(fname, "rb");
  fread(ptr, 4, WIDTH*HEIGHT, fp);
  fclose(fp);
}

EXPORT void ol_line_bres(OlWindow win, int x1, int y1, int x2, int y2)
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
