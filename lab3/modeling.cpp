/*
 * Copyright (c) 2008, 2011, 2012, 2014 University of Michigan, Ann Arbor.
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "xvec.h"
#include "xmat.h"

#include "objects.h"
#include "transforms.h"

#define SCREEN_WIDTH 1280 // pixels
#define SCREEN_HEIGHT 800 // pixels

int wd;                  /* GLUT window handler */
int screen_w, screen_h;  /* window size in pixels */

transform_t mode;               /* MAGNIFICATION, ROTATION, SHEAR, TRANSLATION */

void
setcam()
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(2.0, 3.0, 6.0,  // eye position
            0.0, 0.0, 0.0,  // center position (not gaze direction)
            0.0, 1.0, 0.0); // up vector
  return;
}
  
void
init(void)
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);
  setcam();

  object = CUBE;
  mode = ROTATION;
  fprintf(stderr, "Rotation mode\n");

  return;
}

/* Draw the world with the latest transformations */
void
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawWorld(mode, screen_w, screen_h);

  /* Force display */
  glutSwapBuffers();

  return;
}

void
reshape(int w, int h)
{
  // viewport transformation to fit the CVV to NDC
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /* Viewing and clipping volume */
  /* Does the orthographic projection:
   * translate the view volume to origin and
   * shrink it into the unit cube (CVV)
  */
  glOrtho(-w/2.0, w/2.0, -h/2.0, h/2.0, -w, w);

  screen_w = w;
  screen_h = h;

  return;
}

/*
 * Instructions on using this program:
 *
 * 'M' - magnification (scaling) mode
 * 'R' - rotation mode
 * 'S' - shear mode
 * 'T' - translation mode
 * 'I' - reset to original
 *
 * In each mode, pressing the 'l', 'k', 'w' keys
 * makes the transformation in the 'forward' direction,
 * pressing 'h', 'j', 's' makes the transformation in the
 * 'reverse' direction. You are required to implement this
 * functionality. Look at the kbd() function to see how
 * the various functions are called.
 *
*/
void
kbd(unsigned char key, int x, int y)
{
  switch(key) {
        
  case 'q': /* quit */
  case 27 :
    glutDestroyWindow(wd);
    exit (0);

  case '1':
    object = CUBE;
    printf("Drawing cube\n");
    break;
    
  case '2':
    object = CONE;
    printf("Drawing cone\n");
    break;
    
  case 'I': /* reset */
    resetWorld(mode);
    transformWorld(mode); // TODO in objects.cpp
    fprintf(stderr, "Reset!\n");
    break;
        
    /* The following 5 cases only set the state of the program */
  case 'M':
    mode = MAGNIFICATION;
    printf("Magnification mode\n");
    break;
        
  case 'R':
    mode = ROTATION;
    printf("Rotation mode\n");
    break;
        
  case 'S':
    mode = SHEAR;
    printf("Shear mode\n");
    break;
        
  case 'T':
    mode = TRANSLATION;
    printf("Translation mode\n");
    break;
        
  default:

    setupWorld(mode);       // TODO in objects.cpp

    /* Setup the CTM for modeling transform or
     * the camera coordinate system for viewing transform.
    */
    switch (mode) {
    case MAGNIFICATION:
      scale(key);     // TODO in transforms.cpp
      break;
    case ROTATION:
      rotate(key);    // TODO in transforms.cpp
      break;
    case SHEAR:
      shear(key);     // TODO in transforms.cpp
      break;
    case TRANSLATION:
      translate(key); // TODO in transforms.cpp
      break;
    default:
      break;
    }

    transformWorld(mode); // TODO in objects.cpp
    setcam();

    break;
  }

  glutPostRedisplay();

  return;
}

int
main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  wd = glutCreateWindow("Lab 3: 3D Transformations");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(kbd);
  init();
  glutMainLoop();

  exit(0);
}
