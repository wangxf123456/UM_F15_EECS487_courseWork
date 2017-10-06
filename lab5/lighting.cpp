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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "xvec.h"

#include "objects.h"
#include "modeling.h"
#include "viewing.h"
#include "transforms.h"
#include <iostream>
 using namespace std;

#define SCREEN_WIDTH 1280 // pixels
#define SCREEN_HEIGHT 800 // pixels

int wd;                  /* GLUT window handler */
int screen_w, screen_h;  /* window size in pixels */
int mouse_x, mouse_y;    /* coordinates of last mouse click within window */

transform_t mode;  /* MAGNIFICATION, ROTATION, MOVECAM */
double cmodview[16];  /* current model view matrix, mirroring the MODELVIEW_MATRIX
                         without the viewing transform so that we can play back all
                         accummulated modeling transforms whenever view changes.
                         Stored in TEXTURE_MATRIX, using it as a scratch pad, basically. */

/* initial camera values */
XVec4f eye_pos0;
XVec4f gaze_dir0; // along -z-axis, g = c - e
XVec4f top_dir0;  // along y-axis
float zNear0;     // z-coordinate of the near plane, negative number
float zFar0;      // z-coordinate of the far plane, negative number
float fovy0;      // fovy

/* TODO - Lighting and material properties
 * SUNLIGHT is directional light from a far away source, like the sun;
 *       it maintains its position and direction relative to the world in eye space.
 * Test: Sunlight is set up to shine from above the world, including the camera.
 *       Roll the camera upside down, objects now look upside down on screen,
 *       but sunlight still shines on top of the objects, even if the top
 *       of objects is now at the bottom of the screen.
 * EYELIGHT is a spotlight like a light mounted on a helmet or eyeglasses;
 *       it maintains its position and orientation relative to the eye in world space.
 * Test: Change the camera location or orientation, eyelight follows
 *       camera, i.e., its light slides over objects as either the camera or
 *       objects move out of its range. Eyelight also attenuates, so as the
 *       camera gets closer to objects, eyelight reflects brighter.
 * OBJLIGHT is like a light attached to a vehicle; it maintains its position
 *       and orientation relative to objects in the world in eye space.
 * Test: Rotate objects around any axis or move them along any axis, objlight
 *       follows the objects and shines on the same spots on the objects.
*/
#define SUNLIGHT GL_LIGHT2
#define EYELIGHT GL_LIGHT1
#define OBJLIGHT GL_LIGHT0  // LIGHT0 defaults to specular (1, 1, 1)

int sunlight_on = 1;  
int eyelight_on = 1;
int objlight_on = 1;

/* sun light */
float sunlight_position[] = {0.0, 1.0, 0.0, 0.0};  // directional light
float sunlight_diffuse[] = {0.8, 0.8, 0.0, 1.0};
float sunlight_ambient[] = {0.4, 0.4, 0.2, 1.0};

/* eye light */
/* whenever you need to change the position of the eyelight, you also
 * must change the spot direction of the light */
float eyelight_position[] = { 0.0, 0.0, 0.0, 1.0 };
float eyelight_spotdir[] = { 0.0, 0.0, -1.0, 0.0 };  
float eyelight_diffuse[] = {0.0, 0.9, 0.0, 1.0};

/* object light */
float objlight_position[] = { 0.0, 0.0, 2.0, 1.0 };
float objlight_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
float objlight_specular[] = {0.0, 0.8, 0.0, 1.0 };

float mat_diffuse[] = { 0.8, 0.5, 0.5, 1.0 };
float mat_ambient[] = { 0.2, 0.2, 0.3, 1.0 };
float mat_specular[] = { 0.7, 1.0, 1.0, 1.0 };
//float mat_specular[] = { .7, 1.0, 4.0, 1.0 };
float mat_shininess[] = { 50.0 };

void
init_properties()
{
  /* Lighting and material properties */
  
  /* sun light */
  glLightfv(SUNLIGHT, GL_DIFFUSE, sunlight_diffuse);
  glLightfv(SUNLIGHT, GL_AMBIENT, sunlight_ambient);
  glEnable(SUNLIGHT);

  /* eye light */
  glLightfv(EYELIGHT, GL_DIFFUSE, eyelight_diffuse);
  glLightf(EYELIGHT, GL_SPOT_CUTOFF, 8.0);
  glLightf(EYELIGHT, GL_QUADRATIC_ATTENUATION, 0.03f);
  glEnable(EYELIGHT);
  
  /* object light */
  glLightfv(OBJLIGHT, GL_DIFFUSE, objlight_diffuse);
  glLightfv(OBJLIGHT, GL_SPECULAR, objlight_specular);
  glEnable(OBJLIGHT);  
  
  /* material properties */
  /* YOUR CODE HERE
   * Your surfaces will not interact with your light until you
   * specify how the material of the surface reflects light.
   * Use glMaterial*() call to set the diffuse and specular
   * responses of the material (use the values defined above,
   * mat_diffuse, mat_ambient, mat_specular, mat_shininess).
   * You are welcome to experiment with different values for the settings.
   */

  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
  /* specularity computed taking into account eye location
   * as opposed to just assuming that view is parallel to z */
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glEnable(GL_LIGHTING);
    
  return;
}

void
init(void)
{
  mode = MOVECAM;
  object = TORUS;
  sphere_radius = INITRAD;

  /* Initial camera location and orientation */
  eye_pos0 = XVec4f(2.0, 3.0, 6.0, 1.0);
  gaze_dir0 = XVec4f(-2.0, -3.0, -6.1, 0.0); // c = (0,0,0); g = c - e
  top_dir0 = XVec4f(0.0, 1.0, 0.0, 0.0);
  zNear0 = -0.1;
  zFar0 = -25.0;
  fovy0 = 27.0;
  cam = Camera(eye_pos0, gaze_dir0, top_dir0, zNear0, zFar0, fovy0);

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glGetDoublev(GL_TEXTURE_MATRIX, cmodview);

  init_properties();

  /* YOUR CODE HERE . . .
   * Initialize your lights: some needs to be positioned
   * before viewing transform, others after.
  */
  glLightfv(SUNLIGHT, GL_POSITION, sunlight_position);
  glLightfv(EYELIGHT, GL_POSITION, eyelight_position);
  glLightfv(OBJLIGHT, GL_POSITION, objlight_position);

  setup_view();           // in viewing.cpp
  /* . . . AND/OR HERE */



  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  /* YOUR CODE HERE - enable various other pieces of OpenGL state
   * 1. Depth test, so that OpenGL has a sense of depth (Cf. glEnable())
   * 2. Automatic normalization of normals (Cf. glEnable())
   * 3. Back face culling (Cf. glEnable())
   * 4. Smooth shading of polygons (Cf. glShadeModel())
   * 5. Filled rendering of polygons (Cf. glPolygonMode())
  */
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable(GL_CULL_FACE);
  glShadeModel(GL_SMOOTH);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glColor4f(0.0, 0.0, 1.0, 0.4);
  initWorld(); // TODO in objects.cpp

  return;
}

/* Draw the world with the latest transformations */
void
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glGetDoublev(GL_TEXTURE_MATRIX, cmodview);
  setup_view();
  drawAxes(screen_w, screen_h, 1.0);  // draw world axes in eye space
  glMultMatrixd(cmodview);
  drawWorld(screen_w, screen_h);

  /* Force display */
  glutSwapBuffers();

  return;
}

void
reshape(int w, int h)
{
  // viewport transformation to fit the CVV to NDC
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(cam.fovy, ((float) w)/h, -cam.zNear, -cam.zFar);

  screen_w = w;
  screen_h = h;

  return;
}

/*
 * Instructions on using this program:
 *
 * 'I' - reset to original
 * '0' - toggle drawing torus or sphere
 * 'L' - toggle lights on/off
 * '1' - toggle sun light on/off
 * '2' - toggle eye light on/off
 * '3' - toggle object light on/off
 * 'CTRL' - turns on ROTATION mode
 * 'ALT' - turns on TRANSLATION mode
 * 'M' - magnify sphere radius
 * 'm' - minify sphere radius
 *
 * In each mode, pressing the 'l', 'k', 'w' keys
 * makes the transformation in the 'forward' direction,
 * pressing 'h', 'j', 's' makes the transformation in the
 * 'reverse' direction. You are required to implement this
 * functionality. Look at the kbd() function to see how
 * the various functions are called.
 *
*/
void
kbd(unsigned char key, int x, int y)
{
  int mod=0;

  /* Get info about Alt, Ctrl, Shift modifier keys */
  mod = glutGetModifiers();
  if (mod & GLUT_ACTIVE_ALT) {
    mode = TRANSLATION;
  } else if (mod & GLUT_ACTIVE_CTRL) {
    key += 'a'-1;
    mode = ROTATION;
  }

  switch(key) {
        
  case 'q': /* quit */
  case 27 :
    glutDestroyWindow(wd);
    exit (0);

  case 'I': /* reset */
    sphere_radius = INITRAD;
    cam = Camera(eye_pos0, gaze_dir0, top_dir0, zNear0, zFar0, fovy0);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glGetDoublev(GL_TEXTURE_MATRIX, cmodview);
    setup_view();           // in viewing.cpp
    glLightfv(SUNLIGHT, GL_POSITION, sunlight_position);
    glLightfv(OBJLIGHT, GL_POSITION, objlight_position);
    fprintf(stderr, "Reset!\n");
    break;
        
  case '0':
    object = (object == TORUS) ? SPHERE : TORUS;
    break;

  case '1':
    sunlight_on = 1 - sunlight_on;
    if (sunlight_on) {
      glEnable(SUNLIGHT);
    } else {
     glDisable(SUNLIGHT);
    }
    printf("\tsun light %s\n", sunlight_on ? "on" : "off");
    break;

  case '2':
    eyelight_on = 1 - eyelight_on;
    if (eyelight_on) {
      glEnable(EYELIGHT);
    } else {
     glDisable(EYELIGHT);
    }
    printf("\teye light %s\n", eyelight_on ? "on" : "off");
    break;

  case '3':
    objlight_on = 1 - objlight_on;
    if (objlight_on) {
      glEnable(OBJLIGHT);
    } else {
     glDisable(OBJLIGHT);
    }
    printf("\tobj light %s\n", objlight_on ? "on" : "off");
    break;

  case 'M':
    sphere_radius *= MAGFAC;
    printf("Sphere radius magnified to %.3f\n", sphere_radius);
    break;

  case 'm':
    sphere_radius *= MINFAC;
    printf("Sphere radius minified to %.3f\n", sphere_radius);
    break;

  default:
    /* Setup the modelview matrix for modeling transform or
     * the camera coordinate system for viewing transform.
    */
    switch (mode) {
    case ROTATION:
      rotate(key, x, y);    // in modeling.cpp
      break;
    case TRANSLATION:
      translate(key, x, y); // in modeling.cpp
      break;
    case MOVECAM:
      movecam(key, x, y);   // in viewing.cpp
      break;
    default:
      break;
    }
    break;
  }

  /* YOUR CODE HERE . . .
   * After transformations, you want to adjust some or all of your lights.
   * Again, you may need to reposition some lights after every transformation, 
   * others only if the camera is moved, or some others may never need to be 
   * moved at all.
   */
  if (mode == MOVECAM) {
    
    setup_view();             // in viewing.cpp
    /* . . . AND/OR HERE . . . */
    glMultMatrixd(cmodview); // play back all modeling transforms 
  }
  /* . . . AND/OR HERE */
  glLightfv(OBJLIGHT, GL_POSITION, objlight_position);
  if ((mod & GLUT_ACTIVE_ALT) | (mod & GLUT_ACTIVE_CTRL)) {
    mode = MOVECAM;
  }
  
  glutPostRedisplay();

  return;
}

/*
 * Mouse behaviour:
 * drag left/right: pan
 * drag up/down: tilt
 */
void
motion(int x, int y)
{
  if (mode == MOVECAM) {
    movecam(MOUSE_DRAG, (x - mouse_x), (y - mouse_y)); // in viewing.cpp

    setup_view();     // set up the viewing transformation in viewing.cpp
    /* YOUR CODE HERE . . . 
     * Same as within movecam().
    */
    glMultMatrixd(cmodview);  // play back all modeling transforms
    

    /* . . . AND/OR HERE */
    
    mouse_x = x;
    mouse_y = y;
    
    glutPostRedisplay(); /* Refresh screen after changes */
  }

  return;
}

void
mouse(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN && mode == MOVECAM) {
    mouse_x = x;
    mouse_y = y;
  }

  return;
}

int
main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
  glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
  wd = glutCreateWindow("Lab 5: Lighting");
#ifndef __APPLE__
  glewInit();
#endif
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(kbd);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  init();
  glutMainLoop();

  exit(0);
}
