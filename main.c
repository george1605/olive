#define _REDEF_MAIN_
#include "core.h"

void __main(OlPlatform platform, OlArgs args)
{
    OlWindow* win = (OlWindow*)platform.video_handle;
    ol_fill(*win, RED);
    ol_save_ppm("wow.ppm", *win); 
    free(win->front);
    free(win);  
}

void __setup(OlPlatform* platform)
{
    ol_setupvid_platform(platform, 500, 400);
}