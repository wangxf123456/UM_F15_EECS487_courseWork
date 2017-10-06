/*
 * Copyright (c) 2010, 2011 University of Michigan, Ann Arbor.
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
 * Authors: Ari Grant, Sugih Jamin
 *
 */
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
using namespace std;

#ifdef __APPLE__
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <GLUT/glut.h>
#define GL_RGB16F GL_RGB16F_ARB
#define GL_RGB32F GL_RGB32F_ARB
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "xvec.h"
#include "xmat.h"

int width = 600;
int height = 600;

float viewoffset = -3.0f;

/* Tesselation of the walls.  Higher tesselation results in better
 * lighting because lighting is per-vertex and not per-fragment */
int wallTessellation = 16;

/* Walls in the scene. The 'front' wall behind the viewer is not used. */
typedef enum { FLOOR=0, LEFT_WALL, RIGHT_WALL, CEILING, REAR_WALL, FRONT_WALL } wall_t;
#define NUM_WALLS 5  // FRONT_WALL not used

/* whether shadows are soft */
bool softShadows = false;

/* clip shadows to reciever */
bool stencilClipping = false;

/* The number of times and the amount by which the light is moved
 * along each axis.  This produces the soft shadows */
int numJitters = 2;
float jitterAmount = 0.009f;

/* whether to draw the whole scene */
int clippedTo = FRONT_WALL;

/* global rotation */
GLfloat rotation = 0.0f;

/* the original and current/jittered location of the light */
GLfloat lightPos0[] = { 0.05f, 0.6f, 0.2f, 1.0f };
GLfloat lightPos[] = { 0.05f, 0.6f, 0.2f, 1.0f };

/* Plane equation for each of the five walls in the scene. Each is
 * four numbers (A,B,C,D), representing the plane Ax+By+Cz+D=0. */
GLfloat walls[NUM_WALLS][4] = {
  {0.0f, 1.0f, 0.0f, 1.0f},  /* floor */
  {1.0f, 0.0f, 0.0f, 1.0f},  /* left wall */
  {-1.0f, 0.0f, 0.0f, 1.0f}, /* right wall */
  {0.0f, -1.0f, 0.0f, 1.0f}, /* ceiling */
  {0.0f, 0.0f, 1.0f, 1.0f}   /* rear wall */
};

inline bool
pointInPlane(GLfloat *point, GLfloat *plane)
{
  XVec3f n(plane[0], plane[1], plane[2]);
  XVec3f p(point[0], point[1], point[2]);

  return (n.dot(p)+plane[3] < .05);
}

/* draw a wall */
void
drawWall(int wall)
{
  /* save the OpenGL state since this function
     draws a generic wall and simply rotates it
     first, depending which wall it is */
  glPushMatrix();
        
  /* for each wall, set the color, and if needed,
     rotate the wall into position */
  switch (wall) {
  case FLOOR:
    glColor3f(0.4f, 0.8f, 0.7f);
    break;
  case RIGHT_WALL:
    glColor3f(0.7f, 0.8f, 0.4f);
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    break;
  case CEILING:
    glColor3f(0.4f, 0.7f, 0.8f);
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
    break;
  case LEFT_WALL:
    glColor3f(0.8f, 0.7f, 0.4f);
    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    break;
  case REAR_WALL:
    glColor3f(0.8f, 0.4f, 0.7f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    break;
  default:
    break;
  }
        
  /* draw a grid of quads in the xz-plane.
     the number of quads drawn is the square
     of the tessellation */
  glBegin(GL_QUADS);
  glNormal3f(0.0f, 1.0f, 0.0f);
  for (int i = 0; i < wallTessellation; ++i) {
    for (int j = 0; j < wallTessellation; ++j) {
      float x0 = (float)i/wallTessellation*2.0f - 1.0f;
      float z0 = (float)j/wallTessellation*2.0f - 1.0f;
      
      float x1 = (float)(i+1)/wallTessellation*2.0f - 1.0f;
      float z1 = (float)(j+1)/wallTessellation*2.0f - 1.0f;
      
      glVertex3f(x0, -1.0f, z0);
      glVertex3f(x0, -1.0f, z1);
      glVertex3f(x1, -1.0f, z1);
      glVertex3f(x1, -1.0f, z0);
    }
  }
  glEnd();
        
  /* return the state, since a rotation may
     have been used to position the wall */
  glPopMatrix();

  return;
}

void
positionAndDrawLight()
{
  /* position the light for hard/soft shadows */
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        
  /* save the state, move to the light position,
     draw a near-white sphere, and restore the state.
     the light is always drawn at the same place so
     that it does not turn out blurry in the final
     render. */
  glPushMatrix();
  glTranslatef(lightPos0[0], lightPos0[1], lightPos0[2]);
        
  glDisable(GL_LIGHTING);
  glColor3f(1.0f, 1.0f, 0.8f);
  glutSolidSphere(0.05f, 20, 20);
  glEnable(GL_LIGHTING);
  glPopMatrix();

  return;
}

/* project onto 'receiverWall' with 'lightPos'
   being the point of convergence of the projection. think
   of it as a perspective projection onto 'receiverWall' */
void
multShadowMat(GLfloat *receiverWall)
{
  static XMat4f shadowMat(0.0);
        
  /* TASK 1: YOUR CODE HERE: Hard Shadows
   * Set the 16 components of the shadow matrix using
   * receiverWall and the global lightPos
  */

  float dot = receiverWall[0] * lightPos[0] + receiverWall[1] * lightPos[1]
                  + receiverWall[2] * lightPos[2] + receiverWall[3] * lightPos[3];
  shadowMat(0, 0) = dot - receiverWall[0] * lightPos[0];
  shadowMat(1, 0) = - receiverWall[0] * lightPos[1];
  shadowMat(2, 0) = - receiverWall[0] * lightPos[2];
  shadowMat(3, 0) = - receiverWall[0] * lightPos[3];
  shadowMat(0, 1) = - receiverWall[1] * lightPos[0];
  shadowMat(1, 1) = dot - receiverWall[1] * lightPos[1];
  shadowMat(2, 1) = - receiverWall[1] * lightPos[2];
  shadowMat(3, 1) = - receiverWall[1] * lightPos[3];
  shadowMat(0, 2) = - receiverWall[2] * lightPos[0];
  shadowMat(1, 2) = - receiverWall[2] * lightPos[1];
  shadowMat(2, 2) = dot - receiverWall[2] * lightPos[2];
  shadowMat(3, 2) = - receiverWall[2] * lightPos[3];
  shadowMat(0, 3) = - receiverWall[3] * lightPos[0];
  shadowMat(1, 3) = - receiverWall[3] * lightPos[1];
  shadowMat(2, 3) = - receiverWall[3] * lightPos[2];
  shadowMat(3, 3) = dot - receiverWall[3] * lightPos[3];

  /* add the transformation to the ModelView matrix */
  glMultMatrixf(shadowMat);

  return;
}

void
drawOccluders(bool shadow)
{
  /* save the state and put on some funcky rotations */
  glPushMatrix();
  glRotatef(rotation, 0.7f, 1.0f, -0.2f);
  glRotatef(0.434658f*rotation, 0.8f, 0.1f, 0.65f);
        
  // place some objects in the scene:

  // a torus in the corner
  if (!shadow) {
    glColor3f(1.0f, 0.5f, 0.3f);
  }
  // this makes the torus move forward and back
  glPushMatrix();
  glTranslatef(-0.3f*sinf(rotation/80.0f), 0.3f*sinf(rotation/100.0f), 0.0f);
  glRotatef(80.0f, 1.0f, 1.0f, 0.0f);
  glutSolidTorus(0.05f, 0.35f, 16, 24);
  glPopMatrix();
        
  // a teapot floating
  if (!shadow) {
    glColor3f(0.5f, 0.3f, 1.0f);
  }
  glPushMatrix();
  glTranslatef(-0.3f, 0.16f, 0.1f);
  glRotatef(60.0f*sinf(rotation/60.0f), 0.2f, -0.6f, -0.6f);
  glutSolidTeapot(0.16f);
  glPopMatrix();
        
  /* pop back from the rotation for the objects */
  glPopMatrix();
        
  return;
}

void
drawShadows(GLfloat *receiverWall)
{
  /* set the draw color to darker version of wall color, enable polygon
     offset to avoid z-fighting, turn off lighting,
     save the state, and set up the shadow projection */
  GLfloat color[4];
  glGetFloatv(GL_CURRENT_COLOR, color);
  glColor3f(0.3f*color[0], 0.3f*color[1], 0.3f*color[2]);
  glEnable(GL_POLYGON_OFFSET_FILL);
  
  glDisable(GL_LIGHTING);
  glPushMatrix();
  multShadowMat(receiverWall);
        
  drawOccluders(true);
  
  /* return the state to what it was at the start */
  glDisable(GL_POLYGON_OFFSET_FILL);
  glEnable(GL_LIGHTING);      
  glPopMatrix();

  return;
}

void
drawScene()
{
  int i;
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  /* position light and draw a sphere there */
  positionAndDrawLight();
        
  // draw the walls, to set depth boundaries
  for (i = FLOOR; i <= REAR_WALL; i++) {
    if (clippedTo == FRONT_WALL || i == clippedTo) {
      drawWall(i);
    }
  }

  if (stencilClipping) {
    // enable stencil test for fragment selection
    glEnable(GL_STENCIL_TEST);

    // clear the stencil buffer.
    glClear(GL_STENCIL_BUFFER_BIT);
  }

  for (i = FLOOR; i <= REAR_WALL; i++) {
    if (clippedTo == FRONT_WALL || i == clippedTo) {

      /* TASK 2: YOUR CODE HERE: Clipped Shadows
       *
       * Set the stencil buffer operation to always accept
       * a fragment and set the reference value to 1
       */
      glStencilFunc(GL_ALWAYS, 1, 1);

      /* TASK 2: YOUR CODE HERE: Clipped Shadows
       *
       * Stencil buffered has been cleared to 0.
       * If a fragment fails the stencil test, keep the value in
       * the stencil buffer (0).  If a fragment passes the stencil test
       * but fails the depth test, keep the stencil value (0). If
       * stencil and depth tests pass, replace the stencil buffer
       * value with ref value specified previously (1).
       */
  
      glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);

      // draw the wall into stencil buffer only, not depth and color buffers
      glDisable(GL_DEPTH_TEST);
      glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
      drawWall(i);
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
      glEnable(GL_DEPTH_TEST);

      /* TASK 2: YOUR CODE HERE: Clipped Shadows
       *
       * Set the stencil accept/reject function to only accept
       * fragments that are drawn 'in' the current wall. The stencil
       * values there are 1 and 0 everywhere else. Use 1 as the
       * stencil mask
       */
      glStencilFunc(GL_EQUAL, 1, 1);

      // shadows: draw the occluders in black,
      // projected onto the specified plane/wall
      drawShadows(walls[i]);
    }
  }
    
  // no longer use the stencil buffer to reject fragments
  if (stencilClipping) {
    glDisable(GL_STENCIL_TEST);
  }
  
  // draw the occluders normally
  drawOccluders(false);

  return;
}

int
fbo_accum_init()
{
  GLuint fbod;
  GLuint tbod;
  GLuint rbod[2];
  GLint maxsize;
  GLenum gl_error;
  
  glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxsize);
  if (width > maxsize || height > maxsize) {
    cerr << "Window size > max renderbuffer size" << endl;
    exit(-1);
  }
    
  /* TASK 4: YOUR CODE HERE
   *
   * Generate a framebuffer object with a texture object
   * with GL_RGBA internalformat as GL_COLOR_ATTACHMENT0.
   * Set up your texture completely: specify what to do
   * when the texture needs to be minified and magnified, 
   * how to apply the texture (replace, modulate, blend?).
   * Next generate a renderbuffer object with GL_RGB32F 
   * internalformat as GL_COLOR_ATTACHMENT1, and a 
   * renderbuffer object as GL_DEPTH_STENCIL_ATTACHMENT.
   * Be sure to check that your framebuffer status is 
   * GL_FRAMEBUFFER_COMPLETE.
   */
  glGenFramebuffers(1, &fbod);
  glBindFramebuffer(GL_FRAMEBUFFER, fbod);

  glGenTextures(1, &tbod);
  glBindTexture(GL_TEXTURE_2D, tbod);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbod, 0);

  glGenRenderbuffers(2, rbod);
  glBindRenderbuffer(GL_RENDERBUFFER, rbod[0]);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB32F, width, height);
  if ((gl_error = glGetError()) != GL_NO_ERROR) {
    cerr << "RenderbufferStorage: [" 
         << gl_error << "] " << glGetString(gl_error) << endl;
    exit(-1);
  }
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                            GL_RENDERBUFFER, rbod[0]);

  glBindRenderbuffer(GL_RENDERBUFFER, rbod[1]);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbod[1]);

  if ((gl_error = glCheckFramebufferStatus(GL_FRAMEBUFFER)) !=
      GL_FRAMEBUFFER_COMPLETE) {
    cerr << "Framebuffer incomplete [0x" << hex << gl_error << "] "
         << glGetString(gl_error) << endl;
    exit(-1);
  }

  // clear accumulation buffer
  glDrawBuffer(GL_COLOR_ATTACHMENT1);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  // return drawing to COLOR_ATTACHMENT0
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glClearColor(0.7f, 0.8f, 1.0f, 1.0f);

  return(fbod);
}
  
// assume COLOR_ATTACHMENT0 is a texture object and is the draw buffer
// and COLOR_ATTACHMENT1 is the accumulation buffer
void
fbo_accum(float weight)
{
  float quad[4][2] = {
    { -1.0, 1.0 },
    { -1.0, -1.0 },
    { 1.0, 1.0 },
    { 1.0, -1.0 }
  };
  float normals[4][3] = {
    { 0.0, 0.0, 1.0 },
    { 0.0, 0.0, 1.0 },
    { 0.0, 0.0, 1.0 },
    { 0.0, 0.0, 1.0 }
  };
  float texcoords[4][2] = {
    { 0.0, 1.0 },
    { 0.0, 0.0 },
    { 1.0, 1.0 },
    { 1.0, 0.0 }
  };
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, quad);
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 0, normals);
  glEnable(GL_NORMALIZE);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);  // diff per app?

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 10.0); // diff per app?

  // draw the quad/image without depth testing nor additional lighting
  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  /* TASK 4: YOUR CODE HERE
   *
   * Setup glBlend*() to blend each frame
   * onto the accumulation buffer, using
   * argument "weight".
   */
  glEnable(GL_BLEND);
  glBlendColor(0.0, 0.0, 0.0, weight);
  glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE);
  glBlendEquation(GL_FUNC_ADD);

  /* TASK 4: YOUR CODE HERE
   *
   * Draw the quad above, textured with the image
   * in GL_COLOR_ATTACHMENT0, onto the
   * accumulation buffer
   */
  glDrawBuffer(GL_COLOR_ATTACHMENT1);
  glEnable(GL_TEXTURE_2D);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // restore states
  glDrawBuffer(GL_COLOR_ATTACHMENT0);   
  glPopAttrib();
  glPopMatrix(); // GL_PROJECTION
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  return;
}

void
fbo_accum_return(GLuint fbod)
{
  /* TASK 4: YOUR CODE HERE
   *
   * Bind fbod to GL_READ_FRAMEBUFFER.
   * Set color attachment 1 as the read buffer.
   * Bind the default fbo (0) to GL_DRAW_FRAMEBUFFER.
   * Call glBlitFramebuffer().
   * Bind the default fbo to GL_FRAMEBUFFER and display.
   */
  GLenum gl_error; 
  glDisable(GL_BLEND);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbod);
  glReadBuffer(GL_COLOR_ATTACHMENT1);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
  if ((gl_error = glGetError()) != GL_NO_ERROR) {
    cerr << "BlitFramebuffer: [" 
         << gl_error << "] " << glGetString(gl_error) << endl;
    exit(-1);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return;
}
  
void
display()
{
  /* prepare the modelview with an offset backward */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, viewoffset);

  lightPos[0] = lightPos0[0];
  lightPos[1] = lightPos0[1];
  lightPos[2] = lightPos0[2];
  
  /* if soft shadows are desired, jitter the light position,
     each time accumulating the contents of the framebuffer,
     then copy the accumulation buffer into the framebuffer */
  if (softShadows) {
    /* TASK 3: YOUR CODE HERE: Soft Shadows
     *
     * Jitter the light position using the global variables numJitters
     * and jitterAmount. How, is up to you.  For every jittered light
     * position, call drawScene() to draw the scene (replacing the
     * call to drawScene() below).  After each drawScene(), accumulate
     * the contents of the framebuffer into the accumulation buffer,
     * properly weighted.  Finally, write the accumulated contents
     * back into the framebuffer.
     *
     * TASK 4: YOUR CODE HERE: Frambuffer Object
     *
     * Replace your use of OpenGL's accumulation buffer with calls to
     * fbo_accum_init(), fbo_accum(), and fbo_accum_return() above
     * and fill out these three functions.  (The three functions were
     * designed to be drop-in replacements for the OpenGL accumulation
     * buffer calls, so you should be able to replace them with a
     * one-to-one mapping.)
    */
    GLuint fbod = fbo_accum_init(); 
    for (int i = 0; i < numJitters; i++) {
      for (int j = 0; j < 3; j++) {
        lightPos[j] += jitterAmount;
        drawScene();
        fbo_accum(1.0 / numJitters / 3.0);         
      }
    }

    fbo_accum_return(fbod);
  } else {
    /* for hard shadows, just draw the scene */
    /* clear the buffers */
    drawScene();
  }

  /* with really soft shadows, the application may
     slow greatly, so wait for the pending operations
     to finish */
  glFinish();

  /* and then swap the framebuffer into view */
  glutSwapBuffers();

  return;
}

void
kbd(unsigned char key, int x, int y)
{
  switch(key) {
    /* quit on 'escape' */
  case 'q':
  case '\033':
    exit(0);
    break;
    
    /* move the light left */
  case 'h':
    lightPos0[0] -= 0.02f;
    if (pointInPlane(lightPos0, walls[LEFT_WALL])) {
      lightPos0[0] += 0.02f;
    }
    break;
    
    /* move the light right */
  case 'l':
    lightPos0[0] += 0.02f;
    if (pointInPlane(lightPos0, walls[RIGHT_WALL])) {
      lightPos0[0] -= 0.02f;
    }
    break;
    
    /* move the light down */
  case 'j':
    lightPos0[1] -= 0.02f;
    if (pointInPlane(lightPos0, walls[FLOOR])) {
      lightPos0[1] += 0.02f;
    }
    break;
    
    /* move the light up */
  case 'k':
    lightPos0[1] += 0.02f;
    if (pointInPlane(lightPos0, walls[CEILING])) {
      lightPos0[1] -= 0.02f;
    }
    break;
    
    /* move the light backward */
  case 'w':
    lightPos0[2] -= 0.02f;
    if (pointInPlane(lightPos0, walls[REAR_WALL])) {
      lightPos0[2] += 0.02f;
    }
    break;
    
    /* move the light forward */
  case 's':
    lightPos0[2] += 0.02f;
    break;
    
    /* increase the number of light jitters */
  case '+':
    numJitters++;
    cerr << "numJitters: " << numJitters << endl;
    break;
    
    /* decrease the number of light jitters */
  case '-':
    if (numJitters > 1) {
      numJitters--;
      cerr << "numJitters: " << numJitters << endl;
    }
    break;
    
    /* increase the amount of light jitter */
  case 'a':
    jitterAmount += 0.001f;
    cerr << "jitterAmount: " << jitterAmount << endl;
    break;
    
    /* decrease the amount of light jitter */
  case 'd':
    if (jitterAmount > 0.001f) {
      jitterAmount -= 0.001f;
      cerr << "jitterAmount: " << jitterAmount << endl;
    }
    break;
    
    /* toggles hard/soft shadows */
  case 'f':
    softShadows = !softShadows;
    cerr << (softShadows ? "Soft" : "Hard" ) << " Shadows" << endl;
    break;
    
    /* toggle the visibility of the walls/ceiling */
  case 'x':
    clippedTo++;
    clippedTo %= (NUM_WALLS+1);
    break;
    
  case 'X':
    clippedTo--;
          if (clippedTo < 0) {
      clippedTo = FRONT_WALL;
    }
    break;
    
    /* toggles stencil-based clipping */
  case 'c':
    stencilClipping = !stencilClipping;
    break;
    
    /* rotate the world */
  case 'r':
    rotation += 5.0f;
    break;
    
    /* increase the wall tessellation */
  case 'T':
    wallTessellation += 8;
    cerr << "wallTessellation: " << wallTessellation << endl;
    break;
    
    /* decrease the wall tessellation */
  case 't':
    if (wallTessellation > 1) {
      wallTessellation -= 8;
      cerr << "wallTessellation: " << wallTessellation << endl;
    }
    break;
    
    /* move view offset backward, world forward */
  case 'W':
    if (viewoffset > -6.0f) {
      viewoffset -= 0.1f;
    }
    break;

    /* move view offset forward, world backward */
  case 'S':
    if (viewoffset < -1.0f) {
      viewoffset += 0.1f;
    }
    break;
    
  default:
    break;
  }
  
  /* ask GLUT to draw again */
  glutPostRedisplay();

  return;
}

void
initGL()
{
  /* clear with a pale blue */
  glClearColor(0.7f, 0.8f, 1.0f, 1.0f);
        
  /* enable depth testing */
  glEnable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LEQUAL);
        
  /* setup one light */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.5f);
        
  /* set a polygon offset. its use is disabled
     by default, but it will be enabled later. it
     puts an offset into the depth buffer, used
     here to avoid z-fighting when drawing shadows */
  glPolygonOffset(-1.0f, -5.0f);
        
  /* let OpenGL calculate material properties
     from calls to glColor instead of glMaterial */
  glEnable(GL_COLOR_MATERIAL);
        
  /* load a simple projection (it will not
     be changed anywhere else) */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 2.0, 7.0);

  return;
}

int
main (int argc, char *argv[])
{
  /* init GLUT */
  glutInit(&argc, argv);
        
  /* prepare the window size */
  glutInitWindowSize(width, height);
        
  /* create a window with the necessary buffers */
#if defined(__APPLE__)
  //glutInitDisplayString("acca>8 rgba=8 depth>=12 double stencil>=8");
  glutInitDisplayString("acca>8 rgba>=8 depth>=12 double stencil>=8");
#else
  glutInitDisplayMode(GLUT_RGBA | GLUT_ACCUM | GLUT_STENCIL | GLUT_DEPTH | GLUT_SINGLE );
#endif

  /* cast to void to avoid the warning that the
     returned value (the window handle) is not used */
  (void)glutCreateWindow("Lab7: Projected (Soft) Shadows with Framebuffer Object");
        
#ifndef __APPLE__
  GLenum err;
  if ((err = glewInit()) != GLEW_OK) {
    /* Problem: glewInit failed, something is seriously wrong. */
    cerr << "Error: " << glewGetErrorString(err) << endl;
  }
  cout <<  "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
#endif /* __APPLE__ */

  /* set the GLUT callbacks. there is
     no reshape callback since the
     default one provided by GLUT does
     all that is needed here (it simply
     updates the viewport) */
  glutDisplayFunc(display);
  glutKeyboardFunc(kbd);
        
  /* prepare OpenGL state */
  initGL();
        
  /* enter GLUT's event loop */
  glutMainLoop();
        
  return 0;
}

