#ifndef _GAL_goldfish_h
#define _GAL_goldfish_h


/* Hidden "this" pointer for the video functions */
#define _THIS	GAL_VideoDevice *this


/* Private display data */
struct GAL_PrivateVideoData {
    unsigned char* buf;
    int w;
    int h;
    int bpp;
    int line_length;
    int fd;
};

#endif
