#include "vector.h"
#include <math.h>

#define PIOVER180 0.0174532925

sVector RotateVector( sVector vec, float angle )
{
  angle = angle*PIOVER180;
  sVector ret = {
    vec.x * cos( angle ) - vec.y * sin( angle ),
    vec.y * cos( angle ) + vec.x * sin( angle )
  };
  return ret;
}

