#pragma once

extern "C" {
    #include "../core.h" // include the core - no mangling, yeeey
}

namespace ol
{
    class Platform: public OlPlatform
    {
    public:
        Platform() {}
        OlWindow getWindow() {
            OlWindow* win = (OlWindow*)this->video_handle;
            return *win;
        }
    };

    class App: OlApp
    {
    public:
        Platform platform()
        {
            Platform p;
            p.audio_handle = NULL;
            return p;
        }
    };

    class GContext 
    {
    public:
        GContext(){}
    };
}
