/*
 * Copyright (c) 2007, 2011, 2012 University of Michigan, Ann Arbor.
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
 *
*/
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "ltga.h"
#include "xvec.h"
#include "objects.h"

#define SLICES 1000 //72 /* number of azimuthal (longitudinal) partitions */
#define STACKS 1000 //36 /* number of latitudinal partitions */

enum { SPHERE=0, SIDX, NOBS };

typedef struct {
  XVec3f position;
  XVec2f texcoords;
} postex_t;

void
init_sphere()
{
  GLuint vbods[NOBS];
  int i, j, k, k0, k1;
  double theta, phi;
  double fullcircle = M_PI*2.0;
  double inc_theta = M_PI/((double) STACKS);
  double laststack = M_PI-inc_theta;
  double inc_phi = fullcircle/((double) SLICES);
  double costheta, sintheta;
  double cosphi, sinphi;
  int midrift;
  postex_t *vertices;
  GLuint *vertidx;

  glGenBuffers(NOBS, vbods);
  glBindBuffer(GL_ARRAY_BUFFER, vbods[SPHERE]);
  /* Allocate enough space for the vertex array
   * (3-coords position element) (= normal array
   * for a smooth sphere) and the texture
   * coordinates array (2-coords element)
   * Total: 5*sizeof(float) = sizeof(postex_t)
   */
  glBufferData(GL_ARRAY_BUFFER, (SLICES+1)*(STACKS+1)*sizeof(postex_t), 
               0, GL_STATIC_DRAW);

  /* TASK 4: YOUR CODE HERE
   * Map the allocated buffer to an array of postex_t,
   * which interleaves the position and texcoords of
   * each vertex.
   *
   * Replace the following line with your code
  */
  vertices = (postex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
  
  /* Sphere */
  for (i = SLICES+1, theta = inc_theta;
       theta <= laststack; theta += inc_theta) {
    // assume r = 1.0;
    costheta = cos(theta);
    sintheta = sin(theta);

    for (phi = 0.0; phi <= fullcircle; phi += inc_phi, i++) {
      cosphi = cos(phi);
      sinphi = sin(phi);
      vertices[i].position.x() = sintheta*cosphi;
      vertices[i].position.y() = sintheta*sinphi;
      vertices[i].position.z() = costheta;

      /* TASK 1: YOUR CODE HERE assign texture coordinates per vertex */
      // vertices[i].texcoords.x() = 0.5 + atan2(vertices[i].position.y(), vertices[i].position.x()) / (2 * M_PI);
      // vertices[i].texcoords.y() = acos(vertices[i].position.z()) / M_PI;
      vertices[i].texcoords.x() = phi / fullcircle;
      vertices[i].texcoords.y() = theta / laststack;
    }
  }

  for (j = 0, phi = 0.0; j <= SLICES; j++, phi += inc_phi, i++) {
    // north pole
    vertices[j].position = XVec3f(0.0, 0.0, 1.0);
    // south pole
    vertices[i].position = XVec3f(0.0, 0.0, -1.0);

    /* TASK 1: YOUR CODE HERE: assign texture coordinates to the pole vertices */
    vertices[j].texcoords.y() = 0.5;
    vertices[j].texcoords.x() = 0.5;
    vertices[i].texcoords.y() = 0.3;
    vertices[i].texcoords.x() = 0.5;
  }

  /* TASK 4: YOUR CODE HERE:
   * Release the mapped buffer.
   *
   * Replace the following 3 lines with your code
  */
  glUnmapBuffer(GL_ARRAY_BUFFER);

  // glBufferSubData(GL_ARRAY_BUFFER, 0,
  //              (SLICES+1)*(STACKS+1)*sizeof(postex_t), vertices);
  // free(vertices);

  /* Enable client-side vertex position and normal attributes
   * and set up pointer to the arrays.  Since we're interleaving
   * the vertex position (XVec3f) and texcoords (XVec2f), we need
   * to specify a stride of sizeof(XVec3f)+sizeof(XVec2f).
   */
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(postex_t), 0);
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(postex_t), 0);
  /* Automatic normalization of normals */
  glEnable(GL_NORMALIZE);

  /* TASK 1: YOUR CODE HERE:
   * Enable client-side vertex texture coordinates attribute
   * array and set up pointer to the array.  The first element
   * of the texture-coordinates array is after the first vertex
   * position coordinates.  Since we're interleaving
   * the vertex position (XVec3f) and texcoords (XVec2f),
   * subsequent texture coordinates are at stride
   * sizeof(XVec2f)+sizeof(XVec3f) bytes from the start
   * of the current texture coordinates.
   *
   * Caveat: make sure your texture coordinate pointer is
   * set to BYTE offset from the start of the mapped buffer
   * object, i.e., don't forget to multiply the count by
   * sizeof(float).
  */

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(postex_t), (GLvoid*)sizeof(XVec3f));
  /* Bind the GL_ELEMENT_ARRAY_BUFFER and allocate enough space
   * in graphics system memory to hold the element index
   * array to be used by glDrawElement().
  */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[SIDX]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, (SLICES+1)*STACKS*2*sizeof(GLuint),
               0, GL_STATIC_DRAW); 

  /* TASK 4: YOUR CODE HERE:
   * Map the allocated buffer to the "index" variable.
   * allocate enough space for the index array
   * for use by glDrawElement().
   *
   * Replace the following line with your code
   */
  vertidx = (GLuint *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
  // vertidx = (GLuint *) malloc((SLICES+1)*STACKS*2*sizeof(GLuint));

  // triangle strip
  //   - the poles cannot use triangle fan due to texcoords
  midrift = STACKS*(SLICES+1);  
  for (i=0, k=0, k0=0, k1=SLICES+1; 
       i < midrift;
       i++, k += 2, k0++, k1++) {
    vertidx[k] = k0;
    vertidx[k+1] = k1;
  }

  /* TASK 4: YOUR CODE HERE: 
   * Release the mapped index buffer.
   *
   * Replace the following two calls with your code
  */
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  // glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
  //                 (SLICES+1)*STACKS*2*sizeof(GLuint),
  //                 vertidx);
  // free(vertidx);
  
  return;
}

void
draw_sphere()
{
  int i;
  static const int countperstack = 2*(SLICES+1); 

  for (i = 0; i < STACKS; i++) {
    glDrawElements(GL_TRIANGLE_STRIP, countperstack, GL_UNSIGNED_INT, 
                   (GLvoid *)(i*countperstack*sizeof(GLuint)));
  }

  return;
}
