#include "main.h"

#define WIDTH 640
#define HEIGHT 480
#define DEPTH 32
#define BPP 6

#define MARKERSIZE 40
#define MARKERX 60
#define MARKERY HEIGHT-70

sPlane plane = { 35, 0, 90 };
sPlane bandit = { 0, 0, 0 };

sMessageBox msgBox;

enum GameState
{
  GAME,
  DEBRIEFING
};

enum GameState gameState = GAME;


void DrawText( SDL_Surface *surface, TTF_Font *font, int x, int y, char *text, SDL_Color *textColor, int centered )
{
  SDL_Rect     textLoc;
  SDL_Surface *textSurf;

  if( !(textSurf=TTF_RenderText_Solid( font, text, *textColor )) )
  {
    printf( "TTF_RenderText: %s\n", TTF_GetError() );
  }
  else
  {
    if( centered )
    {
      textLoc.x = x-textSurf->w/2;
      textLoc.y = y-textSurf->h/2;
    }
    else
    {
      textLoc.x = x;
      textLoc.y = y;
    }
    SDL_BlitSurface( textSurf, NULL, surface, &textLoc );
    SDL_FreeSurface( textSurf );
  }
}


void DrawPlane( SDL_Surface *screen, sPlane *plane, unsigned int color )
{
  int centerX=WIDTH/2,
      centerY=HEIGHT/2;

  float ratio = (float)HEIGHT/200;

  sVector planeVector = { 0, plane->distance * ratio };
  sVector planeLocVector = RotateVector( planeVector, plane->bearing );

  sVector planeTailVector = { 0, -10 };
  if( gameState == DEBRIEFING )
  {
    planeTailVector.y = -40*ratio;
  }

  sVector planeTailLocVector = RotateVector( planeTailVector, plane->heading );
  sVector planeArr1Vector = { 0, -5 };
  sVector planeArr1LocVector = RotateVector( planeArr1Vector, plane->heading-45 );
  sVector planeArr2Vector = { 0, -5 };
  sVector planeArr2LocVector = RotateVector( planeArr2Vector, plane->heading+45 );

  planeLocVector.x -= planeTailLocVector.x;
  planeLocVector.y -= planeTailLocVector.y;

  Draw_Line( screen,
            centerX-planeLocVector.x,
            centerY-planeLocVector.y,
            centerX-planeLocVector.x-planeTailLocVector.x,
            centerY-planeLocVector.y-planeTailLocVector.y,
            color );
  Draw_Line( screen,
            centerX-planeLocVector.x,
            centerY-planeLocVector.y,
            centerX-planeLocVector.x-planeArr1LocVector.x,
            centerY-planeLocVector.y-planeArr1LocVector.y,
            color );
  Draw_Line( screen,
            centerX-planeLocVector.x,
            centerY-planeLocVector.y,
            centerX-planeLocVector.x-planeArr2LocVector.x,
            centerY-planeLocVector.y-planeArr2LocVector.y,
            color );
}

void DrawCross( SDL_Surface *screen, sPlane *plane, unsigned int color )
{
  int centerX=WIDTH/2,
      centerY=HEIGHT/2;

  float ratio = (float)HEIGHT/200;

  sVector planeVector = { 0, plane->distance * ratio };
  sVector planeLocVector = RotateVector( planeVector, plane->bearing );

  Draw_Line( screen,
            centerX-planeLocVector.x-5,
            centerY-planeLocVector.y-5,
            centerX-planeLocVector.x+5,
            centerY-planeLocVector.y+5,
            color );
  Draw_Line( screen,
            centerX-planeLocVector.x+5,
            centerY-planeLocVector.y-5,
            centerX-planeLocVector.x-5,
            centerY-planeLocVector.y+5,
            color );
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

  DrawText( screen, font, centerX, centerY-ratio*60, "60nm", &textColor, 1 );
  DrawText( screen, font, centerX, centerY-ratio*30, "30nm", &textColor, 1 );
  DrawText( screen, font, centerX, centerY-ratio*10, "10nm", &textColor, 1 );

  DrawText( screen, font, centerX+ratio*90, centerY, "090", &textColor, 1 );
  DrawText( screen, font, centerX-ratio*90, centerY, "270", &textColor, 1 );
  DrawText( screen, font, centerX, centerY-ratio*90, "000", &textColor, 1 );
  DrawText( screen, font, centerX, centerY+ratio*90, "180", &textColor, 1 );

  if( gameState == DEBRIEFING )
  {
    DrawPlane( screen, &plane, 0xff00ffff );
    DrawCross( screen, &plane, 0xff00ffff );
    DrawCross( screen, &bandit, 0xffff0000 );
  }
}


void DrawBullseyeMarker( SDL_Surface *screen, TTF_Font *font )
{
  // TEXT COLOR
  SDL_Color    textColor = { 0, 255, 0, 0 };

  // DRAW NAUTICAL MILES TEXT
  char nmBuf[4];
  sprintf( nmBuf, "%d", (int)plane.distance );
  DrawText( screen, font, MARKERX, MARKERY, nmBuf, &textColor, 1 );

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
  DrawText( screen, font, MARKERX, MARKERY+MARKERSIZE/2+30, bearBuf, &textColor, 1 );

  // DRAW CIRCLE
  Draw_Circle( screen, MARKERX, MARKERY, MARKERSIZE, 0xff00ff00 );

  // DRAW BULLSEYE POINTER
  sVector pointer = { 0, MARKERSIZE+1 };
  sVector pointerLoc = RotateVector( pointer, -plane.heading+plane.bearing );
  Draw_FillCircle( screen, MARKERX+pointerLoc.x, MARKERY+pointerLoc.y, 3, 0xff00ff00 );
}


void DrawScreen( SDL_Surface *screen, TTF_Font *font )
{ 
  SDL_FillRect( screen, 0, 0 );

  DrawBullseyeMarker( screen, font );
  DrawBullseyeMap( screen, font );
  SDL_Color color = { 255,255,0,0 };
  DrawMessageBox( screen, font, &msgBox, &color );

  SDL_Flip( screen ); 
}


char *FormatBearing( int bearing )
{
  char *bearBuf = malloc( 4*sizeof(char) );
  sprintf( bearBuf, "%d", bearing );
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
  return bearBuf;
}


void GenerateNewScenario()
{
  plane.bearing  = (rand()*100)%360;
  plane.heading  = (rand()*100)%360;
  plane.distance = (rand()*100)%60;
  if( plane.bearing < 0 )  plane.bearing  *= -1;
  if( plane.heading < 0 )  plane.heading  *= -1;
  if( plane.distance < 0 ) plane.distance *= -1;

  bandit.bearing  = (rand()*100)%360;
  bandit.distance = (rand()*100)%60;
  bandit.heading  = (rand()*100)%360;
  if( bandit.bearing < 0 )  bandit.bearing  *= -1;
  if( bandit.heading < 0 )  bandit.heading  *= -1;
  if( bandit.distance < 0 ) bandit.distance *= -1;


  char *banditBearing = FormatBearing( bandit.bearing );
  sprintf( msgBox.lines[0], "Bandit %s %d hot!", banditBearing, (int)bandit.distance );
  sprintf( msgBox.lines[1], "Head your plane towards the bandit." );
  free( banditBearing );
}


void UpdateGameState()
{
  if( gameState == GAME )
    gameState = DEBRIEFING;
  else
  {
    GenerateNewScenario();
    gameState = GAME;
  }
}


int main( int argc, char* argv[] )
{
  srand( time(0) );

  SDL_Surface *screen;
  SDL_Event    event;

  msgBox.lines[0] = malloc( 50*sizeof(char) );
  msgBox.lines[1] = malloc( 50*sizeof(char) );
  memset( msgBox.lines[0], 0, 50*sizeof(char) );
  memset( msgBox.lines[1], 0, 50*sizeof(char) );

  int quit = 0;

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

  GenerateNewScenario();

  int turningRight = 0;
  int turningLeft  = 0;

  while( !quit )
  {
    while( SDL_PollEvent( &event ) )
    {
      switch( event.type )
      {
        case SDL_QUIT:
          quit = 1;
          break;

        case SDL_KEYDOWN:
          switch( event.key.keysym.sym )
          {
            case( SDLK_LEFT ):
              turningLeft = 1;
              break;

            case( SDLK_RIGHT ):
              turningRight = 1;
              break;

            case( SDLK_SPACE ):
              UpdateGameState();
              break;

            case( SDLK_ESCAPE ):
              quit = 1;

            default:
              break;
          }
          break;

        case SDL_KEYUP:
          switch( event.key.keysym.sym )
          {
            case( SDLK_LEFT ):
              turningLeft = 0;
              break;

            case( SDLK_RIGHT ):
              turningRight = 0;
              break;

            default:
              break;
          }
      }
    }

    if( turningLeft )
      plane.heading -= 0.2;
    if( turningRight )
      plane.heading += 0.2;

    if( plane.bearing >= 360 )
      plane.bearing = (int)plane.bearing%360;
    if( plane.heading >= 360 )
      plane.heading = (int)plane.heading%360;

    if( bandit.bearing >= 360 )
      bandit.bearing = (int)plane.bearing%360;
    if( bandit.heading >= 360 )
      bandit.heading = (int)plane.heading%360;

    /* plane.bearing += 0.1; */
    /* plane.heading += 0.1; */
    DrawScreen( screen, font );
  }

  SDL_Quit();

  return 0;
}

