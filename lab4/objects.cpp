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

#include "xvec.h"
#include "objects.h"
#include "viewing.h"
#include <iostream>
using namespace std;

extern XMat4f CTM;

XVec4f cube_array0[5][5][8]; /* 5x5 cube-array in world space */
XVec4f cube_array[5][5][8];  /* the above used for viewing transforms,
                                what's drawn on screen */
XVec4f pyramid0[5];          /* a pyramid in world space */
XVec4f pyramid[5];           /* the above used for viewing transforms,
                                what's drawn on screen */

void
initCubeArray(void)
{
  int m,n;
  GLfloat cube_center_x, cube_center_y, cube_center_z;

  /* Initialize cube */
  cube_center_y = 0.0;

  for(m=0; m<5; ++m) {
    for(n=0; n<5; ++n) {

      cube_center_x = (GLfloat)((m-2)*150.0);
      cube_center_z = (GLfloat)((n-2)*150.0);
      
      cube_array0[m][n][0] = cube_array[m][n][0] =
        XVec4f(cube_center_x+50, cube_center_y+50, cube_center_z+50, 1.0);

      cube_array0[m][n][1] = cube_array[m][n][1] =
        XVec4f(cube_center_x+50, cube_center_y+50, cube_center_z-50, 1.0);

      cube_array0[m][n][2] = cube_array[m][n][2] =
        XVec4f(cube_center_x-50, cube_center_y+50, cube_center_z-50, 1.0);

      cube_array0[m][n][3] = cube_array[m][n][3] =
        XVec4f(cube_center_x-50, cube_center_y+50, cube_center_z+50, 1.0);

      cube_array0[m][n][4] = cube_array[m][n][4] =
        XVec4f(cube_center_x+50, cube_center_y-50, cube_center_z+50, 1.0);

      cube_array0[m][n][5] = cube_array[m][n][5] =
        XVec4f(cube_center_x+50, cube_center_y-50, cube_center_z-50, 1.0);

      cube_array0[m][n][6] = cube_array[m][n][6] =
        XVec4f(cube_center_x-50, cube_center_y-50, cube_center_z-50, 1.0);

      cube_array0[m][n][7] = cube_array[m][n][7] =
        XVec4f(cube_center_x-50, cube_center_y-50, cube_center_z+50, 1.0);
    }
  }

  return;
}

void
initPyramid(void)
{
  /* Initialize pyramid */
  pyramid0[0] = pyramid[0] = XVec4f(  0.0, 200.0, -200.0, 1.0);
  pyramid0[1] = pyramid[1] = XVec4f( 25.0,   0.0, -175.0, 1.0);
  pyramid0[2] = pyramid[2] = XVec4f(-25.0,   0.0, -175.0, 1.0);
  pyramid0[3] = pyramid[3] = XVec4f(-25.0,   0.0, -225.0, 1.0);
  pyramid0[4] = pyramid[4] = XVec4f( 25.0,   0.0, -225.0, 1.0);

  return;
}

void
resetCubePyramid()
{
  int i, j, k;
  
  for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      for (k = 0; k < 8; k++) {
        cube_array[i][j][k] = cube_array0[i][j][k];
      }
    }
  }
  for (i = 0; i < 5; i++) {
    pyramid[i] = pyramid0[i];
  }

  return;
}

void
drawCubeArray(void)
{
  int m,n,k;

  glColor3f(1.0, 1.0, 1.0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  for(m=0; m<5; ++m) {
    for(n=0; n<5; ++n) {
      glBegin(GL_QUADS);
      for (k = 0; k < 8; k++) {
        glVertex3fv(cube_array[m][n][k]);
      }
      glEnd();

      glBegin(GL_LINES);
        glVertex3fv(cube_array[m][n][0]);
        glVertex3fv(cube_array[m][n][4]);
        glVertex3fv(cube_array[m][n][1]);
        glVertex3fv(cube_array[m][n][5]);
        glVertex3fv(cube_array[m][n][2]);
        glVertex3fv(cube_array[m][n][6]);
        glVertex3fv(cube_array[m][n][3]);
        glVertex3fv(cube_array[m][n][7]);
      glEnd();
    }
  }

  return;
}

void
drawPyramid(void)
{
  glColor3f(1.0, 1.0, 0.0);
  glBegin(GL_TRIANGLE_FAN);
    glVertex3fv(pyramid[0]);
    glVertex3fv(pyramid[1]);
    glVertex3fv(pyramid[2]);
    glVertex3fv(pyramid[3]);
    glVertex3fv(pyramid[4]);
    glVertex3fv(pyramid[1]);
  glEnd();

  return;
}

void
drawWorld(transform_t mode, int w, int h)
{
  glLineWidth(1.0);
  
  drawCubeArray();
  drawPyramid();

  glFlush();

  return;
}
    
void
resetWorld(transform_t mode)
{
  CTM.Identity();
  resetCubePyramid();

  return;
}

void
transformWorld(transform_t mode)
{
  /* YOUR CODE HERE 
   * Viewing transform:
   * Apply the viewing transformation set up in the CTM to the original 
   * world coordinates of the objects in cube_array0[5][5] and pyramid0 
   * and store the eye-space coordinates in cube_array[5][5] and pyramid.
   */

  if (mode == PERSPECTIVE) {
    /* YOUR CODE HERE 
     * Perspective projection: project all points onto 2D 
     *   - apply perspective projection
     *   - you only need to create a rectangular view volume here;
     *     you don't need to fit/shrink it into a unit view volume (cvv)
     *     nor to translate it to the origin.  These last two will be
     *     done by the orthographic projection in viewing.cpp:reshape()
     *     (the call to glOrtho()).  Similarly fitting the cvv to ndc
     *     is done in transforms.cpp:reshape() (the call to glViewport()).
     */
     XMat4f mat_p = XMat4f();
     // cam.zFar = 4 * cam.zNear;
     mat_p(0, 0) = cam.zNear;
     mat_p(1, 1) = cam.zNear;
     mat_p(2, 2) = cam.zNear;
     // mat_p(2, 3) = -cam.zNear * cam.zFar;
     mat_p(3, 2) = 1;
     CTM = mat_p * CTM;
  }

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      for (int k = 0; k < 8; k++) {
        cube_array[i][j][k] = CTM * cube_array0[i][j][k];
        if (mode == PERSPECTIVE) {
          cube_array[i][j][k] = cube_array[i][j][k] / cube_array[i][j][k].w();
        }
      }
    }
  }
  for (int i = 0; i < 5; i++) {
    pyramid[i] = CTM * pyramid0[i];
    if (mode == PERSPECTIVE) {
      pyramid[i] = pyramid[i] / pyramid[i].w();
    }
  }

  return;
}
