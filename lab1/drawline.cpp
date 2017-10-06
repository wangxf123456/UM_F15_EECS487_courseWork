/*
 * Copyright (c) 2008, 2011, 2014 University of Michigan, Ann Arbor.
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

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* Where was the mouse last clicked? */
#define NVERTICES 2
int vertex[NVERTICES][2]; /* Array of 2D points */

/* Flag to indicate the number of vertices that the user has set by clicking */
int vvalid = 0;

/* screen width and height - screen refers to
   window within which we draw*/
#define DELTA 25
GLdouble screen_width=51*DELTA, screen_height=32*DELTA;

/* Handle to the GLUT window */
int wd;

void
drawpoint(GLint x, GLint y, GLclampf r, GLclampf g, GLclampf b)
{
  glColor3f(r,g,b);
  glRecti(x*DELTA, y*DELTA, (x+1)*DELTA, (y+1)*DELTA);
  /* NOTE: you cannot call glRect within a glBegin()...glEnd() block */

  return;
}

double line_func(int vertex[NVERTICES][2], double x, double y) {
  double A = vertex[0][1] - vertex[1][1];
  double B = vertex[1][0] - vertex[0][0];
  double C = vertex[0][0] * vertex[1][1] - vertex[1][0] * vertex[0][1];
  return A * x + B * y + C;
}

void draw_line(int vertex[NVERTICES][2], int mode) {
  int y = vertex[0][1];
  int dx = vertex[1][0] - vertex[0][0];
  int dy = vertex[1][1] - vertex[0][1];
  double fmid;
  fmid = line_func(vertex, vertex[0][0] + 1, vertex[0][1] + 0.5);
  for (int x = vertex[0][0]; x <= vertex[1][0]; x++) {
    printf("*** %d %d %f\n", x , y , fmid);
    if (x == vertex[0][0] || x == vertex[1][0]) {
      if (fmid < 0) {
        y++;
        fmid += dx - dy;
      } else {
        fmid -= dy;
      }  
      continue;
    } else {
      double r = 1.0 * (x - vertex[0][0]) / (vertex[1][0] - vertex[0][0]);
      switch (mode) {
        case 0:
          drawpoint(x, y, 1.0 - r, 0.0, r);
          break;
        case 1:
          drawpoint(y, x, 1.0 - r, 0.0, r);
          break;
        case 4:
          drawpoint(x, y, r, 0.0, 1.0 - r);
          break; 
        case 5:
          drawpoint(y, x, r, 0.0, 1.0 - r);
          break;
        case 2:
          drawpoint(x, 2 * vertex[0][1] - y, 1.0 - r, 0.0, r);
          break;
        case 3:
          drawpoint(y, 2 * vertex[0][0] - x, 1.0 - r, 0.0, r);
          break; 
        case 6:
          drawpoint(x, 2 * vertex[0][1] - y, r, 0.0, 1.0 - r);
          break; 
        case 7:
          drawpoint(y, 2 * vertex[0][0] - x, r, 0.0, 1.0 - r);
          break;   
        default:
          break;
      }  
    }
    if (fmid < 0) {
      y++;
      fmid += dx - dy;
    } else {
      fmid -= dy;
    }     
  }
}

/* Display callback */
void
display(void)
{
  int i;
  
  /* Set background to white */
  glClear(GL_COLOR_BUFFER_BIT);

  /* Draw a grey grid */
  glColor3f(0.5, 0.5, 0.5);
  glLineWidth(1.0);
  glBegin(GL_LINES);
  /* vertical lines */
  for(i=0; i<screen_width; i+=DELTA) {
    glVertex2i(i,0);
    glVertex2i(i,(int)screen_height);
  }
  /* horizontal lines */
  for (i=0; i<screen_height; i+=DELTA) {
    glVertex2i(0,i);
    glVertex2i((int)screen_width, i);
  }
  glEnd();

  /* Draw the first point (square) in red, if value) */
  if (vertex[0][0]>=0 && vertex[0][1]>=0) {
    drawpoint(vertex[0][0], vertex[0][1], 1.0, 0.0, 0.0);
  }
  
  /* Draw the second point (square) in blue, if value) */
  if (vertex[1][0]>=0 && vertex[1][1]>=0) {
    drawpoint(vertex[1][0], vertex[1][1], 0.0, 0.0, 1.0);
  }
  
  /* Draw straight line connecting end points if both are valid*/
  if (vvalid == 2) {
    glLineWidth(3.0);
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex2i(vertex[0][0]*DELTA+DELTA/2, vertex[0][1]*DELTA+DELTA/2);
    glVertex2i(vertex[1][0]*DELTA+DELTA/2, vertex[1][1]*DELTA+DELTA/2);
    glEnd();
    
    /* TODO - implement the line drawing algorithm here
       int vertex[0], vertex[1] store integer coordinates of the 
       'endpoint pixels' The support code does the task of trapping 
       mouse clicks and setting these
       NOTE: Interpolate the color between the endpoints to get a 
       gradual transition
    */
    double slope = double(vertex[1][1] - vertex[0][1]) / (vertex[1][0] - vertex[0][0]);
    printf("%d, %d, %d, %d\n", vertex[0][0], vertex[0][1], vertex[1][0], vertex[1][1]);
    printf("%f", slope);
    int base = 0;
    if (vertex[1][0] < vertex[0][0]) {
      base = 4;
      int temp = vertex[1][0];
      vertex[1][0] = vertex[0][0];
      vertex[0][0] = temp;
      temp = vertex[1][1];
      vertex[1][1] = vertex[0][1];
      vertex[0][1] = temp;
    } 
    if (slope <= 1 && slope >= 0) {
      draw_line(vertex, base);
    } else if (slope > 1) {
      int temp = vertex[0][0];
      vertex[0][0] = vertex[0][1];
      vertex[0][1] = temp;
      temp = vertex[1][0];
      vertex[1][0] = vertex[1][1];
      vertex[1][1] = temp;
      draw_line(vertex, base + 1);
    } else if (slope < 0 && slope >= -1) {
      vertex[1][1] = 2 * vertex[0][1] - vertex[1][1];
      draw_line(vertex, base + 2);
    } else {
      vertex[1][1] = 2 * vertex[0][1] - vertex[1][1];
      int temp = vertex[0][0];
      vertex[0][0] = vertex[0][1];
      vertex[0][1] = temp;
      temp = vertex[1][0];
      vertex[1][0] = vertex[1][1];
      vertex[1][1] = temp;       
      draw_line(vertex, base + 3);       
    }
  }
  
  /* Force OpenGL to start drawing */
  // glFlush();
  glutSwapBuffers();

  return;
}

/* Mouse click event callback */
void
mouse(int button, int state, int x, int y)
{
  int i;

  /* Trap left-clicks */
  if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if(vvalid==0) {
      vertex[0][0] = x/DELTA;
      vertex[0][1] = (((int) screen_height)-y)/DELTA;
      ++vvalid;
    } else if(vvalid==1) {
      vertex[1][0] = x/DELTA;
      vertex[1][1] = (((int) screen_height)-y)/DELTA;
      ++vvalid;
    } else {
      for (i = 0; i < NVERTICES; i++) {
        vertex[i][0] = -1;
        vertex[i][1] = -1;
      }
      vvalid = 0;
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
  case 'q':
    glutDestroyWindow(wd);
    exit(0);
    break;
                        
  case ' ':
  case 27:
    for (i = 0; i < NVERTICES; i++) {
      vertex[i][0] = -1;
      vertex[i][1] = -1;
    }
    vvalid = 0;
    glutPostRedisplay();
    break;
                        
  }

  return;
}

/* OpenGL resize callback */
void
reshape(int w, int h)
{
  /* Save new screen dimensions */
  screen_width = (GLfloat) w;
  screen_height = (GLfloat) h;
  
  /* Instruct Open GL to use the whole window for drawing */
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  
  /* No projection transformation */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /* Set coordinate system to first quadrant and bound x and y */
  glOrtho(0.0, screen_width, 0.0, screen_height, -1.f, 1.f);

  return;
}

int
main(int argc, char *argv[])
{
  int i;

  /* Initialize GLUT, let it extract command-line GLUT options that
     you may provide 
     - NOTE THE & BEFORE argc
  */
  glutInit(&argc, argv);
  
  /* Want single window and color will be specified as RGBA values */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

  /* Set initial window size
     screen_width and screen_height have been above
  */
  glutInitWindowSize((int)screen_width, (int)screen_height);

  /* Create the window and store the handle to it */
  wd = glutCreateWindow("Experiment with line drawing" /* title */ );

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
  
  /* -- init data -- */
  for (i = 0; i < NVERTICES; i++) {
    vertex[i][0] = -1;
    vertex[i][1] = -1;
  }
  vvalid = 0;

  /* -- init gl -- */
  glClearColor(1.0, 1.0, 1.0, 1.0);
  /* Fill the 'front' face of polygons - rectangles in our case */
  glPolygonMode(GL_FRONT, GL_FILL); 

  /* Start the GLUT event loop */
  glutMainLoop();

  exit(0);
}
