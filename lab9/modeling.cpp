/*
 * Copyright (c) 2009 University of Michigan, Ann Arbor.
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
 * Authors: Ari Grant
 *
*/
#include <cstdlib>
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "xvec.h"
#include "xmat.h"
#include "mesh.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433
#endif // M_PI

// the mesh that will be subdivided
Mesh cube;

// whether the mesh is displayed with wireframe
bool meshHasWireframe = true;

// global rotation values
float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;

void 
initGL()
{
	cube.toCube();
	
	// clear with a dark, pale blue
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	
	// load a simple projection (it will not
	// be changed anywhere else)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.5, 0.5, -0.375, 0.375, 1.0, 7.0);
	
	// enable color materials and
	// setup a basic light
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	// position the light
	XVec4f lightPos(1.0f, 0.5f, -0.2f, 1.0f);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

  return;
}

void 
drawScene()
{
	// some global rotation
	glRotatef(rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(rotY, 0.0f, 1.0f, 0.0f);
	glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
	
	// shrink the sube down
	glScalef(0.3f, 0.3f, 0.3f);
	
	// draw the cube
	cube.draw(meshHasWireframe);

  return;
}

void 
display()
{		
	
	// clear the buffers
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	// prepare the modelview with an offset backward
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -1.6f);
	
	// draw the scene
	drawScene();
	
	// and then swap the framebuffer into view
	glutSwapBuffers();

  return;
}

void kbd(unsigned char key, int x, int y)
{
	switch(key)
	{
			/* quit on 'escape' */
		case '\033':
		case 'q':
			exit(0);
			break;
			
		case 'x':
		case 'j':
			rotX += 3.0f;
			break;
			
		case 'X':
		case 'k':
			rotX -= 3.0f;
			break;
			
		case 'y':
		case 's':
			rotY += 3.0f;
			break;
			
		case 'Y':
		case 'w':
			rotY -= 3.0f;
			break;
			
		case 'z':
		case 'h':
			rotZ += 3.0f;
			break;
			
		case 'Z':
		case 'l':
			rotZ -= 3.0f;
			break;
			
		case 'd':
			cube.subdivide();
			break;
			
		case 'r':
			cube.randomize();
			break;
			
		case 'm':
			meshHasWireframe = !meshHasWireframe;
			break;
			
		case ' ':
		case 'I':
			cube.toCube();
			break;
			
		default:
			break;
	}
	
	// ask GLUT to draw again
	glutPostRedisplay();

  return;
}

int 
main (int argc, char *argv[])
{
	// init GLUT
    glutInit(&argc, argv);
	
	// prepare the window size
    glutInitWindowSize(800, 600);
	
	// create a window with the necessary buffers
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	
	// cast to void to avoid the warning that the
	// returned value (the window handle) is not used
    (void)glutCreateWindow("Lab12: Modeling");
	
	// set the GLUT callbacks. there is
	// no reshape callback since the
	// default one provided by GLUT does
	// all that is needed here (it simply
	// updates the viewport)
    glutDisplayFunc(display);
    glutKeyboardFunc(kbd);
	
	// prepare OpenGL state
	initGL();
	
	// enter GLUT's event loop
	glutMainLoop();
	
	exit(0);
}
