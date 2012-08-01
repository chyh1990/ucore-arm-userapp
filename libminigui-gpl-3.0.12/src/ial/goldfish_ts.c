/*
 ** goldfish.c: Low Level Input Engine for GOLDFISH
 **         This driver can run on Linux.
 ** 
 ** Copyright (C) 2003 ~ 2007 Feynman Software.
 **
 ** So this IAL engine can be a good template of your new IAL engines,
 ** which compliant to the specification.
 **
 ** Created by Libo Jiao, 2001/08/20
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _MGIAL_GOLDFISH

#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>

#include <minigui.h>

#include "ial.h"

static unsigned char state [NR_KEYS];
static int btn_fd = -1;

static int mousex = 0;
static int mousey = 0;
static int mousebtn = 0;

static int goldfish_mouse_update(void)
{
	return 1;
}

static void goldfish_mouse_getxy(int *x, int *y){
	*x = mousex;
	*y = mousey;
}

static int goldfish_mouse_getbutton(void){
  return mousebtn?IAL_MOUSE_LEFTBUTTON:0;
}

static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
        struct timeval *timeout)
{
  int to_msec = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
  int rd, value, size = sizeof (struct input_event);
  struct input_event ev[64];
  int i, updated = 0;
  if ((rd = read (btn_fd, ev, size * 64)) < 0)
    perror("read()");
  int cnt = rd/sizeof(struct input_event);
  if(cnt < 2)
    goto check_rel;
  for(i=0;i<cnt;i+=2){
    if(ev[i].type == EV_ABS && ev[i+1].type == EV_ABS){
      mousex = ev[i].value;
      mousey = ev[i+1].value;
      updated = 1;
    }
  }
check_rel:
  for(i=0;i<cnt;i++){
    if(ev[i].type == EV_KEY && ev[i].code == BTN_TOUCH){
      mousebtn = ev[i].value;
      updated = 1;
    }
  }
  if(updated){
    return IAL_MOUSEEVENT;
  }
out:
  __mg_os_time_delay (to_msec);
  return 0;
}

static int keyboard_update(void)
{

  return 0;
}

static const char * keyboard_get_state (void)
{
  return NULL;
}

BOOL InitGOLDFISHInput (INPUT* input, const char* mdev, const char* mtype)
{
  printf("IAL: init Goldfish Touchscreen\n");
  btn_fd = open ("event0:", O_RDONLY);
  if (btn_fd < 0 ) {
    fprintf (stderr, "GOLDFISH: Can not open keyboard!\n");
    return FALSE;
  }

  input->update_mouse = goldfish_mouse_update;
  input->get_mouse_xy = goldfish_mouse_getxy;
  input->set_mouse_xy = NULL;
  input->get_mouse_button = goldfish_mouse_getbutton;
  input->set_mouse_range = NULL;

  input->update_keyboard = keyboard_update;
  input->get_keyboard_state = keyboard_get_state;
  input->set_leds = NULL;

  input->wait_event = wait_event;

  return TRUE;
}

void TermGOLDFISHInput (void)
{
  if (btn_fd >= 0)
    close(btn_fd);
}

#endif
