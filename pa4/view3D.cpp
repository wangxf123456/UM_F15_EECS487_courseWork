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
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else 
#ifdef _MSC_VER
#include <GL/glew.h>
#endif
#include <GL/glut.h>
#endif

#include "parseX3D.h"
#include "scene.h"

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

  return(buf_name.c_str());
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

  return(buf_name.c_str());
}
#endif

// Global constants (image dimensions).
const int WIDTH = 640;
const int HEIGHT = 480;

X3Scene* scene = NULL;

bool rmb_down = false;
bool lmb_down = false;
float zoff_old = 0.0f;
float phi_old = 0.0f;
float theta_old = 0.0f;
XVec2i down_rmb, down_lmb;

// timing parameters: in milliseconds
bool stop_animation = true;
int start_time = 0; 
int current_time = 0;
int one_step = 50; // 20 fps

// This function is called when window needs to be rendered.
void
display()
{
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (scene) {
    // setup the viewpoint cam

    // Setup lights.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    int count_lights = 0;

    scene->SetupLights(&count_lights);

    // cout << count_lights << " lights in the scene." << endl;

    // If there are no lights in the scene file, setup a default light.
    if (count_lights==0) {
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      XVec4f light_pos(1, 1, 0, 1);
      glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
      glEnable(GL_LIGHT0);
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

    //scene->set_time(0.001f*glutGet(GLUT_ELAPSED_TIME));
    if (!stop_animation) {
      current_time = glutGet(GLUT_ELAPSED_TIME) - start_time;
    } 
    scene->set_time(0.001f*current_time);

    scene->Render();
  }
    
  glutSwapBuffers();

  return;
}

void
idle()
{
  glutPostRedisplay();

  return;
}

// This function is called when user presses a key.
void
kbd(unsigned char key, int x, int y)
{
  switch(key) {
  case 32:
    stop_animation = !stop_animation;
    if (stop_animation) {
      // stopped animation -- remember current_time
      current_time = glutGet(GLUT_ELAPSED_TIME) - start_time;
      glutIdleFunc(NULL);
    } else {
      // restarting...
      start_time = glutGet(GLUT_ELAPSED_TIME) - current_time;
      glutIdleFunc(idle);
    }
    break;
  case 'q':
  case 27: // ESC key
    exit(0);
    break;
  }
  glutPostRedisplay();

  return;
}

// This function is called when user presses a special key.
void
special(int key, int x, int y)
{
  switch(key) {
  case GLUT_KEY_HOME:
    if (stop_animation) {
      current_time = 0;
    }
    break;
  case GLUT_KEY_RIGHT:
    if (stop_animation) {
      current_time += one_step;
    }
    break;
  case GLUT_KEY_LEFT:
    if (stop_animation) {
      current_time -= one_step;
    }
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
    if (state==GLUT_DOWN) {
      lmb_down = true;
      phi_old = scene->viewpoint()->phi();
      theta_old = scene->viewpoint()->theta();
      down_lmb = XVec2i(x, y);
    } else if (state==GLUT_UP) {
      lmb_down = false;
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if (state==GLUT_DOWN) {
      rmb_down = true;
      zoff_old = scene->viewpoint()->zoff();
      down_rmb = XVec2i(x, y);
    } else if (state==GLUT_UP) {
      rmb_down = false;
    }
    break;
  }

  return;
}

void
motion(int x, int y)
{
  if (rmb_down) {
    scene->viewpoint()->set_zoff(zoff_old + 0.1f * (y - down_rmb(1)));
  }
  if (lmb_down) {
    scene->viewpoint()->set_phi(phi_old + 0.1f * (x - down_lmb(0)));
    scene->viewpoint()->set_theta(theta_old + 0.1f * (y - down_lmb(1)));
  }
  glutPostRedisplay();

  return;
}

int
main(int argc, char** argv)
{
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

#ifdef _MSC_VER
  GLenum err = glewInit();
  if (GLEW_OK != err) {
      /* Problem: glewInit failed, something is seriously wrong. */
      cerr << "Error: " << glewGetErrorString(err) << endl;
    }
  cout <<  "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
#endif /* __APPLE__ */

  ifstream input_stream(argv[1]);
  X3Reader x3reader;
  x3reader.set_dirname(dirname(argv[1]));

  // This is called after glutInit so we can setup OpenGL while parsing 
  // the file.
  scene = x3reader.Read(input_stream);

  glutDisplayFunc(display);
  glutKeyboardFunc(kbd);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutSpecialFunc(special);
  if (!stop_animation) {
    glutIdleFunc(idle);
  }

  //cerr << endl << "Here is your scene: " << endl;
  //scene->Print(cerr, 0);

  glutMainLoop();

  exit(0);
}
