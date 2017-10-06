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
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
#include "windows.h"
#endif
#include <GL/glu.h>
#endif

#include "xvec.h"

#include "viewing.h"
#include "transforms.h"

const float DELTA = (float) 0.1;
const float THETA = (float) (4.0*M_PI/180.0);
const float THETA_PER_PIXEL = (float) (0.5*M_PI/180.0);

Camera cam;

Camera::
Camera(XVec4f &eye_pos, XVec4f &gaze_dir, XVec4f &top_dir,
       float near_plane, float far_plane, float deg)
{
  e = eye_pos;
  g = gaze_dir;
  t = top_dir;

  zNear = near_plane;
  zFar = far_plane;
  fovy = deg;

  orient();

  return;
}

void Camera::
orient()
{  
  /* 
   * Compute the new basis vectors u, v, w based on
   * gaze direction (g) and top direction (t) already
   * stored in the Camera's class variables.
   * Store the results in the Camera's u, v, w variables.
  */

  w = -g;
  w.normalize();
  
  u = t.cross(w);
  u.normalize();
  
  v = w.cross(u);

  return;
}

void Camera::
dolly(float delta)
{
  /*
   * Advance the eye position along
   * gaze direction by delta factor.
  */
  e += delta*g;
  
  return;
}

void Camera::
crab(float delta)
{
  /* 
   * Track the eye POSITION left and right
   * by delta factor, no panning.
  */
  e += delta*u;
  
  return;
}

void Camera::
boom(float delta)
{
  /*
   * Track the eye POSITION up and down
   * by delta factor, no tilting.
  */
  e += delta*v;

  return;
}

void Camera::
pan(float theta)
{
  /*
   * Rotate gaze direction by theta
   * about the camera's v axis
  */
  g.rotate(theta, v);

  return;
}
  
void Camera::
tilt(float theta)
{
  /*
   * Rotate gaze direction AND top direction
   * by theta about the camera's u axis
  */
  g.rotate(theta, u);
  t.rotate(theta, u);

  return;
}
  
void Camera::
roll(float theta)
{
  /*
   * Manipulate top direction so as to get the effect of roll
  */
  t.rotate(theta, w);

  return;
}

void
movecam(unsigned char key, int x, int y)
{
  switch (key) {
  case 'l': // crab right
  case 'X':
    cam.crab(DELTA);
    break;
  case 'h': // crab left
  case 'x':
    cam.crab(-DELTA);
    break;
  case 'k': // boom up
  case 'Y':
    cam.boom(DELTA);
    break;
  case 'j': // boom down
  case 'y':
    cam.boom(-DELTA);
    break;
  case 'w': // dolly forward
  case 'z':
    cam.dolly(DELTA);
    break;
  case 's': // dolly backward
  case 'Z':
    cam.dolly(-DELTA);
    break;
  case 'd': // roll cw
    cam.roll(-THETA);
    break;
  case 'a': // roll ccw
    cam.roll(THETA);
    break;
  case MOUSE_DRAG:
    if (x) { // pan left or right, gaze follows mouse
      cam.pan(-x*THETA_PER_PIXEL);
    }
    if (y) { // tilt left or right, gaze follows mouse
      cam.tilt(-y*THETA_PER_PIXEL);
    }
    break;
  default:
    break;
  }

  /* re-orient camera's coordinate system */
  cam.orient();
  
  return;
}

void
setup_view()
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gluLookAt(cam.e.x(), cam.e.y(), cam.e.z()+cam.zNear,
            cam.g.x()+cam.e.x(), cam.g.y()+cam.e.y(), cam.g.z()+cam.e.z()+cam.zNear,
            cam.t.x(), cam.t.y(), cam.t.z());

  return;
}
