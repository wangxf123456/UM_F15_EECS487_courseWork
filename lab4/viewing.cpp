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
#include "viewing.h"

#define SCREEN_WIDTH 1280 // pixels
#define SCREEN_HEIGHT 800 // pixels

int wd;                  /* GLUT window handler */
int screen_w, screen_h;  /* window size in pixels */
int mouse_x, mouse_y;    /* coordinates of last mouse click within window */

transform_t mode;        /* ORTHOGRAPHIC, PERSPECTIVE, HARDWARE */

/* initial camera values */
XVec4f eye_pos0;
XVec4f gaze_dir0; // along -z-axis, g = c - e
XVec4f top_dir0;  // along y-axis
float zNear0;     // z-coordinate of the near plane, negative number
float zFar0;      // z-coordinate of the far plane, negative number
float fovy0;      // fovy

void
init_viewcam(transform_t mode)
{
  eye_pos0 = XVec4f(0.0, 0.0, 1000, 1.0);  // {100, 50, 250, 1.0};
  gaze_dir0 = XVec4f(0.0, 0.0, -1.0, 0.0); // along -z-axis
  top_dir0 = XVec4f(0.0, 1.0, 0.0, 0.0);
  zNear0 = -300.0; // z-coordinate of the near plane, negative number
  zFar0 = -5000.0; // = infinity
  fovy0 = 105.0;
  
  cam = Camera(eye_pos0, gaze_dir0, top_dir0, zNear0, zFar0, fovy0); // last 2 args not used
  
  setup_view(mode);
  transformWorld(mode); // TODO do the transform, including perspective transform in objects.cpp

  return;
}

void
init(void)
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);

  // in objects.cpp
  initCubeArray();
  initPyramid();

  mode=ORTHOGRAPHIC;
  init_viewcam(mode);

  fprintf(stderr, "Viewing mode\n");

  return;
}

/* Draw the cube with the latest transformations */
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
  screen_w = w;
  screen_h = h;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /* Viewing and clipping volume */
  /* Does the orthographic projection:
   * translate the view volume to origin and
   * shrink it into the unit cube (CVV)
  */
  if (mode == HARDWARE) {
    /* YOUR CODE HERE
     * Set up perspective projection using gluPerspective()
     * with the set up stored in Camera "cam".
    */
    gluPerspective(cam.fovy, w / h, abs(cam.zNear), abs(cam.zFar));
  } else {
    glOrtho(-w/2.0, w/2.0, -h/2.0, h/2.0, -w, w);
  }

  // viewport transformation to fit the CVV to NDC
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);

  return;
}

/*
 * Instructions on using this program:
 *
 * 'H' - hardware viewing and perspective transforms
 * 'O' - orthographic projection mode
 * 'P' - perspective projection mode
 * 'I' - reset to original setup
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

  case 'I': /* reset */
    cam = Camera(eye_pos0, gaze_dir0, top_dir0, zNear0, zFar0, fovy0);
    resetWorld(mode);
    setup_view(mode);
    transformWorld(mode); // TODO do the transform, including perspective transform in objects.cpp
    fprintf(stderr, "Reset!\n");
    break;
        
  case 'H':
    mode = HARDWARE;
    init_viewcam(mode);
    reshape(screen_w, screen_h);
    fprintf(stderr, "Hardware transforms\n");
    break;
        
  case 'O':
    mode = ORTHOGRAPHIC;
    init_viewcam(mode);
    reshape(screen_w, screen_h);
    fprintf(stderr, "Orthographic mode\n");
    break;
        
  case 'P':
    mode = PERSPECTIVE;
    init_viewcam(mode);
    reshape(screen_w, screen_h);
    fprintf(stderr, "Perspective mode\n");
    break;
        
  default:
    movecam(key, x, y);   // TODO in transfomrs.cpp
    setup_view(mode);     // TODO set up the viewing transformation in transforms.cpp
    transformWorld(mode); // TODO do the transform, including perspective transform in objects.cpp

    break;
  }

  glutPostRedisplay();

  return;
}

void
mouse(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN) {
    mouse_x = x;
    mouse_y = y;
  }

  return;
}

/*
 * Mouse behaviour:
 * drag left/right: pan
 * drag up/down: tilt
 */
void
motion(int x, int y)
{
  movecam(MOUSE_DRAG, (x - mouse_x), (y - mouse_y)); /* TODO in transforms.cpp */
  setup_view(mode);     // TODO set up the viewing transformation in transforms.cpp
  transformWorld(mode); // TODO do the transform, including perspective transform in objects.cpp

  mouse_x = x;
  mouse_y = y;
    
  glutPostRedisplay(); /* Refresh screen after changes */

  return;
}

int
main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  wd = glutCreateWindow("Lab 4: Viewing Transformations");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(kbd);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  init();
  glutMainLoop();

  exit(0);
}
