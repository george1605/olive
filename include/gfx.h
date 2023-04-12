#ifndef __GFX__
#define __GFX__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#ifdef EXPORT_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

typedef unsigned char u8;
typedef unsigned int u32;
typedef struct { uint16_t x, y; } OlPoint;
static int WIDTH, HEIGHT, COLOR;

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

typedef struct {
  u8 r, g, b, a;
} OlColor;

EXPORT void ol_swapbuf(OlWindow win);
EXPORT void ol_draw(OlWindow win, u8* pixels, u32 off);
EXPORT void ol_fill(OlWindow win, u32 color);
EXPORT void ol_setwin(OlWindow win, int x, int y, int color);
EXPORT void ol_setup(int x, int y);
EXPORT void ol_set_pixel(u32* ptr, int x, int y, int color);
EXPORT void ol_draw_rect(u32* ptr, int x, int y, int w, int h);
EXPORT void ol_zoom(OlWindow win, int x, int y, int s);
EXPORT OlWindow ol_new_win();
EXPORT OlColor color(int r, int g, int b);
EXPORT void ol_save_file(char* fname, u32* ptr);
EXPORT int ol_save_ppm(char* fname, OlWindow win);
EXPORT void ol_gradient(OlWindow win, OlColor col1, OlColor col2, int width, int height);
EXPORT void ol_quad_bezier(OlWindow win, OlPoint points[3]);
EXPORT void ol_bgradient(OlWindow win, OlColor cols[4], int width, int height);
EXPORT void ol_line_bres(OlWindow win, int x1, int y1, int x2, int y2);

#endif