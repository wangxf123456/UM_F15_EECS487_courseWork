/*
 * Copyright (c) 2008, 2011, 2014, 2015 University of Michigan, Ann Arbor.
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
 * Authors: Manoj Rajagopalan, Ari Grant, Sugih Jamin
*/
#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Include the GL headers */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "xvec.h"
#include "rasterizer.h"

/* Where was the mouse last clicked? */
#define NVERTICES 3
XVec2f vertices[NVERTICES]; /* Array of 2D points */

/* Flag to indicate the number of vertices that the user has set by clicking */
#define ALPHA_DEC 0.25
#define CMINX 4.0
#define CMINY 4.0

int vvalid;
bool aa;
float alpha;
int clipped;
float vwidth, vheight; /* screen's resolution in virtual pixels */
XVec4f clipView, background;

/* screen width and height - screen refers to
   window within which we draw*/
#define DELTA 25
GLdouble screen_width=51*DELTA, screen_height=32*DELTA;

/* Handle to the GLUT window */
int wd;

void
init()
{
  int i;

  /* enable alpha blending */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  for (i = 0; i < NVERTICES; i++) {
    vertices[i] = XVec2f(-1.0, -1.0);
  }
  vvalid = 0;
  aa = false;
  alpha = 1.0;
  clipped = 0;

  return;
}

inline XVec4f
inBW(XVec4f &color)
{
  float grey = XVec4f(0.3,0.59,0.11,0.0).dot(color);
  return XVec4f(grey, grey, grey, color.alpha());
}

void
drawPoint(XVec2f &point, XVec4f &pointcolor)
{
  glColor4f(pointcolor.red(), pointcolor.green(), pointcolor.blue(), pointcolor.alpha());
  glRectf(point.x()*DELTA, point.y()*DELTA, (point.x()+1)*DELTA, (point.y()+1)*DELTA);
  /* NOTE: you cannot call glRect within a glBegin()...glEnd() block */

  return;
}

void
initgrid()
{
  int i;
  
  /* set/clear background color */
  if (clipped) {
    glClearColor(0.7, 0.7, 0.7, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* clipped window */
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glRectf(clipView(0)*DELTA, clipView(1)*DELTA,
            (clipView(0)+clipView(2))*DELTA,
            (clipView(1)+clipView(3))*DELTA);
  } else {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }
        
  /* Draw a grey grid */
  glColor3f(0.5, 0.5, 0.5);
  glLineWidth(1.0);
  glBegin(GL_LINES);
  /* vertical lines */
  for (i = 0; i < screen_width; i += DELTA) {
    glVertex2i(i,0);
    glVertex2i(i,(int) screen_height);
  }
  /* horizontal lines */
  for (i = 0; i < screen_height; i += DELTA) {
    glVertex2i(0,i);
    glVertex2i((int)screen_width, i);
  }
  glEnd();

  return;
}
  
/* Display callback */
void
display(void)
{       
  Line l;
  Triangle t;
  XVec4f color0, color1;

  initgrid();        
        
  /* Fill the 'front' face of polygons - rectangles in our case */
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
        
  /* Draw the first point (square) in red, if valid) */
  if (vertices[0].x() >= 0.0 && vertices[0].y() >= 0.0) {
    l.vertex0 = t.vertex0 = vertices[0];
    l.color0 = t.color0 = XVec4f(1.0, 0.0, 0.0, alpha);
    drawPoint(t.vertex0, t.color0);
  }
        
  /* Draw the second point (square) in green, if valid) */
  if (vertices[1].x() >= 0.0 && vertices[1].y() >= 0.0) {
    l.vertex1 = t.vertex1 = vertices[1];
    l.color1 = t.color1 = XVec4f(0.0, 1.0, 0.0, 1.0);
    drawPoint(t.vertex1, t.color1);
  }
        
  /* Draw the third point (square) in blue, if valid and not double clicked on a line */
  if ((vertices[2] != vertices[1]) && (vertices[2].x() >= 0.0) && (vertices[2].y() >= 0.0)) {
    t.vertex2 = vertices[2];
    t.color2 = XVec4f(0.0, 0.0, 1.0, 1.0);
    drawPoint(t.vertex2, t.color2);
  }
        
  if (vvalid == 3) {

    t.isAntialiased = aa;

    if (clipped) {
      cerr << "Triangle clipping not supported." << endl;
      cerr << "Triangle drawn unclipped." << endl;
    }

    t.drawInRect(clipView);

    /* Draw a triangle outline between vertices */
    glLineWidth(1.0);
    glColor3f(0.0, 0.0, 0.0);
    glPushAttrib(GL_POLYGON_BIT);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(1.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    glVertex2f(t.vertex0.x()*DELTA+DELTA/2, t.vertex0.y()*DELTA+DELTA/2);
    glVertex2f(t.vertex1.x()*DELTA+DELTA/2, t.vertex1.y()*DELTA+DELTA/2);
    glVertex2f(t.vertex2.x()*DELTA+DELTA/2, t.vertex2.y()*DELTA+DELTA/2);
    glEnd();
    glPopAttrib();
    
  } else if (vvalid == 2) {

    l.isAntialiased = aa;

    if (clipped) {
      color0 = l.color0;
      color1 = l.color1;

      l.color0 = inBW(color0);
      l.color1 = inBW(color1);
      l.drawInRect(background);

      l.color0 = color0;
      l.color1 = color1;
    }

    l.drawInRect(clipView);

    /* Draw a line between vertices */
    glLineWidth(1.0);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex2f(l.vertex0.x()*DELTA+DELTA/2, l.vertex0.y()*DELTA+DELTA/2);
    glVertex2f(l.vertex1.x()*DELTA+DELTA/2, l.vertex1.y()*DELTA+DELTA/2);
    glEnd();
  }

  /* Force OpenGL to start drawing */
  glutSwapBuffers();

  return;
}

/* Mouse click event callback */
#define MAXCLICKS 3
void
mouse(int button, int state, int x, int y)
{
  int i;
  static int done=0;

  /* Trap left-clicks */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if (vvalid == 0) {
      vertices[0].x() = floorf(((float) x)/DELTA);
      vertices[0].y() = rintf(((float) (screen_height-y))/DELTA);

      done = 0;

    } else if (vvalid==1) {
      vertices[1].x() = floorf(((float) x)/DELTA);
      vertices[1].y() = rintf(((float) (screen_height-y))/DELTA);

      if (vertices[1] == vertices[0]) {
        vvalid = MAXCLICKS;
      }

    } else if (vvalid==2) {
      if (!done) {
        vertices[2].x() = floorf(((float) x)/DELTA);
        vertices[2].y() = rintf(((float) (screen_height-y))/DELTA);

        if (vertices[2] == vertices[1]) {
           vvalid = 1;
        }
        done = 1;
      } else {
        vvalid = MAXCLICKS;
        done = 0;
      }
    }

    if (++vvalid > MAXCLICKS) {
      for (i = 0; i < NVERTICES; i++) {
        vertices[i] = XVec2f(-1.0, -1.0);
      }
      vvalid = 0;
    }
    glutPostRedisplay();
  }

  return;
}

/* Keyboard callback */
void
kbd(unsigned char key, int x, int y)
{
  int i;

  switch((char)key) {
                        
  case 'a':
    aa = aa ? false : true;
    break;
    
  case 'c':
    clipped = clipped ? 0 : 1;
    if (clipped) {
      clipView = XVec4f(CMINX, CMINY, rintf(vwidth/2), rintf(vheight/2));
      background = XVec4f(0.0, 0.0, vwidth, vheight);
    } else {
      clipView = background = XVec4f(0.0, 0.0, vwidth, vheight);
    }
    break;
  
  case 'q':
    glutDestroyWindow(wd);
    exit(0);
    break;
                        
  case 't':
    if (alpha > 0.0) {
       alpha -= ALPHA_DEC;
       if (alpha < 0.0) alpha = 0.0;
    } else {
      alpha = 1.0;
    }
    break;
  
  case ' ':
  case 27:
    for (i = 0; i < NVERTICES; i++) {
      vertices[i] = XVec2f(-1.0, -1.0);
    }
    vvalid = 0;
    break;
                        
  }
  glutPostRedisplay();

  return;
}

/* OpenGL resize callback */
void
reshape(int w, int h)
{
  /* Save new screen dimensions */
  screen_width = (GLdouble) (w - w%DELTA);
  screen_height = (GLdouble) (h - h%DELTA);

  vwidth = (float) screen_width/DELTA;
  vheight = (float) screen_height/DELTA;
  if (clipped) {
    clipView = XVec4f(CMINX, CMINY, rintf(vwidth/2), rintf(vheight/2));
    background = XVec4f(0.0, 0.0, vwidth, vheight);
  } else {
    clipView = background = XVec4f(0.0, 0.0, vwidth, vheight);
  }
        
  /* Instruct Open GL to use the whole window for drawing */
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
        
  /* Do not perform projection transformations - later in the course */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
        
  /* Set coordinate system to first quadrant and bound x and y */
  glOrtho(0.0, screen_width, 0.0, screen_height, -1.f, 1.f);

  return;
}

int
main(int argc, char *argv[])
{
  /* Initialize GLUT, let it extract command-line GLUT options that
     you may provide 
     - NOTE THE & BEFORE argc
  */
  glutInit(&argc, argv);
        
  /* Want double window and color will be specified as RGBA values */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
        
  /* Set initial window size
     screen_width and screen_height have been above
  */
  glutInitWindowSize((int)screen_width, (int)screen_height);
        
  /* Create the window and store the handle to it */
  wd = glutCreateWindow("EECS PA1: Rasterization Debug" /* title */ );
        
  /* --- register callbacks for GUI events --- */
  /* NOTE prototypes for each, declared above */
        
  /* Register function that paints the window
     - Calling OpenGL functions or otherwise
  */
  glutDisplayFunc(display);
        
  /* Register function to handle window reshapes */
  glutReshapeFunc(reshape);
        
  /* Register keyboard event processing function */
  glutKeyboardFunc(kbd);
        
  /* Register mouse-click event callback */
  glutMouseFunc(mouse);
        
  /* initialize GL and program states */
  init();
  
  /* Start the GLUT event loop */
  glutMainLoop();
        
  exit(0);
}
