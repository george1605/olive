#include "CImg.h"
#define cimg_display 2
using namespace cimg_library;
 
int main() {
  CImg<unsigned char> visu(500,400,1,3,0);
  const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
  visu.blur(2.5);
  CImgDisplay draw_disp(visu,"De ce nu mergi?");
  draw_disp.render(visu);
  draw_disp.show();
  cimg::wait(2000);
  while (!draw_disp.is_closed()) {
    draw_disp.wait();
    if (draw_disp.button() && draw_disp.mouse_y()>=0) {
      const int y = draw_disp.mouse_y();
      const int x = draw_disp.mouse_x();
    }
    }
  return 0;
}
