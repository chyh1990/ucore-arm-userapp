
## Makefile.am for MiniGUI using the Qt virtual framebuffer video driver

INCLUDES = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			-I$(abs_top_srcdir)/src/newgal/ -I/usr/include/

noinst_LTLIBRARIES = libvideo_goldfish.la
libvideo_goldfish_la_SOURCES = $(VIDEOGOLDFISH_SRCS)

# The Qt virtual FrameBuffer video driver sources
VIDEOGOLDFISH_SRCS = videogoldfish.c videogoldfish.h
