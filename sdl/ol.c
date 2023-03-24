#ifndef __OL_SDL__
#define __OL_SDL__
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdint.h>
#define OL_WIN_OPENGL SDL_WINDOW_OPENGL
#define OL_WIN_BASE SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED

void ol_sdl_init()
{
    if(!SDL_Init(SDL_INIT_EVERYTHING))
        {
            printf("Could not init.Error=%s", SDL_GetError());
            exit(-1);
        }
}

typedef struct {
    SDL_Window* win;
    uint32_t* buf; // pixel buffer
} OlPanel;

OlPanel ol_sdl_new(const char* title, int w, int h, uint32_t flags)
{
    OlPanel panel;
    panel.win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
    panel.buf = NULL;
    return panel;
}

void ol_sdl_free(OlPanel panel)
{
    SDL_DestroyWindow(panel.win);
    if(panel.buf)
        free(panel.buf);
}

int ol_closed(OlPanel panel)
{
    SDL_Event event;
    SDL_PollEvent(&event);
    return (event.type != SDL_QUIT);
}
#endif