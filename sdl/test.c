#include "include/SDL2/SDL.h"
#include "include/SDL2/SDL_timer.h"
#include "include/SDL2/SDL_syswm.h"
#include "ol.h"
#include <stdio.h>
#undef main
 
int main(int argc, char *argv[])
{
    // returns zero on success else non-zero
    ol_sdl_init();
    SDL_Event ev;
    int running = 1;
    OlPanel p = ol_sdl_new("GAME", 500, 500, 0);
    SDL_Window* win = p.win;
    OlColor col = {.r = 255, .g = 0, .b = 173};
    SDL_Renderer* rend = ol_sdl_renderer(p);
    ol_sdl_clear(rend, col);
    while(running)
    {
        SDL_PollEvent(&ev);
        if(ev.type == SDL_MOUSEBUTTONDOWN)
        {
            printf("Mouse button clicked %i", ev.button.which);
        }
        if(ev.type == SDL_KEYDOWN || ev.type == SDL_QUIT)
        {
            running = 0;
        }
    }
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}