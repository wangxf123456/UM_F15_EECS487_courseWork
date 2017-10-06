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
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "xvec.h"
#include "objects.h"
#include "viewing.h"
#include "transforms.h"
#include <iostream>
#include <cmath>
using namespace std;

object_t object;
float sphere_radius;
enum { SPOB=0, SIDX, NOBS }; // NOBS: number of object buffers
unsigned int bods[NOBS];     // vertex buffer object handles
int countperstack;

/* draw world coordinate system's axis */
void
drawAxes(int w, int h, float alpha)
{
  glDisable(GL_LIGHTING);

  glLineWidth(2.0);
  glBegin(GL_LINES);
    glColor4f(1.0, 0.0, 0.0, alpha);
    glVertex3f(-w, 0.0, 0.0);
    glVertex3f(w, 0.0, 0.0);
    glColor4f(0.0, 1.0, 0.0, alpha);
    glVertex3f(0.0, -h, 0.0);
    glVertex3f(0.0, h, 0.0);
    glColor4f(0.0, 0.0, 1.0, alpha);
    glVertex3f(0.0, 0.0, -w);
    glVertex3f(0.0, 0.0, w);
  glEnd();

  glEnable(GL_LIGHTING);

  return;
}

#define SLICES 1000  // longitude slices
#define STACKS 1000  // latitude slices

int sphere_indices[STACKS * SLICES];

/*
 * Initialize the vertex buffer objects
 * containing the vertices and vertex attributes
 * of objects in the world.
 * For sphere, use the macros SLICES and STACKS
 * to define the sphere.
*/
void
initWorld()
{
  /*
   * YOUR CODE HERE
   * Compute the vertices of your sphere and store them
   * in a vertex array and store the normals in a corresponding normal
   * array.  Then compute the indices to access the vertex array
   * to draw the sphere using glDrawElement().  Don't forget to set
   * the normal of your vertices.  Make sure the normals are of unit
   * length, i.e., normalized. You can either do manually or by
   * enabling an OpenGL mode (find it!).
   */
  for (int i = 0; i < STACKS - 1; i++) {
    for (int j = 0; j < SLICES; j++) {
      if (j % 2 == 0) {
        sphere_indices[i * SLICES + j] = i * SLICES + j;
      } else {
        sphere_indices[i * SLICES + j] = (i + 1) * SLICES + j;
      }
    }
    sphere_indices[(i + 1) * SLICES] = sphere_indices[i * SLICES];
  }

  return;
}
  
/*
 * Draw a sphere of the given radius.
*/
void
drawSphere(double radius)
{
  /* YOUR CODE HERE: 
   * Draw your sphere using glDrawElements() and GL_TRIANGLE_STRIP
   *
   * Caveat: the last argument to glDrawElement()
   * is offset IN BYTES from the start of index[]
   * array, i.e., give the offset in terms of
   * element count * sizeof(unsigned int), not just in
   * terms of element count, or your GPU will lock
   * up and you'd have to reboot your machine.
  */
  XVec3f* sphere_vertex = new XVec3f[SLICES * STACKS];
  for (int i = 0; i < STACKS; i++) {
    for (int j = 0; j < SLICES; j++) {
      float theta = i / (float)STACKS * 2 * M_PI;
      float phi = j / (float)SLICES * 2 * M_PI;
      sphere_vertex[i * SLICES + j] = radius * XVec3f(sin(theta) * cos(phi),
                                                              sin(theta) * sin(phi),
                                                              cos(theta));
    }
  }

  glGenBuffers(NOBS, bods);
  glBindBuffer(GL_ARRAY_BUFFER, bods[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(XVec3f) * SLICES * STACKS, sphere_vertex, GL_STATIC_DRAW);

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, 0); 
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bods[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_indices), sphere_indices, GL_STATIC_DRAW);
  glDrawElements(GL_TRIANGLE_STRIP, SLICES * STACKS, GL_UNSIGNED_INT, 0);

  // glVertexPointer(3, GL_FLOAT, 0, sphere_vertex); 
  // glNormalPointer(GL_FLOAT, 0, sphere_vertex);
  // glDrawElements(GL_TRIANGLE_STRIP, SLICES * STACKS, GL_UNSIGNED_INT, sphere_indices);
  
  delete[] sphere_vertex;
  return;
}

void
drawWorld(int w, int h)
{
  static GLUquadric *q;
  
  drawAxes(1, 1, 0.4); // draw object axes

  if (!q) q = gluNewQuadric();
  gluQuadricOrientation(q, GLU_OUTSIDE);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(0.6, 0.0, 0.0);
  if (object == TORUS) {
    glutSolidTorus(0.2, 0.3, SLICES, STACKS);
  } else {
    /* YOUR CODE HERE
     * uncomment the gluSphere call to see the expected
     * look and behavior of your sphere.  Make sure
     * you comment out the call to gluSphere and uncomment
     * the call to drawSphere before you turn in your lab.
    */
    // gluSphere(q, sphere_radius, SLICES, STACKS);
    drawSphere(sphere_radius);
  }
  glTranslatef(-1.2, -0.5, 0.0);
  glRotatef(-90.0, 1.0, 0.0, 0.0);
  gluCylinder(q, 0.5, 0.0, 1.0, SLICES, STACKS);
  glPopMatrix();

  return;
}
