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

int wd;
GLdouble screen_width=800, screen_height=600;

GLfloat position[] = { 0.0, 0.0, 2.0, 0.0 };
//GLfloat diffuse[] = { 0.8, 0.5, 0.6, 1.0 };
GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat specular[] = {0.0, 0.8, 0.0, 1.0 };
//GLfloat ambient[] = {0.4, 0.4, 0.2, 1.0};
GLfloat ambient[] = {0.5, 0.5, 0.5, 1.0};
GLfloat mat_specular[] = { .7, 1.0, 4.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };
GLfloat mat_ambient[] = { 0.2, 0.2, 0.3, 1.0 };

void
init_lights(void)
{
  /* Lighting and material properties */
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);

  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

  return;
}

void
load_texture(LTGA *texture)
{
  /* 
   * TASK 2: YOUR CODE HERE
   *
   * Set up autogeneration of mipmap:
   * - specify GL_TEXTURE_MAX_LEVEL using glTexParameteri()
   * - auto generate mipmap by setting GL_GENERATE_MIPMAP
   *   to GL_TRUE using glParameteri() BEFORE calling
   *   glTexImage2D().
  */
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);

  /* TASK 1: YOUR CODE HERE
   *
   * Specify the texture and set its parameters. 
   * To actually map the texture onto the sphere,
   * go back to the init_sphere() functions and assign a pair of 
   * texture coordinates to each vertex forming the object.
   * You may want to use the GetAlphaDepth() and GetImageType()
   * methods of LTGA (see ltga.h and ltga.cpp ) to determine
   * whether the image is greyscale (GL_LUMINANCE or
   * GL_LUMINANCE_ALPHA) or color (GL_RGB or GL_RGBA).
   *
   * TASK 3: YOUR CODE HERE
   * Modify your call to glTexImage2D() to work with pixel 
   * buffer object
   */
  /* Specify texture */

  /* Set texture parameters */
  // linearly interpolate between 4 nearest texel values while
  // shrinking or stretching

  // clamp texture coordinates (s,t) to [0,1] each
  /* BGIN SOLUTION */
  LImageType temp = texture->GetImageType();
  GLenum type = 0;
  switch(temp) {
    case itGreyscale:
      type = GL_LUMINANCE;
      break;
    case itRGB:
      type = GL_RGB;
      break;
    case itRGBA:
      type = GL_RGBA;
      break;
    default:
      type = GL_RGB;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, type, texture->GetImageWidth(),
                texture->GetImageHeight(), 0, type, GL_UNSIGNED_BYTE, 
                0);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  /* END SOLUTION */

  // Simply copy the texture, don't modulate or blend etc
  // and enable texture
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glEnable(GL_TEXTURE_2D); 
  // perspective correct interpolation
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


  return;
}

void
toggle_mipmapping()
{
  static bool mipmapping=false;

  mipmapping = !mipmapping;

  if (mipmapping) {
    /* TASK 2: YOUR CODE HERE: 
     * Turn on mipmapping using glTexParameteri().
     * Assume mipmap has been generated in load_texture().
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  } else {
    // linearly interpolate between 4 nearest texel values while 
    // shrinking or stretching
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  }
  cerr << "Mipmapping " << (mipmapping ? "ON" : "OFF") << endl;

  return;
}

/*
 * called by LTGA::LoadFromFile()
 * see comments inside function.
 */
void *
pbo_alloc(uint size)
{
  /* TASK 3: YOUR CODE HERE
   * Allocate graphics memory of given size, map it to client
   * address space, and return a pointer to the mapped memory.
   *
   * Assume pixel buffer object has been generated by
   * glGenBuffers() (in init_texture()) and that it has
   * been bound for unpack buffer.
   *
   * Replace the following line with your code
  */
  glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_STREAM_DRAW);
  return glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_READ_WRITE);
}

void
read_texture(LTGA *texture, const char *texture_image_filename)
{
  texture->LoadFromFile(texture_image_filename);
  if (!texture->IsLoaded()) {
    cerr << " Image not loaded" << endl;
    exit(1);
  }
  cout << "Texture: " << endl;
  cout << "Image type   = " << LImageTypeString[texture->GetImageType()] 
       << " (" << texture->GetImageType() << ")" << endl;
  cout << "Image width  = " << texture->GetImageWidth() << endl;
  cout << "Image height = " << texture->GetImageHeight() << endl;
  cout << "Pixel depth  = " << texture->GetPixelDepth() << endl;
  cout << "Alpha depth  = " << texture->GetAlphaDepth() << endl;
  cout << "RL encoding  = " << (texture->GetImageType() > (LImageType) 8) << endl;
  /* use texture->GetPixels()  to obtain the pixel array */

  return;
}

void
init_texture(const char *texture_image_filename)
{
  LTGA texture;

  /* 
   * TASK 3: YOUR CODE HERE
   * Generate a pixel buffer object and
   * bind it to the pixel unpack buffer.
   */
  GLuint pbod;
  glGenBuffers(1, &pbod);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbod);

  read_texture(&texture, texture_image_filename);

  /* TASK 3: YOUR CODE HERE: 
   * Unmap the pixel buffer object,
   * otherwise glTexImage2D() won't have
   * access to the buffer.
  */
  
  glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

  /* TASK 1: YOUR CODE HERE:
   * Generate a texture object and bind
   * it to the 2D texture object
   */
  GLuint tod;
  glGenTextures(1, &tod);
  glBindTexture(GL_TEXTURE_2D, tod);

  load_texture(&texture);

  /* TASK 3: YOUR CODE HERE
   * Now that the texture has been unpacked from the
   * pbo to the texture object, delete the pixel
   * buffer object, which also automatically unbinds
   * the pixel unpack buffer.
   */

  glDeleteBuffers(1, &pbod);
  return;
}

void
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw the sphere from vbo.
  draw_sphere();

  glutSwapBuffers();

  return;
}

void
reshape(int w, int h)
{
  /* Save new screen dimensions */
  screen_width = (GLdouble) w;
  screen_height = (GLdouble) h;

  /* Instruct Open GL to use the whole window for drawing */
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);

  /* Set the perspective - later in this course */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(55, screen_width/screen_height, 0.1, 25);

  return;
}

void
kbd(unsigned char key, int x, int y)
{
  static bool in_texture_mode = true;

  // set the matrix mode to model-view
  glMatrixMode(GL_MODELVIEW);

  switch((char)key) {
  case 'q':
  case 27:
    glutDestroyWindow(wd);
    exit(0);
    break;

  case 'k':
  case 'X':
    glRotatef(5,1,0,0);
    break;

  case 'j':
  case 'x':
    glRotatef(-5,1,0,0);
    break;

  case 'w':
  case 'Y':
    glRotatef(5,0,1,0); 
    break;

  case 's':
  case 'y':
    glRotatef(-5,0,1,0); 
    break;

  case 'h':
  case 'z':
    glRotatef(5,0,0,1);
    break;

  case 'l':
  case 'Z':
    glRotatef(-5,0,0,1);
    break;

  case 'm':
    glScalef(0.5, 0.5, 0.5);
    break;

  case 'M':
    glScalef(1.5, 1.5, 1.5);
    break;

  case 'i':
    toggle_mipmapping();
    break;
    
  case 't':
    in_texture_mode = !in_texture_mode;
    if (in_texture_mode) {
      glEnable(GL_TEXTURE_2D); 
    } else {
      glDisable(GL_TEXTURE_2D);
    }
    break;
  }

  glutPostRedisplay();

  return;
}

void
init_gl()
{
  /* Initial camera location and orientation */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);

  glPolygonMode(GL_FRONT, GL_FILL);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_CULL_FACE);

  return;
}

int
main(int argc, char *argv[])
{
  if (argc != 2) {
    cerr << "Usage: globe <texture_image_filename>.tga" << endl;
    return -1;
  }
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize((int)screen_width, (int)screen_height);
  wd = glutCreateWindow("Lab6: Texturing and Mapped Buffer Objects");
#ifndef __APPLE__
  glewInit();
#endif
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(kbd);

  init_gl();

  init_lights();
  init_texture(argv[1]);
  init_sphere();

  glutMainLoop();
  exit(0);
}
