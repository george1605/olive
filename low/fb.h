#ifndef __FRAMEBUFFER__
#define __FRAMEBUFFER__

#ifndef __linux__
#error Linux Only!
#endif

#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include "../gfx.c"

int ol_open_fb(const char* fbdev, fb_var_screeninfo* info)
{
    int fbfd = open (fbdev, O_RDWR);
    if(fbfd < 0) return;
    ioctl (fbfd, FBIOGET_VSCREENINFO, info);
    return fbfd;
}

void* ol_map_fb(int fb, fb_var_screeninfo* info)
{
    size_t size = info->xres * info->yres * (info->bits_per_pixel/8);
    return mmap(0, size, 
        PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0)
}

void* ol_init_fb(int* _fd)
{
    fb_var_screeninfo p;
    int fd = ol_open_fb("/dev/fb0", &p);
    *_fd = fd;
    return ol_map_fb(fd, &p);
}

void ol_dump_fb(OlWindow win)
{
    
}

#endif