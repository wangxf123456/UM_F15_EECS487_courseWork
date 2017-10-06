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

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include "windows.h"
#endif
#include <GL/gl.h>
#endif

const float DELTA = (float)0.1;
#define ROTDEG 5.0
//const float THETA = ROTDEG * M_PI/180.0;
    // theta (in rad)= s (arc length)/r (radius),
    // circle (in rad): circumference/radius = 2pi*r/r = 2pi
    // 2pi rad = 360 deg

void
rotate(unsigned char key, int x, int y)
{
  glMatrixMode(GL_MODELVIEW);

  switch (key) {
  case 'l': // rotate ROTDEG degree cw around -z-axis
  case 'd': // rotate ROTDEG degree cw around -z-axis
  case 'Z':
    glRotatef(-ROTDEG, 0.0, 0.0, 1.0);
    glMatrixMode(GL_TEXTURE);
    glRotatef(-ROTDEG, 0.0, 0.0, 1.0);
    break;
  case 'h': // rotate ROTDEG degree ccw around -z-axis
  case 'a': // rotate ROTDEG degree ccw around -z-axis
  case 'z':
    glRotatef(ROTDEG, 0.0, 0.0, 1.0);
    glMatrixMode(GL_TEXTURE);
    glRotatef(ROTDEG, 0.0, 0.0, 1.0);
    break;
  case 'k': // rotate ROTDEG degree cw around x-axis
  case 'X':
    glRotatef(-ROTDEG, 1.0, 0.0, 0.0);
    glMatrixMode(GL_TEXTURE);
    glRotatef(-ROTDEG, 1.0, 0.0, 0.0);
    break;
  case 'j': // rotate ROTDEG degree ccw around x-axis
  case 'x':
    glRotatef(ROTDEG, 1.0, 0.0, 0.0);
    glMatrixMode(GL_TEXTURE);
    glRotatef(ROTDEG, 1.0, 0.0, 0.0);
    break;
  case 'w': // rotate ROTDEG degree cw around y-axis
  case 'Y':
    glRotatef(-ROTDEG, 0.0, 1.0, 0.0);
    glMatrixMode(GL_TEXTURE);
    glRotatef(-ROTDEG, 0.0, 1.0, 0.0);
    break;
  case 's': // rotate ROTDEG degree ccw around y-axis
  case 'y':
    glRotatef(ROTDEG, 0.0, 1.0, 0.0);
    glMatrixMode(GL_TEXTURE);
    glRotatef(ROTDEG, 0.0, 1.0, 0.0);
    break;
  default:
    break;
  }
  glMatrixMode(GL_MODELVIEW);

  return;
}

void
translate(unsigned char key, int x, int y)
{
  glMatrixMode(GL_MODELVIEW);

  switch (key) {
  case 'l': // translate +DELTA along x-axis
  case 'X':
    glTranslatef(DELTA, 0.0, 0.0);
    glMatrixMode(GL_TEXTURE);
    glTranslatef(DELTA, 0.0, 0.0);
   break;
  case 'h': // translate -DELTA along x-axis
  case 'x':
    glTranslatef(-DELTA, 0.0, 0.0);
    glMatrixMode(GL_TEXTURE);
    glTranslatef(-DELTA, 0.0, 0.0);
    break;
  case 'k': // translate +DELTA along y-axis
  case 'Y':
    glTranslatef(0.0, DELTA, 0.0);
    glMatrixMode(GL_TEXTURE);
    glTranslatef(0.0, DELTA, 0.0);
    break;
  case 'j': // translate -DELTA along y-axis
  case 'y':
    glTranslatef(0.0, -DELTA, 0.0);
    glMatrixMode(GL_TEXTURE);
    glTranslatef(0.0, -DELTA, 0.0);
    break;
  case 'w': // translate -DELTA along z-axis
  case 'z':
    glTranslatef(0.0, 0.0, -DELTA);
    glMatrixMode(GL_TEXTURE);
    glTranslatef(0.0, 0.0, -DELTA);
    break;
  case 's': // translate +DELTA along z-axis
  case 'Z':
    glTranslatef(0.0, 0.0, DELTA);
    glMatrixMode(GL_TEXTURE);
    glTranslatef(0.0, 0.0, DELTA);
    break;
  default:
    break;
  }
  glMatrixMode(GL_MODELVIEW);

  return;
}
