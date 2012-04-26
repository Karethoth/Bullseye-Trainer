#include "messagebox.h"


extern void DrawText( SDL_Surface*, TTF_Font*, int, int, char*, SDL_Color*, int );


void DrawMessageBox( SDL_Surface *screen,
                     TTF_Font    *font,
                     sMessageBox *msgBox,
                     SDL_Color   *textColor )
{
  if( msgBox->lines[0] )
  {
    DrawText( screen, font, 10, 10, msgBox->lines[0], textColor, 0 );
  }

  if( msgBox->lines[1] )
  {
    DrawText( screen, font, 10, 24, msgBox->lines[1], textColor, 0 );
  }
}

