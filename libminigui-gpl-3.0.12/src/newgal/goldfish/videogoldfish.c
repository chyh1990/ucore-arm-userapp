/*
 * =====================================================================================
 *
 *       Filename:  videogoldfish.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/31/2012 10:54:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include "common.h"

#include "minigui.h"
#include "misc.h"
#include "newgal.h"
#include "sysvideo.h"
#include "pixels_c.h"
#include "videogoldfish.h"


/* Initialization/Query functions */
static int GoldfishFB_VideoInit(_THIS, GAL_PixelFormat *vformat);
static GAL_Rect **GoldfishFB_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags);
static GAL_Surface *GoldfishFB_SetVideoMode(_THIS, GAL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int GoldfishFB_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors);
static void GoldfishFB_VideoQuit(_THIS);

/* Hardware surface functions */
static int GoldfishFB_AllocHWSurface(_THIS, GAL_Surface *surface);
static void GoldfishFB_FreeHWSurface(_THIS, GAL_Surface *surface);


/* GoldfishFB driver bootstrap functions */
static int GoldfishFB_Available (void)
{
  int console;
  const char *GAL_fbdev;

  GAL_fbdev = getenv("FRAMEBUFFER");
  if ( GAL_fbdev == NULL ) {
    GAL_fbdev = "fb0:";
  }
  console = open(GAL_fbdev, O_RDWR, 0);
  if ( console >= 0 ) {
    printf("NEWGAL: %s found!\n", GAL_fbdev);
    setenv("FRAMEBUFFER", GAL_fbdev, 1);
    close(console);
  }
  return(console >= 0);
}

static void GoldfishFB_DeleteDevice(GAL_VideoDevice *device)
{
  free(device->hidden);
  free(device);
}


static GAL_VideoDevice *GoldfishFB_CreateDevice (int devindex)
{
  GAL_VideoDevice *this;
  /* Initialize all variables that we clean on shutdown */
  this = (GAL_VideoDevice *)malloc(sizeof(GAL_VideoDevice));
  if ( this ) {
    memset(this, 0, (sizeof *this));
    this->hidden = (struct GAL_PrivateVideoData *)
      malloc((sizeof *this->hidden));
  }
  if ( (this == NULL) || (this->hidden == NULL) ) {
    GAL_OutOfMemory();
    if ( this ) {
      free(this);
    }
    return(0);
  }
  memset(this->hidden, 0, (sizeof *this->hidden));

  this->VideoInit = GoldfishFB_VideoInit;
  this->ListModes = GoldfishFB_ListModes;
  this->SetVideoMode = GoldfishFB_SetVideoMode;
  this->SetColors = GoldfishFB_SetColors;
  this->VideoQuit = GoldfishFB_VideoQuit;
  //this->GetFBInfo = GoldfishFB_GetFBInfo;
  this->free = GoldfishFB_DeleteDevice;

  this->AllocHWSurface = GoldfishFB_AllocHWSurface;
  this->FreeHWSurface = GoldfishFB_FreeHWSurface;

  printf("TODO GoldfishFB_CreateDevice\n");
  return this;
}

static GAL_Rect **GoldfishFB_ListModes(_THIS, GAL_PixelFormat *format, Uint32 flags)
{
    return (GAL_Rect**) -1;
}

VideoBootStrap GOLDFISH_bootstrap = {
  "goldfishfb", "Goldfish FrameBuffer",
  GoldfishFB_Available, GoldfishFB_CreateDevice
};

static int GoldfishFB_VideoInit (_THIS, GAL_PixelFormat *vformat)
{
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
    const char *fbdev = getenv("FRAMEBUFFER");
    struct GAL_PrivateVideoData* data = this->hidden;
    if(fbdev == NULL){
      fbdev = "fb0";
    }
    int console_fd = open(fbdev, O_RDWR, 0);
    if ( console_fd < 0 ) {
      GAL_SetError("NEWGAL>FBCON: Unable to open %s\n", fbdev);
      return(-1);
    }
    data->fd = console_fd;

    /* Get the type of video hardware */
    if ( ioctl(console_fd, FBIOGET_FSCREENINFO, &finfo) < 0 ) {
        GAL_SetError("NEWGAL>GOLDFISHFB: Couldn't get console hardware info\n");
        GoldfishFB_VideoQuit(this);
        return(-1);
    }
    switch (finfo.type) {
        case FB_TYPE_PACKED_PIXELS:
            /* Supported, no worries.. */
            break;
        default:
            GAL_SetError("NEWGAL>FBCON: Unsupported console hardware\n");
            GoldfishFB_VideoQuit(this);
            return(-1);
    }
    if ( ioctl(console_fd, FBIOGET_VSCREENINFO, &vinfo) < 0 ) {
        GAL_SetError("NEWGAL>GOLDFISHFB: Couldn't get console pixel format\n");
        GoldfishFB_VideoQuit(this);
        return(-1);
    }
    vformat->BitsPerPixel = vinfo.bits_per_pixel;
    data->bpp = vinfo.bits_per_pixel;
    data->w = vinfo.xres;
    data->h = vinfo.yres;
    data->line_length = finfo.line_length;
    int memlen= data->w * data->h * vinfo.bits_per_pixel/8;
    void *mappedmem = mmap(NULL, memlen,
                            PROT_READ|PROT_WRITE, MAP_PRIVATE, console_fd, 0);
    if(mappedmem == (void*)-1){
        GAL_SetError("NEWGAL>GoldfishFB: Unable to memory map the video hardware\n");
        GoldfishFB_VideoQuit(this);
        return(-1);
    }
    data->buf = (unsigned char*)mappedmem;

    printf("NEWGAL>GOLDFISHFB: HW: %dx%d-%dbpp, line_length: %d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, data->line_length);
    switch (vformat->BitsPerPixel) {
#ifdef _MGGAL_SHADOW
        case 1:
            break;
        case 4:
            break;
#endif
        case 8:
            vformat->BytesPerPixel = 1;
            break;
        case 12:
            vformat->BitsPerPixel = 16;
            vformat->BytesPerPixel = 2;
            vformat->Rmask = 0x00000F00;
            vformat->Gmask = 0x000000F0;
            vformat->Bmask = 0x0000000F;
            break;
        case 16:
            vformat->BytesPerPixel = 2;
            vformat->Rmask = 0x0000F800;
            vformat->Gmask = 0x000007E0;
            vformat->Bmask = 0x0000001F;
            break;
        case 32:
            vformat->BytesPerPixel = 4;
            vformat->Amask = 0xFF000000;
            vformat->Rmask = 0x00FF0000;
            vformat->Gmask = 0x0000FF00;
            vformat->Bmask = 0x000000FF;
            break;
        default:
            GAL_SetError ("NEWGAL>GoldfishFB: Not supported depth: %d, "
                "please try to use Shadow NEWGAL engine with targetname qvfb.\n", vformat->BitsPerPixel);
            return -1;
    }

    return 0;
}

static GAL_Surface *GoldfishFB_SetVideoMode (_THIS, GAL_Surface *current,
                                int width, int height, int bpp, Uint32 flags)
{
  struct GAL_PrivateVideoData* data = this->hidden;
  printf("TODO GoldfishFB_SetVideoMode %dx%d-%d\n", width, height, bpp);
  if(data->w != width || data->h != height || data->bpp != bpp){
      GAL_SetError ("NEWGAL>GoldfishFB: Not supported Mode. ");
      return NULL;
  }
  /* Set up the mode framebuffer */
  current->flags = GAL_HWSURFACE | GAL_FULLSCREEN;
  current->w = data->w;
  current->h = data->h;
  current->pitch = data->line_length;
  current->pixels = data->buf;

  return current;
}

static void GoldfishFB_VideoQuit (_THIS)
{
  printf("TODO GoldfishFB_VideoQuit\n");
}

/* We don't actually allow hardware surfaces other than the main one */
static int GoldfishFB_AllocHWSurface(_THIS, GAL_Surface *surface)
{
    return -1;
}

static void GoldfishFB_FreeHWSurface(_THIS, GAL_Surface *surface)
{
    surface->pixels = NULL;
}

static int GoldfishFB_SetColors(_THIS, int firstcolor, int ncolors, GAL_Color *colors)
{
  printf("TODO GoldfishFB_SetColors\n");
  return 1;
}
