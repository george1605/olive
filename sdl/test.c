#define _REDEF_MAIN_
#include "include/SDL2/SDL.h"
#include "include/SDL2/SDL_timer.h"
#include "include/SDL2/SDL_syswm.h"
#undef main
#include "../core.h"
#include "ol.h"
#include <stdio.h>

void __main(OlPlatform platform, OlArgs args)
{
    // returns zero on success else non-zero
    SDL_Event ev;
    int running = 1;
    SDL_Window* win = platform.video_handle; // passed by __setup()
    OlPanel p = {.win = win};
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
}

void __setup(OlPlatform* platform)
{
    ol_sdl_init();
    platform->video_handle = ol_sdl_new("My Window", 500, 400, OL_WIN_BASE).win;
}