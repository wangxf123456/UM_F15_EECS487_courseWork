/*
 * Copyright (c) 2007, 2011 University of Michigan, Ann Arbor.
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
 * Authors: Igor Guskov, Sugih Jamin
 *
*/
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

#ifndef _WIN32
#include <libgen.h>
#else
string buf_name;

// define the dirname function for win32.
const char*
dirname(const char* path)
{
  string path_copy(path);
  string::size_type last_slash = path_copy.find_last_of("/\\");
  if (last_slash==string::npos) {
    buf_name.assign(".");
  } else {
    buf_name = path_copy.substr(0, last_slash);
  }
  cerr << buf_name << endl;
  return buf_name.c_str();
}

const char*
basename(const char* path)
{
  string path_copy(path);
  string::size_type last_slash = path_copy.find_last_of("/\\");
  if (last_slash == string::npos) {
    buf_name = path_copy;
  } else {
    buf_name = path_copy.substr(last_slash+1);
  }
  cerr << buf_name << endl;
  return buf_name.c_str();
}
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else 
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "parseX3D.h"
#include "scene.h"
#include "shaders.h"

// Global constants (image dimensions).
const int WIDTH = 640;
const int HEIGHT = 480;
int screen_w = WIDTH;
int screen_h = HEIGHT;
int mouse_x, mouse_y;
bool lb_down = false;
bool rb_down = false;

X3Scene* scene = NULL;
//const float DEG_PER_PIXEL = 0.1f;

typedef struct {
  char *vs;
  char *fs;
  GLuint pd;
} shader_t;
enum ShaderModeEnum {
  NONE,
  GOURAUD,
  PHONG,
  BLINN,
  INTERESTING,
  NSHADERS
} shader_mode = NONE;
shader_t shaders[] = {
  { NULL, NULL, 0 },
  { (char *)"gouraud", NULL, 0 },
  { (char *)"phong", (char *)"phong", 0 },
  { (char *)"phong", (char *)"blinn", 0 },
  { (char *)"interesting", (char *)"interesting", 0 },
};
bool shaders_supported = true;
GLuint spd; // shader program object handle
int count_lights = 0;

void
display()
{
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(shaders[shader_mode].pd);

  if (scene) {
    // setup the viewpoint cam
    
    if (shaders_supported) {  
      // pass count_lights to shaders as a uniform variable
      int uid;

      if (shader_mode != NONE) {
        uid = glGetUniformLocation(shaders[shader_mode].pd, "numlights");
        glUniform1i(uid, count_lights);
      }
    }
    
    // Render scene.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glColor3f(1.0f, 0.0f, 0.0f);

    scene->Render();
  }

  glUseProgram(0);

  glutSwapBuffers();
  
  return;
}

void
kbd(unsigned char key, int x, int y)
{
  switch(key) {
  case 'q':
  case 27: // ESC key
    exit(0);
    break;
  case '@':
    if (scene) {
      scene->viewpoint()->reset();
    }
    break;
  case 'i':
    //if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader) {
    cout << "Loading interesting shaders." << endl;
    shader_mode = INTERESTING;
    //} else {
    //  cerr << "shaders not supported." << endl;
    //}
    break;
  case 'g':
    cout << "Loading per-vertex Gouraud shaders." << endl;
    shader_mode = GOURAUD;
    break;
  case 'p':
    cout << "Loading per-pixel Phong shaders." << endl;
    shader_mode = PHONG;
    break;
  case 'b':
    cout << "Loading per-pixel Blinn-Phong shaders." << endl;
    shader_mode = BLINN;
    break;
  case 'n':
    shader_mode = NONE;
    cout << "Fixed-function pipeline mode (no programmable shaders)." << endl;
    break;
  }
  glutPostRedisplay();

  return;
}

void
reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (scene) {
    float aspect = float(w) / h;
    gluPerspective(45.0f, aspect, 0.1f, 1000.0f);
  }
  screen_w = w;
  screen_h = h;

  return;
}

void
mouse(int button, int state, int x, int y)
{
  if (!scene) {
    return;
  }

  switch(button) {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN) {
      lb_down = true;
      mouse_x = x;
      mouse_y = y;
    } else if (state == GLUT_UP){
      lb_down = false;
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if (state == GLUT_DOWN) {
      rb_down = true;
      mouse_x = x;
      mouse_y = y;
    } else {
      rb_down = false;
    }
    break;
  default:
    break;
  }

  return;
}

void
motion(int x, int y)
{
  if (lb_down) {
    scene->viewpoint()->track_latlong(((float) (x - mouse_x))/screen_w,
                                      ((float) (y - mouse_y))/screen_h);
  }

  if (rb_down) {
    scene->viewpoint()->dolly(((float) (y - mouse_y))/screen_h);
  }
    
  mouse_x = x;
  mouse_y = y;

  glutPostRedisplay();
 
  return;
}

int
main(int argc, char** argv)
{
  int i;
  struct stat filestat;

  if (argc<2) {
    cerr << "Usage: " << basename(argv[0]) << " <input>.x3d" << endl;
    return -1;
  }

  if (stat(argv[1], &filestat)) {
    cerr << basename(argv[0]) << ": " << argv[1] << ": No such file or directory" << endl;
    return(-1);
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);
  glutInitWindowSize(WIDTH, HEIGHT); 
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[1]);

#ifndef __APPLE__
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      cerr << "Error: " << glewGetErrorString(err) << endl;
    }
  cout <<  "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
#endif /* __APPLE__ */

  for (i = GOURAUD; i < NSHADERS; i++) {
    shaders_supported = InitShaders(shaders[i].vs,
                                    shaders[i].fs,
                                    &shaders[i].pd);
    if (!shaders_supported) {
      cerr << "Shaders init failed. Exiting." << endl;
      exit(-1);
    }
  }

  ifstream input_stream(argv[1]);
  X3Reader x3reader;
  scene = x3reader.Read(input_stream);

  // Setup lights.
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  scene->SetupLights(&count_lights);
  glPopMatrix();
  // If there are no lights in the scene file, setup a default light.
  if (count_lights==0) {
    XVec4f light_pos(1, 1, 0, 1);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glEnable(GL_LIGHT0);
          count_lights = 1;
  }

  glutDisplayFunc(display);
  glutKeyboardFunc(kbd);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  glutMainLoop();

  exit(0);
}
