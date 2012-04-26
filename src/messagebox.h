#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>


typedef struct {
  char *lines[2];
} sMessageBox;


extern void DrawMessageBox( SDL_Surface*, TTF_Font*, sMessageBox*, SDL_Color* );

#endif

