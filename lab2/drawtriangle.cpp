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
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

/* Include the GL headers */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* Where was the mouse last clicked? */
#define NVERTICES 3
int vertex[NVERTICES][2]; /* Array of 2D points */

/* Flag to indicate the number of vertices that the user has set by clicking */
int vvalid=0;

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

vector<vector<int> > get_bound(int vertex[NVERTICES][2]);

class line_eqn {
  private:
    int A;
    int B;
    int C;

  public:
    line_eqn(int v1[2], int v2[2]) {
      A = v1[1] - v2[1];
      B = v2[0] - v1[0];
      C = v1[0] * v2[1] - v2[0] * v1[1];
    }
    void flip() {
      A = -A;
      B = -B;
      C = -C;
    }

    double calulate(int v[2]) {
      return A * v[0] + B * v[1] + C;
    }

    double calulate(int x, int y) {
      return A * x + B * y + C;
    }
};

int get_min(int vertex[NVERTICES][2], bool is_y) {
  int index = 0;
  if (is_y) {
    index = 1;
  }
  int result = vertex[0][index];
  if (min(result, vertex[1][index]) > vertex[2][index]) {
    return vertex[2][index];
  } else {
    return min(result, vertex[1][index]);
  }
}

int get_max(int vertex[NVERTICES][2], bool is_y) {
  int index = 0;
  if (is_y) {
    index = 1;
  }
  int result = vertex[0][index];
  if (max(result, vertex[1][index]) < vertex[2][index]) {
    return vertex[2][index];
  } else {
    return max(result, vertex[1][index]);
  }
}

vector<double> get_area(int vertex[NVERTICES][2], int x, int y) {
  vector<double> result(4);
  int ux = vertex[0][0] - vertex[1][0];
  int uy = vertex[0][1] - vertex[1][1];
  int vx = vertex[0][0] - vertex[2][0];
  int vy = vertex[0][1] - vertex[2][1];
  result[0] = abs(ux * vy - uy * vx);
  ux = x - vertex[0][0];
  uy = y - vertex[0][1];
  vx = vertex[1][0] - vertex[0][0];
  vy = vertex[1][1] - vertex[0][1];
  result[1] = abs(ux * vy - uy * vx);
  ux = x - vertex[1][0];
  uy = y - vertex[1][1];
  vx = vertex[2][0] - vertex[1][0];
  vy = vertex[2][1] - vertex[1][1];
  result[2] = abs(ux * vy - uy * vx);  
  ux = x - vertex[2][0];
  uy = y - vertex[2][1];
  vx = vertex[0][0] - vertex[2][0];
  vy = vertex[0][1] - vertex[2][1];
  result[3] = abs(ux * vy - uy * vx);
  return result;
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
  for (i=0; i<screen_width; i+=DELTA) {
    glVertex2i(i,0);
    glVertex2i(i,screen_height);
  }
  /* horizontal lines */
  for (i=0; i<screen_height; i+=DELTA) {
    glVertex2i(0,i);
    glVertex2i(screen_width, i);
  }
  glEnd();
        
  /* Draw the first point (square) in red, if valid) */
  if (vertex[0][0]>=0 && vertex[0][1]>=0)
    drawpoint(vertex[0][0], vertex[0][1], 1.0, 0.0, 0.0);
        
  /* Draw the second point (square) in green, if valid) */
  if (vertex[1][0]>=0 && vertex[1][1]>=0)
    drawpoint(vertex[1][0], vertex[1][1], 0.0, 1.0, 0.0);
        
  /* Draw the third point (square) in blue, if valid) */
  if (vertex[2][0]>=0 && vertex[2][1]>=0)
    drawpoint(vertex[2][0], vertex[2][1], 0.0, 0.0, 1.0);
        
  if (vvalid == 3) {
    /* Draw solid, black triangle between vertices */
    glLineWidth(3.0);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex2i(vertex[0][0]*DELTA+DELTA/2, vertex[0][1]*DELTA+DELTA/2);
    glVertex2i(vertex[1][0]*DELTA+DELTA/2, vertex[1][1]*DELTA+DELTA/2);
    glVertex2i(vertex[2][0]*DELTA+DELTA/2, vertex[2][1]*DELTA+DELTA/2);
    glEnd();
                
    /* TODO - implement the triangle rasterization algorithm here.
       int vertex[0], vertex[1] and vertex[2] store integer coordinates 
       of the vertices.  The support code does the task of trapping mouse 
       clicks and setting these
       NOTE: Interpolate the color between the endpoints to get a 
       gradual transition
    */
    /* YOUR CODE HERE */
    line_eqn l1 = line_eqn(vertex[0], vertex[1]);
    line_eqn l2 = line_eqn(vertex[1], vertex[2]);
    line_eqn l3 = line_eqn(vertex[2], vertex[0]);
    if (l1.calulate(vertex[2]) < 0) {
      l1.flip();
    }
    if (l2.calulate(vertex[0]) < 0) {
      l2.flip();
    }
    if (l3.calulate(vertex[1]) < 0) {
      l3.flip();
    }
    int minx = get_min(vertex, false);
    int miny = get_min(vertex, true);
    int maxx = get_max(vertex, false);
    int maxy = get_max(vertex, true);
    // cout << minx << " " << miny << " " << maxx << " " << maxy << endl;
    for (int i = minx; i <= maxx; i++) {
      for (int j = miny; j <= maxy; j++) {
        if (l1.calulate(i, j) >= 0 && l2.calulate(i, j) >= 0 && l3.calulate(i, j) >= 0) {
          vector<double> areas = get_area(vertex, i, j);
          double gamma = areas[1] / areas[0];
          double alpha = areas[2] / areas[0];
          double beta = areas[3] / areas[0];
          if (minx != maxx && miny != maxy) {
            drawpoint(i, j, alpha, beta, gamma);
          }
        }
      }
    }
  }

  /* Force OpenGL to start drawing */
  glutSwapBuffers();
  
  return;
}

/* Mouse click event callback */
void
mouse(int button, int state, int x, int y)
{
  int i;

  /* Trap left-clicks */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    if (vvalid==0) {
      vertex[0][0] = x/DELTA;
      vertex[0][1] = (((int) screen_height)-y)/DELTA;
      ++vvalid;
    } else if (vvalid==1) {
      vertex[1][0] = x/DELTA;
      vertex[1][1] = (((int) screen_height)-y)/DELTA;
      ++vvalid;
    } else if (vvalid==2) {
      vertex[2][0] = x/DELTA;
      vertex[2][1] = (((int) screen_height)-y)/DELTA;
      ++vvalid;
    } else {
      for (i = 0; i < NVERTICES; i++) {
        vertex[i][0] = -1;
        vertex[i][1] = -1;
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
  screen_width = (GLdouble) w;
  screen_height = (GLdouble) h;
        
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
  wd = glutCreateWindow("Lab2: Triangle Rasterization" /* title */ );
        
  /* --- register callbacks for GUI events --- */
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
        
  /* -- init GL -- */
  glClearColor(1.0, 1.0, 1.0, 1.0);
  /* Fill the 'front' face of polygons - rectangles in our case */
  glPolygonMode(GL_FRONT, GL_FILL); 

  /* Start the GLUT event loop */
  glutMainLoop();
        
  exit(0);
}
