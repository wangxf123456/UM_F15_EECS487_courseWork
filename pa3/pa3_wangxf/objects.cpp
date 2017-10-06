/*
 * Copyright (c) 2012 University of Michigan, Ann Arbor.
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
 * Author: Sugih Jamin
 *
*/
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "ltga.h"
#include "xvec.h"
#include "objects.h"

static GLenum gl_error;

#define SLICES 1000 /* number of azimuthal (longitudinal) partitions */
#define STACKS 1000 /* number of latitudinal partitions */

extern GLuint spd;
extern GLuint *tods;
extern int ntods;
extern GLuint *nmods;
extern int nnmods;

enum { OBJ, IDX, NOBS };

unsigned int vaods[NOBJS]; 

typedef struct {
  XVec3f position;
  XVec2f texcoords;
  /* TASK 7: YOUR CODE HERE
   * add a vertex attribute for tangent */
  XVec3f tangent;
} sphere_vertex_t;

typedef struct {
  XVec3f position;
  XVec3f normal;
  XVec2f texcoords;
  XVec3f tangent;
} cube_vertex_t;

bool
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
  sphere_vertex_t *vertices;
  GLuint *vertidx;

  glGenBuffers(NOBS, vbods);
  if ((gl_error = glGetError()) != GL_NO_ERROR) {
    cerr << "GenBuffers sphere: [Error " 
         << gl_error << "] " << gluGetString(gl_error) << endl;
    exit(1);
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbods[OBJ]);
  if ((gl_error = glGetError()) != GL_NO_ERROR) {
    cerr << "BindBuffer sphere: [Error " 
         << gl_error << "] " << gluGetString(gl_error) << endl;
    exit(1);
  }

  /* Allocate enough space for the vertex array
   * (3-coords position element) (= normal array
   * for a smooth sphere) and the texture
   * coordinates array (2-coords element)
   * Total: 5*sizeof(float) = sizeof(sphere_vertex_t)
   */
  glBufferData(GL_ARRAY_BUFFER, (SLICES+1)*(STACKS+1)*sizeof(sphere_vertex_t), 
               0, GL_STATIC_DRAW);
  if ((gl_error = glGetError()) != GL_NO_ERROR) {
    cerr << "BufferData sphere: [Error " 
         << gl_error << "] " << gluGetString(gl_error) << endl;
    exit(1);
  }

  /*
   * TASK 1: replace the following line with your code from Lab6      
   *
   * Map the allocated buffer to an array of sphere_vertex_t,
   * which interleaves the position and texcoords of
   * each vertex.
  */

  vertices = (sphere_vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

  /* Sphere */                          // we'll do the north pole together
  for (i = SLICES+1, theta = inc_theta; // with the south pole later
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

      /* TASK 1: copy your code from Lab6 */
      /* assign texture coordinates per vertex */
      vertices[i].texcoords.x() = phi / fullcircle;
      vertices[i].texcoords.y() = theta / laststack;

    }
  }

  for (j = 0, phi = 0.0; j <= SLICES; j++, phi += inc_phi, i++) {
    // north pole, half of the triangles become lines, but
    // the result looks fine and it simplifies the code . . . .
    vertices[j].position = XVec3f(0.0, 0.0, 1.0);
    // south pole
    vertices[i].position = XVec3f(0.0, 0.0, -1.0);

    /* TASK 1: copy your code from Lab6 */
    /* assign texture coordinates to the pole vertices */
    vertices[j].texcoords.y() = 1.0;
    vertices[j].texcoords.x() = phi / fullcircle;
    vertices[i].texcoords.y() = 0.0;
    vertices[i].texcoords.x() = phi / fullcircle;
  }

  /* TASK 7:
   * Compute tangent of each triangle and assign
   * them as attributes of the three vertices
   * Don't forget to wrap around at the seam.
  */

  /*
   * TASK 1: replace the following three lines
   * with your code from Lab6
   *
   * Release the mapped buffer.
  */

  glUnmapBuffer(GL_ARRAY_BUFFER);

  /* TASK 5: YOUR CODE HERE
   * get shader vertex position and normal attribute locations
   */

  GLuint vert = glGetAttribLocation(spd, "va_Position" );
  GLuint normal = glGetAttribLocation(spd, "va_Normal");

  /* TASK 6: YOUR CODE HERE
   * get shader vertex texture coordinates locations
   */

  GLuint texcoords = glGetAttribLocation(spd, "va_TexCoords");

  /* TASK 7: YOUR CODE HERE
   * get shader tangent attribute locations
   */

  GLuint tangent = glGetAttribLocation(spd, "va_Tangent");
  /* Enable client-side vertex position and normal attributes
   * and set up pointer to the arrays.  Since we're interleaving
   * the vertex position (XVec3f) and texcoords (XVec2f), we need
   * to specify a stride of sizeof(XVec3f)+sizeof(XVec2f).
   */

  /* TASK 5: YOUR CODE HERE
   * Comment out the five function calls above
   * and replace them with shader version.
   */

  glEnableVertexAttribArray(vert);
  glVertexAttribPointer(vert, 3, GL_FLOAT, GL_FALSE, sizeof(sphere_vertex_t), 0);
  glEnableVertexAttribArray(normal);
  glVertexAttribPointer(normal, 3, GL_FLOAT, GL_TRUE, sizeof(sphere_vertex_t), 0);

  /* 
   * TASK 1: copy your code from Lab6
   *
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
   * object.
  */

  glEnableVertexAttribArray(texcoords);
  glVertexAttribPointer(texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(sphere_vertex_t), (GLvoid*)sizeof(XVec3f));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[IDX]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, (SLICES+1)*STACKS*2*sizeof(GLuint),
               0, GL_STATIC_DRAW); 

  /* TASK 6: YOUR CODE HERE
   *
   * Replace the client-side vertex texture coordinates attribute
   * array and pointer to it in TASK 1 above with shader version.
   */

  /* TASK 7: YOUR CODE HERE
   * Enable shader vertex tangent attribute
   * and set up pointer to the array.  The first element
   * of the tangent array is after the first vertex
   * position and texture coordinates.
   */

  glEnableVertexAttribArray(tangent);
  glVertexAttribPointer(tangent, 3, GL_FLOAT, GL_TRUE, sizeof(sphere_vertex_t), 
        (GLvoid*)(sizeof(XVec3f) + sizeof(XVec2f)));   

  /* Bind the GL_ELEMENT_ARRAY_BUFFER and allocate enough space
   * in graphics system memory to hold the element index
   * array to be used by glDrawElement().
  */

  /*
   * TASK 1: replace the line below with your code from Lab6
   *
   * Map the allocated buffer to the "index" variable.
   * allocate enough space for the index array
   * for use by glDrawElement().
   */
  vertidx = (GLuint *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);

  // triangle strip
  //   - the poles cannot use triangle fan due to texcoords
  midrift = STACKS*(SLICES+1);  
  for (i=0, k=0, k0=0, k1=SLICES+1; 
       i < midrift;
       i++, k += 2, k0++, k1++) {
    vertidx[k] = k0;
    vertidx[k+1] = k1;
  }

  for (int i = 0; i + 2 < 2 * STACKS*(SLICES+1); i++) {
    XVec3f p0 = vertices[vertidx[i]].position;
    XVec3f p1 = vertices[vertidx[i + 1]].position;
    XVec3f p2 = vertices[vertidx[i + 2]].position;
    XVec3f p1_p0 = p1 - p0;
    XVec3f p2_p0 = p2 - p0;

    XVec2f st0 = vertices[vertidx[i + 1]].texcoords - vertices[vertidx[i]].texcoords;
    XVec2f st1 = vertices[vertidx[i + 2]].texcoords - vertices[vertidx[i]].texcoords;

    XVec3f t = 1.0 / (st0.x() * st1.y() - st1.x() * st0.y()) 
      * (p1_p0 * st1.y() - p2_p0 * st0.y());

    // average tangent vector
    vertices[vertidx[i]].tangent += t;
    vertices[vertidx[i + 1]].tangent += t;
    vertices[vertidx[i + 2]].tangent += t;     
  }

  /* 
   * TASK 1: replace the following two calls with your code from Lab6
   *
   * Release the mapped index buffer.
  */
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  
  return true;
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


bool
init_cube()
{
  /*
   * TASK 2: YOUR CODE HERE
   * Initialize a cube.
   */
  GLuint vbods[NOBS];
  cube_vertex_t *vertices;
  GLuint *vertidx;

  glGenBuffers(NOBS, vbods);
  if ((gl_error = glGetError()) != GL_NO_ERROR) {
    cerr << "GenBuffers sphere: [Error " 
         << gl_error << "] " << gluGetString(gl_error) << endl;
    exit(1);
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbods[OBJ]);
  if ((gl_error = glGetError()) != GL_NO_ERROR) {
    cerr << "BindBuffer sphere: [Error " 
         << gl_error << "] " << gluGetString(gl_error) << endl;
    exit(1);
  }

  glBufferData(GL_ARRAY_BUFFER, 36*sizeof(cube_vertex_t), 
               0, GL_STATIC_DRAW);
  if ((gl_error = glGetError()) != GL_NO_ERROR) {
    cerr << "BufferData cube: [Error " 
         << gl_error << "] " << gluGetString(gl_error) << endl;
    exit(1);
  }

  vertices = (cube_vertex_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

  // vertices data
  // extremely ugly but I dont know how to use loops to do this
  vertices[0].position = XVec3f(-0.5, -0.5, 0.5);
  vertices[1].position = XVec3f(0.5, -0.5, 0.5);
  vertices[2].position = XVec3f(-0.5, 0.5, 0.5);
  vertices[3].position = XVec3f(0.5, -0.5, 0.5);
  vertices[4].position = XVec3f(0.5, 0.5, 0.5);
  vertices[5].position = XVec3f(-0.5, 0.5, 0.5);

  vertices[6].position = XVec3f(0.5, -0.5, 0.5);
  vertices[7].position = XVec3f(0.5, -0.5, -0.5);
  vertices[8].position = XVec3f(0.5, 0.5, 0.5);
  vertices[9].position = XVec3f(0.5, -0.5, -0.5);
  vertices[10].position = XVec3f(0.5, 0.5, -0.5);
  vertices[11].position = XVec3f(0.5, 0.5, 0.5);

  vertices[12].position = XVec3f(0.5, -0.5, -0.5);
  vertices[13].position = XVec3f(-0.5, -0.5, -0.5);
  vertices[14].position = XVec3f(0.5, 0.5, -0.5);
  vertices[15].position = XVec3f(-0.5, -0.5, -0.5);
  vertices[16].position = XVec3f(-0.5, 0.5, -0.5);
  vertices[17].position = XVec3f(0.5, 0.5, -0.5);

  vertices[18].position = XVec3f(-0.5, -0.5, -0.5);
  vertices[19].position = XVec3f(-0.5, -0.5, 0.5);
  vertices[20].position = XVec3f(-0.5, 0.5, -0.5);
  vertices[21].position = XVec3f(-0.5, -0.5, 0.5);
  vertices[22].position = XVec3f(-0.5, 0.5, 0.5);
  vertices[23].position = XVec3f(-0.5, 0.5, -0.5);

  vertices[24].position = XVec3f(-0.5, 0.5, 0.5);
  vertices[25].position = XVec3f(0.5, 0.5, 0.5);
  vertices[26].position = XVec3f(-0.5, 0.5, -0.5);
  vertices[27].position = XVec3f(0.5, 0.5, 0.5);
  vertices[28].position = XVec3f(0.5, 0.5, -0.5);
  vertices[29].position = XVec3f(-0.5, 0.5, -0.5);

  vertices[30].position = XVec3f(-0.5, -0.5, -0.5);
  vertices[31].position = XVec3f(0.5, -0.5, -0.5);
  vertices[32].position = XVec3f(0.5, -0.5, 0.5);
  vertices[33].position = XVec3f(-0.5, -0.5, -0.5);
  vertices[34].position = XVec3f(0.5, -0.5, 0.5);
  vertices[35].position = XVec3f(-0.5, -0.5, 0.5);

  for (int i = 0; i < 6; i++) {
    vertices[i].normal = XVec3f(0.0, 0.0, 1.0);
  }
  for (int i = 6; i < 12; i++) {
    vertices[i].normal = XVec3f(1.0, 0.0, 0.0);
  }
  for (int i = 12; i < 18; i++) {
    vertices[i].normal = XVec3f(0.0, 0.0, -1.0);
  }
  for (int i = 18; i < 24; i++) {
    vertices[i].normal = XVec3f(-1.0, 0.0, 0.0);
  }
  for (int i = 24; i < 30; i++) {
    vertices[i].normal = XVec3f(0.0, 1.0, 0.0);
  }
  for (int i = 30; i < 36; i++) {
    vertices[i].normal = XVec3f(0.0, -1.0, 0.0);
  }
  for (int i = 0; i < 5; i++) {
    vertices[i * 6].texcoords = XVec2f(1.0, 1.0);
    vertices[i * 6 + 1].texcoords = XVec2f(0.0, 1.0);
    vertices[i * 6 + 2].texcoords = XVec2f(1.0, 0.0);
    vertices[i * 6 + 3].texcoords = XVec2f(0.0, 1.0);
    vertices[i * 6 + 4].texcoords = XVec2f(0.0, 0.0);
    vertices[i * 6 + 5].texcoords = XVec2f(1.0, 0.0);    
  }
  for (int i = 5; i < 6; i++) {
    vertices[i * 6].texcoords = XVec2f(1.0, 1.0);
    vertices[i * 6 + 1].texcoords = XVec2f(0.0, 1.0);
    vertices[i * 6 + 2].texcoords = XVec2f(0.0, 0.0);
    vertices[i * 6 + 3].texcoords = XVec2f(1.0, 1.0);
    vertices[i * 6 + 4].texcoords = XVec2f(0.0, 0.0);
    vertices[i * 6 + 5].texcoords = XVec2f(1.0, 0.0);    
  }

  for (int i = 0; i < 12; i++) {
    XVec3f p0 = vertices[i * 3].position;
    XVec3f p1 = vertices[i * 3 + 1].position;
    XVec3f p2 = vertices[i * 3 + 2].position;
    XVec3f p1_p0 = p1 - p0;
    XVec3f p2_p0 = p2 - p0;

    XVec2f st0 = vertices[i * 3 + 1].texcoords - vertices[i * 3].texcoords;
    XVec2f st1 = vertices[i * 3 + 2].texcoords - vertices[i * 3].texcoords;

    XVec3f t = 1.0 / (st0.x() * st1.y() - st1.x() * st0.y()) 
      * (p1_p0 * st1.y() - p2_p0 * st0.y());
    vertices[i * 3].tangent = t;
    vertices[i * 3 + 1].tangent = t;
    vertices[i * 3 + 2].tangent = t;     
  }

  // average tangent
  XVec3f t0 = vertices[0].tangent + vertices[19].tangent + 
              vertices[21].tangent + vertices[35].tangent;
  XVec3f t1 = vertices[1].tangent + vertices[3].tangent + 
              vertices[6].tangent + vertices[32].tangent + vertices[34].tangent;
  XVec3f t2 = vertices[4].tangent + vertices[8].tangent + 
              vertices[11].tangent + vertices[25].tangent + vertices[27].tangent;
  XVec3f t3 = vertices[2].tangent + vertices[5].tangent + 
              vertices[22].tangent + vertices[24].tangent;
  XVec3f t4 = vertices[13].tangent + vertices[15].tangent + 
              vertices[18].tangent + vertices[30].tangent + vertices[33].tangent;
  XVec3f t5 = vertices[7].tangent + vertices[9].tangent + 
              vertices[12].tangent + vertices[31].tangent;
  XVec3f t6 = vertices[10].tangent + vertices[14].tangent + 
              vertices[17].tangent + vertices[28].tangent;
  XVec3f t7 = vertices[16].tangent + vertices[20].tangent + 
              vertices[23].tangent + vertices[26].tangent + vertices[29].tangent;
  vertices[0].tangent = vertices[19].tangent = vertices[21].tangent = 
                        vertices[35].tangent = t0;
  vertices[1].tangent = vertices[3].tangent = vertices[6].tangent = 
                        vertices[32].tangent = vertices[34].tangent = t1;
  vertices[4].tangent = vertices[8].tangent = vertices[11].tangent =
                        vertices[25].tangent = vertices[27].tangent = t2;
  vertices[2].tangent = vertices[5].tangent = vertices[22].tangent
                      = vertices[24].tangent = t3;
  vertices[13].tangent = vertices[15].tangent = vertices[18].tangent =
                         vertices[30].tangent = vertices[33].tangent = t4;
  vertices[7].tangent = vertices[9].tangent = vertices[12].tangent =
                        vertices[31].tangent = t5;
  vertices[10].tangent = vertices[14].tangent = vertices[17].tangent =
                        vertices[28].tangent = t6;
  vertices[16].tangent = vertices[20].tangent = vertices[23].tangent =
                        vertices[26].tangent = vertices[29].tangent = t7;

  glUnmapBuffer(GL_ARRAY_BUFFER);

  GLuint vert = glGetAttribLocation(spd, "va_Position" );
  GLuint normal = glGetAttribLocation(spd, "va_Normal");
  GLuint texcoords = glGetAttribLocation(spd, "va_TexCoords");
  GLuint tangent = glGetAttribLocation(spd, "va_Tangent");

  glEnableVertexAttribArray(vert);
  glVertexAttribPointer(vert, 3, GL_FLOAT, GL_FALSE, sizeof(cube_vertex_t), 0);
  glEnableVertexAttribArray(normal);
  glVertexAttribPointer(normal, 3, GL_FLOAT, GL_TRUE, sizeof(cube_vertex_t), (GLvoid*)sizeof(XVec3f));
  glEnableVertexAttribArray(tangent);
  glVertexAttribPointer(tangent, 3, GL_FLOAT, GL_TRUE, sizeof(sphere_vertex_t), 
        (GLvoid*)(2*sizeof(XVec3f) + sizeof(XVec2f)));   
  
  glEnableVertexAttribArray(texcoords);
  glVertexAttribPointer(texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(cube_vertex_t), (GLvoid*)(2*sizeof(XVec3f)));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[IDX]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36*sizeof(GLuint),
               0, GL_STATIC_DRAW); 
  vertidx = (GLuint *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);

  for (int i = 0; i < 36; i++) {
    vertidx[i] = i;
  }
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  return true;
}

void
draw_cube()
{
  /*
   * TASK 2: YOUR CODE HERE
   * Draw your cube.
   */

  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 
                 0);

  return;
}


bool
init_world(objType drawobject)
{
  extern bool scene;
  
  switch (drawobject) {
  case SPHERE:
    return(init_sphere());
    break;
  case CUBE:
    return(init_cube());
    break;
  default:
    scene = true;

    /* TASK 3: YOUR CODE HERE
     *
     * Generate NOBJS vertex-array objects, then
     * bind to the SPHERE handle and call init_sphere(),
     * and bind to the CUBE handle and call init_cube().
    */
    glGenVertexArrays(NOBJS, vaods);
    glBindVertexArray(vaods[SPHERE]);
    init_sphere();
    glBindVertexArray(vaods[CUBE]);
    init_cube();
    break;
  }
  
  return true;
}

void
draw_world(objType drawobject)
{
  switch (drawobject) {
  case SPHERE: 
    /* TASK 3: YOUR CODE HERE
     * Bind the vertex array object for sphere.
     *
     * TASK 4: YOUR CODE HERE
     * Bind the texture for sphere.
     *
     * TASK 7: YOUR CODE HERE
     * Bind the normal map for sphere.
     */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tods[SPHERE]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, nmods[SPHERE]);
    glBindVertexArray(vaods[SPHERE]);
    draw_sphere();
    break;

  case CUBE: 
    /* TASK 3: YOUR CODE HERE
     * Bind the vertex array object for cube.
     *
     * TASK 4: YOUR CODE HERE
     * Bind the texture for cube.
     *
     * TASK 7: YOUR CODE HERE
     * Bind the normal map for cube.
     */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tods[CUBE]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, nmods[CUBE]);
    glBindVertexArray(vaods[CUBE]);
    draw_cube();
    break;

  default: // whole scene of multiple objects
    /* TASK 7: YOUR CODE HERE
     *
     * Display at least three objects, using BOTH sphere and cube.
     *
     * Texture each object with a different texture.
     * You'll need to acces the global variables
     * "tods" and "ntods".
     * Make a scene. Be creative!
     *
     * Don't forget to activate the correct texture unit
     * and bind the right normal map also.
     */

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
      glRotatef(180.0, 0.0, 0.0, 1.0);
      glTranslatef(0.0, -0.4, -3.0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, tods[0]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, nmods[0]);
      glBindVertexArray(vaods[CUBE]);
      draw_cube();
    glPopMatrix();

    glPushMatrix();
      glTranslatef(-1.0, -1.5, -2.0);
      glScalef(0.3, 0.3, 0.3);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, tods[2]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, nmods[2]);
      glBindVertexArray(vaods[SPHERE]);
      draw_sphere();
    glPopMatrix();

    glPushMatrix();
      glTranslatef(1.0, -1.5, -2.0);
      glScalef(0.3, 0.3, 0.3);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, tods[2]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, nmods[2]);
      glBindVertexArray(vaods[SPHERE]);
      draw_sphere();
    glPopMatrix();

    glPushMatrix();
      glTranslatef(0.0, -1.0, -2.0);
      glScalef(0.2, 0.2, 0.2);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, tods[2]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, nmods[2]);
      glBindVertexArray(vaods[SPHERE]);
      draw_sphere();
    glPopMatrix();

    glPushMatrix();
      glTranslatef(0.0, 0.0, -5.0);
      glScalef(10.0, 7.0, 1.0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, tods[1]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, nmods[1]);
      glBindVertexArray(vaods[CUBE]);
      draw_sphere();
    glPopMatrix();

    break;
  }
  
  return;
}
