/*
 * Copyright (c) 2008, 2011, 2012 University of Michigan, Ann Arbor.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of Michigan, Ann Arbor. The name of the University 
 * may not be used to endorse or promote products derived from this 
 * software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Authors: Manoj Rajagopalan, Sugih Jamin
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

using namespace std;
#include "matmv.h"

#define DELTA 10.0
#define ROTDEG 9.0

const float THETA = ROTDEG * M_PI/180.0;
    // theta (in rad)= s (arc length)/r (radius),
    // circle (in rad): circumference/radius = 2pi*r/r = 2pi
    // 2pi rad = 360 deg

void
scale(unsigned char key)
{
  switch (key) {
  case 'l': // scale x-coord up 50%
  case 'X':
    /* YOUR CODE HERE */
    CTM.scale(1.5, 1, 1);
    break;
  case 'h': // scale x-coord down 50%
  case 'x':
    /* YOUR CODE HERE */
    CTM.scale(1 / 1.5, 1, 1);
    break;
  case 'k': // scale y-coord up 50%
  case 'Y':
    /* YOUR CODE HERE */
    CTM.scale(1, 1.5, 1);
    break;
  case 'j': // scale y-coord down 50%
  case 'y':
    /* YOUR CODE HERE */
    CTM.scale(1, 1 / 1.5, 1);
    break;
  case 'w': // scale z-coord down 50%
  case 'z':
    /* YOUR CODE HERE */
    CTM.scale(1, 1, 1 / 1.5);
    break;
  case 's': // scale z-coord up 50%
  case 'Z':
    /* YOUR CODE HERE */
    CTM.scale(1, 1, 1.5);
    break;
  default:
    CTM.Identity();
    break;
  }

  return;
}

void
rotate(unsigned char key)
{
  switch (key) {
  case 'l': // rotate ROTDEG degree cw around -z-axis
  case 'Z':
    /* YOUR CODE HERE */
    CTM.rotateZ(-THETA);
    break;
  case 'h': // rotate ROTDEG degree ccw around -z-axis
  case 'z':
    /* YOUR CODE HERE */
    CTM.rotateZ(THETA);
    break;
  case 'k': // rotate ROTDEG degree cw around x-axis
  case 'X':
    /* YOUR CODE HERE */
    CTM.rotateX(-THETA);
    break;
  case 'j': // rotate ROTDEG degree ccw around x-axis
  case 'x':
    /* YOUR CODE HERE */
    CTM.rotateX(THETA);
    break;
  case 'w': // rotate ROTDEG degree cw around y-axis
  case 'Y':
    /* YOUR CODE HERE */
    CTM.rotateY(THETA);
    break;
  case 's': // rotate ROTDEG degree ccw around y-axis
  case 'y':
    /* YOUR CODE HERE */
    CTM.rotateY(-THETA);
    break;
  default:
    CTM.Identity();
    break;
  }

  return;
}

void
shear(unsigned char key)
{
  switch (key) {
  case 'l': // shear x by 10% y
  case 'X':
    /* YOUR CODE HERE */
    CTM.Xshearby(0.1, 0);
    break;
  case 'h': // shear x by -10% y
  case 'x':
    /* YOUR CODE HERE */
    CTM.Xshearby(-0.1, 0);
    break;
  case 'k': // shear y by 10% z
  case 'Y':
    /* YOUR CODE HERE */
    CTM.Yshearby(0, 0.1);
    break;
  case 'j': // shear y by -10% z
  case 'y':
    /* YOUR CODE HERE */
    CTM.Yshearby(0, -0.1);
    break;
  case 'w': // shear z by -10% y
  case 'z':
    /* YOUR CODE HERE */
    CTM.Zshearby(0, -0.1);
    break;
  case 's': // shear z by 10% y
  case 'Z':
    /* YOUR CODE HERE */
    CTM.Zshearby(0, 0.1);
    break;
  default:
    CTM.Identity();
    break;
  }

  return;
}

void
translate(unsigned char key)
{
  switch (key) {
  case 'l': // translate +DELTA along x-axis
  case 'X':
    /* YOUR CODE HERE */
    CTM.translate(DELTA, 0, 0);
    break;
  case 'h': // translate -DELTA along x-axis
  case 'x':
    /* YOUR CODE HERE */
    CTM.translate(-DELTA, 0, 0);
    break;
  case 'k': // translate +DELTA along y-axis
  case 'Y':
    /* YOUR CODE HERE */
    CTM.translate(0, DELTA, 0);
    break;
  case 'j': // translate -DELTA along y-axis
  case 'y':
    /* YOUR CODE HERE */
    CTM.translate(0, -DELTA, 0);
    break;
  case 'w': // translate -DELTA along z-axis
  case 'z':
    /* YOUR CODE HERE */
    CTM.translate(0, 0, -DELTA);
    break;
  case 's': // translate +DELTA along z-axis
  case 'Z':
    /* YOUR CODE HERE */
    CTM.translate(0, 0, DELTA);
    break;
  default:
    CTM.Identity();
    break;
  }

  return;
}
