/*
 * Copyright (c) 2009, 2014 University of Michigan, Ann Arbor.
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
 * This lab consists of the following tasks:
 *   1. Draw two points on the screen
 *   2. Connect the two points into a line
 *   3. Change the line to 3-pixel thick
 *   4. Draw filled squares at both end points:
 *      - perturb coordinates, use GL_QUADS
 *   5. Draw a line at the x- and y-axes 
 *   6. Register the provided refresh() function that shifts the coordinate 
 *      system by (-1, -1) every 100 ms when the system is idle
 *   7. Add keyboard command `b' to draw outline squares at both end points
 *      Use glPolygonMode().
 *   8. Draw a filled square around the Origin, use glPushAttrib() and glPopAttrib().
 *   9. Register the cursor() function as a GLUT call back for mouse cursor motion events.
 *  10. Register the mouse() function as a GLUT call back for mouse clicked events.
 *  11. Register the drag() function as a GLUT call back for mouse dragged events.
 * 
 * Authors: Manoj Rajagopalan, Sugih Jamin
*/
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define NENDS 2           /* number of end "points" to draw */
#define ENDHWIDTH 5       /* half width of end "points," in pixels */
 
GLdouble width, height;   /* window width and height */
int wd;                   /* GLUT window handle */
int ends[NENDS][2];       /* array of 2D points */
int lastx, lasty;         /* where was the mouse last clicked? */
int end;                  /* which of the above points was
                             the last mouse click close to?
                             -1 if neither */
int drift;                /* whether to drift the coordinate system */
double offset=0.0;
int mode;         /* draw points or lines? */

/* Program initialization NOT OpenGL/GLUT dependent,
   as we haven't created a GLUT window yet */
void
init(void)
{
  drift = 0;
  width  = 640.0;                 /* initial window width and height, */
  height = 400.0;                  /* within which we draw. */
  ends[0][0] = (int)(0.25*width);  /* (0,0) is the lower left corner */
  ends[0][1] = (int)(0.75*height);
  ends[1][0] = (int)(0.75*width);
  ends[1][1] = (int)(0.25*height);

  /* Task 1: set the global variable "mode" to the OpenGL drawing mode to draw points */
  mode = GL_POINTS;

  return;
}

/* Callback functions for GLUT */

/* Draw the window - this is where all the GL actions are */
void
display(void)
{
  int i;
  
  /* clear the screen */
  glClear(GL_COLOR_BUFFER_BIT);
  
  /* Task 1 and Task 2 */
  /* Draw the two end points stored in the ends[] global array and
     then draw a line between them.  The global variable "mode"
     specifies whether to draw points or lines.  It is set in init()
     and toggled in kbd(). */
  glBegin(mode);
  for (i = 0; i < NENDS; ++i) {
    glVertex2iv(ends[i]);
  }
  glEnd();
  
  /* Task 4: draw a red square of width 2*ENDHWIDTH pixels
     around each ends[] to highlight the end points. */
  /* set the squares' color to red */
  /* form quadrilaterals around each ends[]
     - each quad takes a group of 4 points */
  
  int square[NENDS][4][2];
  for (i = 0; i < NENDS; ++i) {
    square[i][0][0] = ends[i][0] - ENDHWIDTH;
    square[i][0][1] = ends[i][1] - ENDHWIDTH;
    square[i][1][0] = ends[i][0] + ENDHWIDTH;
    square[i][1][1] = ends[i][1] - ENDHWIDTH;
    square[i][2][0] = ends[i][0] + ENDHWIDTH;
    square[i][2][1] = ends[i][1] + ENDHWIDTH;
    square[i][3][0] = ends[i][0] - ENDHWIDTH;
    square[i][3][1] = ends[i][1] + ENDHWIDTH;
  }
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_QUADS);
  for (i = 0; i < NENDS; ++i) {
    glVertex2iv(square[i][0]);
    glVertex2iv(square[i][1]);
    glVertex2iv(square[i][2]);
    glVertex2iv(square[i][3]);
  } 
  glEnd();
  /* Task 5: draw a green line on the x- and y-axes */

  int axis[4][2];
  axis[0][0] = -(int)width;
  axis[0][1] = 0;
  axis[1][0] = (int)width;
  axis[1][1] = 0;
  axis[2][0] = 0;
  axis[2][1] = -(int)height;
  axis[3][0] = 0; 
  axis[3][1] = (int)height;
  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_LINES);
  for (i = 0; i < 4; ++i) {
    glVertex2iv(axis[i]);
  }
  glEnd();
  /* Task 8: draw a 10x10 blue filled square around the Origin, *
   *         use glPushAttrib() and glPopAttrib().              */
  
  int origin[4][2];
  origin[0][0] = -ENDHWIDTH;
  origin[0][1] = -ENDHWIDTH;
  origin[1][0] = ENDHWIDTH;
  origin[1][1] = -ENDHWIDTH;
  origin[2][0] = ENDHWIDTH;
  origin[2][1] = ENDHWIDTH;
  origin[3][0] = -ENDHWIDTH;
  origin[3][1] = ENDHWIDTH;

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glPolygonMode(GL_FRONT, GL_FILL);
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_QUADS);
  glVertex2iv(origin[0]);
  glVertex2iv(origin[1]);
  glVertex2iv(origin[2]);
  glVertex2iv(origin[3]);
  glEnd();
  glPopAttrib();
  /* force drawing to start */
  glFlush();

  return;
}

/* Called when window is resized,
   also when window is first created,
   before the first call to display(). */
void
reshape(int w, int h)
{
  /* save new screen dimensions */
  width = (GLdouble) w;
  height = (GLdouble) h;

  /* tell OpenGL to use the whole window for drawing */
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);

  /* do an orthographic parallel projection with the coordinate
     system set to first quadrant, limited by screen/window size */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, width, 0.0, height, -1.f, 1.f);

  return;
}

/* Refresh the display, called when system is idle */
void
refresh(void)
{
  static struct timeval prev;
  struct timeval now;
  double tprev;
  double tnow;

#ifdef _WIN32
  SYSTEMTIME st;
  
  GetSystemTime(&st);
  now.tv_sec = st.wSecond;
  now.tv_usec = st.wMilliseconds*1000;
#else
  gettimeofday(&now, NULL);
#endif

  tprev = (double)prev.tv_sec + 1.0e-6*(double)prev.tv_usec;
  tnow = (double)now.tv_sec + 1.0e-6*(double)now.tv_usec;
  if ((tnow - tprev) > 0.1) {
    prev.tv_sec = now.tv_sec;
    prev.tv_usec = now.tv_usec;

    if (drift) {
      /* shift the coordinate system by (-1, -1) every 100 ms */
      offset -= 1.0;
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(offset, width+offset, offset, height+offset, -1.f, 1.f);
    }     

    /* redisplay */
    glutPostRedisplay();
  }

  return;
}

/* Trap and process keyboard events */
void
kbd(unsigned char key, int x, int y)
{
  switch((char)key) {
  case 'q':
  case 27:    /* ESC */
    glutDestroyWindow(wd);
    exit(0);
    break;
  case '3':
    /* Task 3: set line thickness to 3 pixels (default 1) */
    glLineWidth(3);
    break;
  case 'a':
    drift = drift ? 0 : 1;
    break;
  case 'b':   /* Task 7: draw outline squares at both end points */
    /* Draw only the boundary/edges of the end squares: set front
     * facing polygon to draw only line, no fill, use glPolygonMode(). */
    glPolygonMode(GL_FRONT, GL_LINE);
    break;
  case 'l':
    /* Task 2: draw a line */
    /* Set the global variable "mode" to the OpenGL draw mode to draw a line */
    mode = GL_LINES;
    break;
  case 'p':
    /* Task 1: Set the global variable "mode" to the OpenGL draw mode to draw points */
    mode = GL_POINTS;
    break;
  default:
    break;
  }
  
  /* redisplay */
  glutPostRedisplay();

  return;
}

int
find_closest_end(int x, int y)
{
  int n;
  for (n = 0; n < NENDS; ++n) {
    if (abs(x - ends[n][0]) <= ENDHWIDTH && abs(y - ends[n][1]) <= ENDHWIDTH) {
      return (n);
    }
  }
  
  return (-1);
}

/* Handle "mouse cursor moved" events */
void
cursor(int x, int y)
{
  static int cursor = GLUT_CURSOR_LEFT_ARROW;
  int ncursor;

  end = find_closest_end(x+(int)offset, (int)height - y+(int)offset);

  ncursor = (end < 0) ? GLUT_CURSOR_LEFT_ARROW : GLUT_CURSOR_CROSSHAIR;
  if (cursor != ncursor) {
    cursor = ncursor;
    glutSetCursor(cursor);
  }

  return;
}

/* Handle "mouse moved with button pressed" events */
void
drag(int x, int y)
{
  if (end >= 0) {
    ends[end][0] += (x - lastx);
    ends[end][1] -= (y - lasty);
    lastx = x;
    lasty = y;
    glutPostRedisplay();
  }

  return;
}

/* Handle mouse button pressed and released events */
void
mouse(int button, int state, int x, int y)
{
  /* Trap left-clicks */
  if (button == GLUT_LEFT_BUTTON && end >= 0) {
    if (state == GLUT_DOWN) {
      lastx = x;
      lasty = y;
    } else {
      drag(x, y);
    }
  }

  return;
}

int
main(int argc, char *argv[])
{
  /* perform initialization NOT OpenGL/GLUT dependent,
     as we haven't created a GLUT window yet */
  init();
  
  /* initialize GLUT, let it extract command-line 
     GLUT options that you may provide 
     - NOTE THE '&' BEFORE argc */
  glutInit(&argc, argv);
  
  /* specify the display to be single 
     buffered and color as RGBA values */
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
  
  /* set the initial window size */
  glutInitWindowSize((int) width, (int) height);
  
  /* create the window and store the handle to it */
  wd = glutCreateWindow("Experiment with line drawing" /* title */ );

  /* --- register callbacks with GLUT --- */

  /* register function that draws in the window */
  glutDisplayFunc(display);

  /* register function to handle window resizes */
  glutReshapeFunc(reshape);

  /* Task 6: register the provided refresh() 
     function to call when system is idle */
  glutIdleFunc(refresh);

  /* register keyboard event processing function */
  glutKeyboardFunc(kbd);

  /* Task 9: register cursor moved event callback */
  glutPassiveMotionFunc(cursor);

  /* Task 10: register mouse clicked event callback */
  glutMouseFunc(mouse);

  /* Task 11: register mouse dragged event callback */
  glutMotionFunc(drag);
  
  /* Set up initial OpenGL context */
  /* clear color buffer to white */
  glClearColor(1.0, 1.0, 1.0, 0.0);
  /* set drawing color to black */
  glColor3f(0.0, 0.0, 0.0);
  /* set point size to 5 so that the endpoints are visible */
  glPointSize(5.0);

  /* start the GLUT main loop */
  glutMainLoop();

  exit(0);
}
