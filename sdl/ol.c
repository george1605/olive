#include "../gfx.c"
#include <SDL2/SDL.h>
#undef main
#define OL_WIN_OPENGL SDL_WINDOW_OPENGL
#define OL_WIN_BASE SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS

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
    u32* buf; // pixel buffer
} OlPanel;

OlPanel ol_sdl_new(const char* title, int w, int h, u32 flags)
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

int main()
{
    ol_sdl_init();
    OlPanel p = ol_sdl_new("Window", 1000, 1000, 0);
    while(1);
}

