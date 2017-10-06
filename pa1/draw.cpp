/*
 * Copyright (c) 2010 University of Michigan, Ann Arbor.
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
 * Authors:
 *        Ari Grant, grantaa@umich.edu
 *        Sugih Jamin, jamin@eecs.umich.edu
 */
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "canvas.h"
#include "colorpicker.h"

#include "targa.h"

/* different options in the pop-up menu */
enum menuButtons {
  SELECT_MODE_BUTTON,
  DRAW_MODE_BUTTON,
  DELETE_SHAPE_BUTTON,
  SET_ANTIALIASED_BUTTON,
  BRING_FORWARD_BUTTON,
  BRING_TO_FRONT_BUTTON,
  SEND_BACKWARD_BUTTON,
  SEND_TO_BACK_BUTTON,
  GRID_BUTTON,
  SNAP_BUTTON,
  HARDWARE_RENDER_BUTTON,
  NEW_SCENE_BUTTON,
  OPEN_SCENE_BUTTON,
  SAVE_SCENE_BUTTON,
  SAVE_SCENE_AS_BUTTON,
  DRAW_CLIP_AREA_BUTTON,
  TOGGLE_CLIPPING_BUTTON
};

Canvas *canvas;                                 /* the drawing canvas */
ColorPicker *picker;                    /* the color picker */
bool gridOn = false;                    /* if the grid is visible */
bool snapOn = false;                    /* if snapping is enabled */
bool lineAA = false;                    /* if line anti-aliasing is enabled */
bool hardwareRender = false;    /* whether hardware of software rendering */

bool needsSaveAsImage = false;  /* if the canvas needs to be saved as an image */

int width = 640, height = 480;  /* size of the window, initially 640x480 */

int leftEdgeOfPicker;                   /* the left edge of the picker in the window */

string location("");                    /* location of save/open file */
string tempLocation("");                /* for typing save/open file */

bool isTypingFile = false;              /* if the user is typing a filename */
bool fileWillOpen = false;              /* if the user is opening or saving a file */

/* a quick, simply way to have GLUT render a c-string */
inline void
renderBitmapString(void *font, char *s)
{
  for (; *s != '\0'; s++) {
    glutBitmapCharacter(font, *s);
  }

  return;
}

void
processMenuEvent(int value)
{
  if (isTypingFile) {
    tempLocation += value;
  }
        
  /* what to do depending on popup menu selection */
  switch (value) {
  case DELETE_SHAPE_BUTTON:
    canvas->deleteSelected();
    break;
  case SET_ANTIALIASED_BUTTON:
    canvas->toggleSelectedAntialiased();
    break;
  case BRING_FORWARD_BUTTON:
    canvas->bringForward();
    break;
  case BRING_TO_FRONT_BUTTON:
    canvas->bringToFront();
    break;
  case SEND_BACKWARD_BUTTON:
    canvas->sendBackward();
    break;
  case SEND_TO_BACK_BUTTON:
    canvas->sendToBack();
    break;
  case GRID_BUTTON:
    gridOn = !gridOn;
    canvas->setGridOn(gridOn);
    break;
  case SNAP_BUTTON:
    snapOn = !snapOn;
    canvas->setSnapOn(snapOn);
    break;
  case HARDWARE_RENDER_BUTTON:
    hardwareRender = !hardwareRender;
    canvas->setHardwareRender(hardwareRender);
    break;
  case NEW_SCENE_BUTTON:
    canvas->newScene();
    break;
  case OPEN_SCENE_BUTTON:
    isTypingFile = true;
    tempLocation = "";
    fileWillOpen = true;
    break;
  case SAVE_SCENE_BUTTON:
    canvas->saveScene(location);
    if (location == "") {
      isTypingFile = true;
      tempLocation = "";
      fileWillOpen = false;
    }
    needsSaveAsImage = true;
    break;
  case SAVE_SCENE_AS_BUTTON:
    isTypingFile = true;
    tempLocation = "";
    fileWillOpen = false;
    break;
  case DRAW_CLIP_AREA_BUTTON:
    canvas->startDrawingClipView();
    break;
  case TOGGLE_CLIPPING_BUTTON:
    canvas->toggleIsDrawingClipped();
    break;
  default:
    break;
  }
  glutPostRedisplay();

  return;
}

void
createMenus()
{
  /* registers the callback function for clicking an item in the popup menu */
  glutCreateMenu(processMenuEvent);
        
  /* add all the entries to the popup */
  glutAddMenuEntry("Grid On/Off\t\t\t\t\t(g)", GRID_BUTTON);
  glutAddMenuEntry("Snap On/Off\t\t\t\t(x)", SNAP_BUTTON);

  glutAddMenuEntry("Delete Triangle\t\t\t\t(backspace)", DELETE_SHAPE_BUTTON);
  glutAddMenuEntry("Anti-Alias Shape On/Off\t\t(a)", SET_ANTIALIASED_BUTTON);
        
  glutAddMenuEntry("Hardware Render On/Off\t(h)", HARDWARE_RENDER_BUTTON);
  glutAddMenuEntry("Draw Clip Area\t\t\t\t(c)", DRAW_CLIP_AREA_BUTTON);
  glutAddMenuEntry("Toggle Clipping On/Off\t\t\t(CTRL+c)", TOGGLE_CLIPPING_BUTTON);

  glutAddMenuEntry("Bring Forward\t\t\t\t(f)", BRING_FORWARD_BUTTON);
  glutAddMenuEntry("Bring To Front\t\t\t\t(CTRL+f)", BRING_TO_FRONT_BUTTON);
  glutAddMenuEntry("Send Backward\t\t\t\t(b)", SEND_BACKWARD_BUTTON);
  glutAddMenuEntry("Send To Back\t\t\t\t(CRTL+b)", SEND_TO_BACK_BUTTON);
        
  glutAddMenuEntry("New Scene\t\t\t\t\t(CTRL+n)", NEW_SCENE_BUTTON);
  glutAddMenuEntry("Open Scene\t\t\t\t\t(CTRL+o)", OPEN_SCENE_BUTTON);
  glutAddMenuEntry("Save Scene\t\t\t\t\t(CTRL+s)", SAVE_SCENE_BUTTON);
  glutAddMenuEntry("Save Scene As...\t\t\t\t(CTRL+SHIFT+s)", SAVE_SCENE_BUTTON);
        
  /* attach it to the secondary mouse button */
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void
init()
{
  /* set clear color to light grey and enable alpha blending */
  glClearColor(0.8, 0.8, 0.8, 1.0);
        
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
}

void
display()
{
  glClear(GL_COLOR_BUFFER_BIT);
        
  /* get the currently selected vertex's color and
     set it to be the current color of the color picker */
  picker->currentColor() = canvas->currentColor();
        
  /* hide grid and selection */
  if (needsSaveAsImage) {
    canvas->setGridOn(false);
    canvas->setSnapOn(false);
    canvas->setSelected(-1);
  }
        
  /* draw the canvas and the picker */
  canvas->draw();
        
  if (canvas->selected() != NULL) {
    picker->draw();
  } else {
    picker->drawNoSelection();
  }
        
  /* set the viewport and draw the words 'software render'
     or 'hardware render' depending on which is current */
  glViewport(0, 0, width, height);
  glColor4f(0.4, 0.4, 0.5, 1.0);
  glRasterPos2f(1, 2);
        
  if (hardwareRender && !isTypingFile) {
    char str[] = "Hardware Render";
    renderBitmapString(GLUT_BITMAP_HELVETICA_10, str);
  } else {
    char str[] = "Software Render";
    renderBitmapString(GLUT_BITMAP_HELVETICA_10, str);
  }
        
  if (isTypingFile) {
    if (fileWillOpen) {
      char str[] = "Enter name of file to open: ";
      glRasterPos2f(1, height-60);
      renderBitmapString(GLUT_BITMAP_HELVETICA_10, str);
      renderBitmapString(GLUT_BITMAP_HELVETICA_10, (char *)tempLocation.c_str());
    } else {
      char str[] = "Enter name of file for save: ";
      glRasterPos2f(1, height-60);
      renderBitmapString(GLUT_BITMAP_HELVETICA_10, str);
      renderBitmapString(GLUT_BITMAP_HELVETICA_10, (char *)tempLocation.c_str());
    }
  }

  glutSwapBuffers();
        
  /* save as image */
  if (needsSaveAsImage) {
                
    XVec4f clipView = canvas->canvasRect();
    XVec2i offset = canvas->offset();
                
    int imageWidth = (int)clipView(2);
    int imageHeight = (int)clipView(3);
        
    unsigned char *data = new unsigned char[imageWidth*imageHeight*4];
        
    glReadPixels(offset(0)+(int)clipView(0),offset(1)+(int)clipView(1),imageWidth,imageHeight,GL_RGBA,GL_UNSIGNED_BYTE,data);
                
    printToTarga(location, imageWidth, imageHeight, data);
    delete [] data;
        
    needsSaveAsImage = false;
  }
}

void
reshape(int w, int h)
{
  width = w;
  height = h;
        
  /* the picker has constant size, so the canvas' size and
     the picker's location are calculated accordingly and the
     left edge of the picker is saved for dispatching mouse
     clicks to the canvas or to the color picker */
  canvas->resize(w - 210, h - 30);
  picker->resize(180, 435, w - 190, h - 455);
  leftEdgeOfPicker = w - 190;
  glutPostRedisplay();
}

void
keyboard(unsigned char key, int x, int y)
{
  if (isTypingFile) {
                
    if (key == 127 || key == 8) { //backspace
      if (tempLocation.size() > 0) {
        tempLocation = tempLocation.erase(tempLocation.size()-1);
      }
    } else if (key == 27) { //escape key
      tempLocation = "";
      isTypingFile = false;

    } else if (key == 13) { //enter key
      if (fileWillOpen) {
        if (canvas->openScene(tempLocation)) {
          location = tempLocation;
          tempLocation = "";
        }
        isTypingFile = false;
      } else {
        canvas->saveScene(tempLocation);
        isTypingFile = false;
        location = tempLocation;
        tempLocation = "";
                                
        needsSaveAsImage = true;
      }
    } else {
      tempLocation += key;
    }
        
    glutPostRedisplay();
    return;
  }
        
  bool controlKey = false;
  if (glutGetModifiers() & GLUT_ACTIVE_CTRL) {
    key += 'a'-1;
    controlKey = true;
  }
        
  switch (key) {
  case 27:      //escape key
    canvas->cancelDrawing();
    break;
  case 8:
  case 127:     //backspace
    canvas->deleteSelected();
    break;
  case 'a':
    canvas->toggleSelectedAntialiased();
    break;
  case 'f':
    if (controlKey) {
      canvas->bringToFront();
    } else {
      canvas->bringForward();
    }
    break;
  case 'b':
    if (controlKey) {
      canvas->sendToBack();
    } else {
      canvas->sendBackward();
    }
    break;
  case 'g':
    gridOn = !gridOn;
    canvas->setGridOn(gridOn);
    break;
  case 'x':
    snapOn = !snapOn;
    canvas->setSnapOn(snapOn);
    break;
  case 'h':
    hardwareRender = !hardwareRender;
    canvas->setHardwareRender(hardwareRender);
    break;
  case 'n':
    if (controlKey) {
      canvas->newScene();
    }
    break;
  case 'o':
    if (controlKey) {
      isTypingFile = true;
      tempLocation = "";
      fileWillOpen = true;
    }
    break;
  case 's':
    if (controlKey) {
      if (location == "" || (glutGetModifiers() & GLUT_ACTIVE_SHIFT)) {
        isTypingFile = true;
        tempLocation = "";
        fileWillOpen = false;
      }
      needsSaveAsImage = true;
    }
    break;
  case 19:
    isTypingFile = true;
    tempLocation = "";
    fileWillOpen = false;
    break;
  case 'c':
    if (controlKey) {
      canvas->toggleIsDrawingClipped();
    } else {
      canvas->startDrawingClipView();
    }
    break;
  default:
    break;
  }
  glutPostRedisplay();
}

void
specialKeyboard(int key, int x, int y)
{
  if (key == LEFT || key == UP || key == RIGHT || key == DOWN) {
    canvas->nudge(key);
  }
  glutPostRedisplay();
}

void
mouse(int button, int state, int x, int y)
{
  /* this is called when the mouse goes up or down */
        
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    /* if the left button is released is in the canvas */
    if (x < leftEdgeOfPicker) {
      /* get the currently selected object */
      Line *l = canvas->selected();
                        
      /* tell the canvas where it was released */
      /* if the return value is true, it means a new
         triangle was made, so its colors of the previous
         selection are added to the color picker */
      if (canvas->mouseReleasedAt(x-15, y-20) && l != NULL) {
        if (l->type() == TRIANGLE) {
          picker->setPreviousColors(l->color0, l->color1, ((Triangle *)l)->color2);
        } else {
          XVec4f blue = XVec4f(0,0,1,1);
          picker->setPreviousColors(l->color0, l->color1, blue);
        }
      }
    } else {
      /* if the left button is released in the picker tell the picker
         and update the canvas' selected triangle with the picker's color */
      picker->mouseReleasedAt(x - leftEdgeOfPicker, 435 - (y - 20));
      canvas->setCurrentColor(picker->currentColor());
    }
  } else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    /* on a mouse down event simply pass it on to the correct view */
    if (x < leftEdgeOfPicker) {
      canvas->mousePressedAt(x-15, y-20);
    } else {
      picker->mousePressedAt(x - leftEdgeOfPicker, 435 - (y - 20));
      canvas->setCurrentColor(picker->currentColor());
    }
  }
  glutPostRedisplay();
}

void
passiveMotion(int x, int y)
{
  /* when the mouse moves with no button down, tell the canvas */
  canvas->mouseMovedTo(x-15, y-20);
  glutPostRedisplay();
}

void
motion(int x, int y)
{
  /* when the mouse moves with a button down, tell the canvas or picker */
  if (x > leftEdgeOfPicker) {
    picker->mouseReleasedAt(x - leftEdgeOfPicker, 435 - (y - 20));
    canvas->setCurrentColor(picker->currentColor());
  } else {
    canvas->mouseMovedTo(x-15, y-20);
  }
  glutPostRedisplay();
}

int
main(int argc, char * argv[])
{
        
  /* create window and register callbacks */
        
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutCreateWindow("EECS487 PA1: Rasterization");
        
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialKeyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutPassiveMotionFunc(passiveMotion);
        
  /* create canvas, picker, and menus */
  canvas = new Canvas;
  picker = new ColorPicker;
        
  createMenus();

  /* begin the application */
  init();
  glutMainLoop();
        
  /* cleanup at the end */
  delete canvas;
  delete picker;
        
  exit(0);
}
