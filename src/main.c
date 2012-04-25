#include "main.h"

#define WIDTH 640
#define HEIGHT 480
#define DEPTH 32
#define BPP 6

sPlane plane = { 35, 0, 90 };


void DrawText( SDL_Surface *surface, TTF_Font *font, int x, int y, char *text, SDL_Color *textColor )
{
  SDL_Rect     textLoc;
  SDL_Surface *textSurf;

  if( !(textSurf=TTF_RenderText_Solid( font, text, *textColor )) )
  {
    printf( "TTF_RenderText: %s\n", TTF_GetError() );
  }
  else
  {
    textLoc.x = x-textSurf->w/2;
    textLoc.y = y-textSurf->h/2;
    SDL_BlitSurface( textSurf, NULL, surface, &textLoc );
    SDL_FreeSurface( textSurf );
  }
}


void DrawBullseyeMap( SDL_Surface *screen, TTF_Font *font )
{
  SDL_Color    textColor = { 0, 255, 0 };

  int centerX=WIDTH/2,
      centerY=HEIGHT/2;

  float ratio = (float)HEIGHT/200;

  Draw_Circle( screen, centerX, centerY, ratio*60, 0xffa0a0a0 );
  Draw_Circle( screen, centerX, centerY, ratio*30, 0xffa0a0a0 );
  Draw_Circle( screen, centerX, centerY, ratio*10, 0xffa0a0a0 );

  Draw_VLine( screen, centerX, centerY-ratio*90, centerY+ratio*90, 0xffa0a0a0 );
  Draw_HLine( screen, centerX-ratio*90, centerY, centerX+ratio*90, 0xffa0a0a0 );

  DrawText( screen, font, centerX, centerY-ratio*60, "60nm", &textColor );
  DrawText( screen, font, centerX, centerY-ratio*30, "30nm", &textColor );
  DrawText( screen, font, centerX, centerY-ratio*10, "10nm", &textColor );

  DrawText( screen, font, centerX+ratio*90, centerY, "090", &textColor );
  DrawText( screen, font, centerX-ratio*90, centerY, "270", &textColor );
  DrawText( screen, font, centerX, centerY-ratio*90, "000", &textColor );
  DrawText( screen, font, centerX, centerY+ratio*90, "180", &textColor );


  // Draw The Plane


  sVector planeVector = { 0, plane.distance * ratio };
  sVector planeLocVector = RotateVector( planeVector, plane.bearing );

  sVector planeTailVector = { 0, -10 };
  sVector planeTailLocVector = RotateVector( planeTailVector, plane.heading );
  sVector planeArr1Vector = { 0, -5 };
  sVector planeArr1LocVector = RotateVector( planeArr1Vector, plane.heading-45 );
  sVector planeArr2Vector = { 0, -5 };
  sVector planeArr2LocVector = RotateVector( planeArr2Vector, plane.heading+45 );

  planeLocVector.x -= planeTailLocVector.x;
  planeLocVector.y -= planeTailLocVector.y;

  Draw_Line( screen,
            centerX-planeLocVector.x,
            centerY-planeLocVector.y,
            centerX-planeLocVector.x-planeTailLocVector.x,
            centerY-planeLocVector.y-planeTailLocVector.y,
            0xff00ffff );
  Draw_Line( screen,
            centerX-planeLocVector.x,
            centerY-planeLocVector.y,
            centerX-planeLocVector.x-planeArr1LocVector.x,
            centerY-planeLocVector.y-planeArr1LocVector.y,
            0xff00ffff );
  Draw_Line( screen,
            centerX-planeLocVector.x,
            centerY-planeLocVector.y,
            centerX-planeLocVector.x-planeArr2LocVector.x,
            centerY-planeLocVector.y-planeArr2LocVector.y,
            0xff00ffff );
}


void DrawBullseyeMarker( SDL_Surface *screen, TTF_Font *font )
{
  // TEXT COLOR
  SDL_Color    textColor = { 0, 255, 0 };

  // DRAW NAUTICAL MILES TEXT
  char nmBuf[4];
  sprintf( nmBuf, "%d", (int)plane.distance );
  DrawText( screen, font, 30, HEIGHT-40, nmBuf, &textColor );

  // DRAW BEARING TEXT
  char bearBuf[4];
  sprintf( bearBuf, "%d", (int)plane.bearing );
  if( bearBuf[1] == 0 )
  {
    bearBuf[3] = 0;
    bearBuf[2] = 0;
    bearBuf[1] = bearBuf[0];
    bearBuf[0] = '0';
  }
  if( bearBuf[2] == 0 )
  {
    bearBuf[3] = 0;
    bearBuf[2] = bearBuf[1];
    bearBuf[1] = bearBuf[0];
    bearBuf[0] = '0';
  }
  DrawText( screen, font, 30, HEIGHT-10, bearBuf, &textColor );

  // DRAW CIRCLE
  Draw_Circle( screen, 30, HEIGHT-40, 20, 0xff00ff00 );

  // DRAW BULLSEYE POINTER
  sVector pointer = { 0, 20 };
  sVector pointerLoc = RotateVector( pointer, -plane.heading+plane.bearing );
  Draw_Circle( screen, 30+pointerLoc.x, HEIGHT-40+pointerLoc.y, 2, 0xff00ff00 );
}


void DrawScreen( SDL_Surface *screen, TTF_Font *font )
{ 
  SDL_FillRect( screen, 0, 0 );

  DrawBullseyeMarker( screen, font );
  DrawBullseyeMap( screen, font );

  SDL_Flip( screen ); 
}


int main( int argc, char* argv[] )
{
  SDL_Surface *screen;
  SDL_Event    event;

  int keypress = 0;

  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) return 1;
  if( TTF_Init() < 0 ) return 1;

  if( !(screen = SDL_SetVideoMode( WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE )) )
  {
    SDL_Quit();
    return 1;
  }

  TTF_Font *font;
  font = TTF_OpenFont( "/usr/share/fonts/truetype/ttf-droid/DroidSansMono.ttf", 12 );
  if( !font )
  {
    printf( "TTF_OpenFont: %s\n", TTF_GetError() );
    return 1;
  }

  while( !keypress )
  {
    while( SDL_PollEvent( &event ) )
    {
      switch( event.type )
      {
        case SDL_QUIT:
          keypress = 1;
          break;
        case SDL_KEYDOWN:
          keypress = 1;
          break;
      }
    }

    if( plane.bearing >= 360 )
      plane.bearing = (int)plane.bearing%360;
    if( plane.heading >= 360 )
      plane.heading = (int)plane.heading%360;

    plane.bearing += 0.1;
    plane.heading += 0.1;
    DrawScreen( screen, font );
  }

  SDL_Quit();

  return 0;
}

