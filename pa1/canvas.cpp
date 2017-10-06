/*
 * Copyright (c) 2010, 2015 University of Michigan, Ann Arbor.
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

#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "canvas.h"
#include "rasterizer.h"

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

void drawPoint(XVec2f &point, XVec4f &pointColor);

inline XVec4f
inBW(XVec4f &color)
{
  float grey = XVec4f(0.3,0.59,0.11,0.0).dot(color);
  return XVec4f(grey, grey, grey, color.alpha());
}

inline bool
areNbors(XVec2f a, XVec2f b)
{
  /* determines if a and b are close to each other */
  float dx = a.x() - b.x();
  float dy = a.y() - b.y();
  
  return (dx < 1.5*GRID_SIZE &&
           dx > -1.5*GRID_SIZE &&
           dy <  1.5*GRID_SIZE &&
           dy > -1.5*GRID_SIZE);
}

void
drawPoint(XVec2f &point, XVec4f &pointColor)
{
  /* This sets the pixel point to have the color pointColor. clipView is
     four integers (viewLowerLeftX, viewLowerLeftY, width, height). */
        
  glColor4fv(pointColor);
  glVertex2f(point.x(), point.y());

  return;
}

void
drawBox(XVec2f p)
{
  /* draws a small white box around the point p
     with a small black box inside it */
  
  glBegin(GL_LINE_LOOP);
  glVertex2f(p.x() + GRID_SIZE, p.y() + GRID_SIZE);
  glVertex2f(p.x() + GRID_SIZE, p.y() - GRID_SIZE);
  glVertex2f(p.x() - GRID_SIZE, p.y() - GRID_SIZE);
  glVertex2f(p.x() - GRID_SIZE, p.y() + GRID_SIZE);
  glEnd();
  
  glColor4f(0.0,0.0,0.0,1.0);
  glBegin(GL_LINE_LOOP);
  glVertex2f(p.x() + GRID_SIZE - 1, p.y() + GRID_SIZE - 1);
  glVertex2f(p.x() + GRID_SIZE - 1, p.y() - GRID_SIZE + 1);
  glVertex2f(p.x() - GRID_SIZE + 1, p.y() - GRID_SIZE + 1);
  glVertex2f(p.x() - GRID_SIZE + 1, p.y() + GRID_SIZE - 1);
  glEnd();

  return;
}

Canvas::
Canvas()
{
  /* the user has not begun drawing */
  firstMouse = XVec2f(-1,-1);
  secondMouse = XVec2f(-1,-1);
  tempSecondMouse = XVec2f(-1,-1);
  tempThirdMouse = XVec2f(-1,-1);
  
  /* the colors start off as red, green, blue */
  color0 = XVec4f(1,0,0,1);
  color1 = XVec4f(0,1,0,1);
  color2 = XVec4f(0,0,1,1);
  
  /* the grid is invisible, snapping is off, clipping is off software rendering */
  gridOn = false;
  snapOn = false;
  isDrawingClipped = false;
  isHardwareRender = false;
  
  /* no triangle is selected */
  indexOfSelected = -1;
  selectedVertex = 0;
  
  /* the user is not drawing a clipping rectangle */
  isDrawingClipArea = false;
  clipView = XVec4f(0,0,430,450);

  return;
}

void Canvas::
resize(int w, int h)
{
  /* calaculate the new canvas size */
  canvasWidth = w - w % GRID_SIZE + 1;
  canvasHeight = h - h % GRID_SIZE + 1;
  
  /* store the new window size */
  width = w;
  height = h;

  return;
}

void Canvas::
mousePressedAt(int x, int y)
{
  /* if nothing has yet been pressed,
     that is, drawing is just beggining */
  if (firstMouse.x() == -1) {
    
    /* flip the y coordinate since GLUT's origin
       for the mouse is at the upper left and
       OpenGL's is at the lower left */
    y = height - y;
    
    /* if snapping is on convert x and y to the
       nearest grid points */
    if (snapOn) {
      int gridOffsetX = x % GRID_SIZE;
      int gridOffsetY = y % GRID_SIZE;
      
      x -= gridOffsetX;
      if (gridOffsetX > GRID_SIZE/2)
        x += GRID_SIZE;
      
      y -= gridOffsetY;
      if (gridOffsetY > GRID_SIZE/2)
        y += GRID_SIZE;
    }
    /* first mouse location is at this point */
    firstMouse = XVec2f(x,y);
    
    /* set colors to be selected triangle */
    if (indexOfSelected != -1) {
      color0 = shapes[indexOfSelected]->color0;
      color1 = shapes[indexOfSelected]->color1;
      
      if (shapes[indexOfSelected]->type() == TRIANGLE) {
        color2 = ((Triangle *)shapes[indexOfSelected])->color2;
      }
    }
  }

  return;
}

bool Canvas::
mouseReleasedAt(int x, int y)
{
  /* this returns a boolean to note if the
     selection has changed at all */
  
  /* if it was not pressed down in here, stop */
  if (firstMouse.x() == -1) {
    return false;
  }
  
  /* flip the y coordinate since GLUT's origin
     for the mouse is at the upper left and
     OpenGL's is at the lower left */
  y = height - y;
  
  XVec2f position(x,y);
  
  if (isDrawingClipArea) {
    if (snapOn) {
      /* if snapping is on convert x and y to the
         nearest grid points */
      int gridOffsetX = x % GRID_SIZE;
      int gridOffsetY = y % GRID_SIZE;
      
      x -= gridOffsetX;
      if (gridOffsetX > GRID_SIZE/2) {
        x += GRID_SIZE;
      }
      
      y -= gridOffsetY;
      if (gridOffsetY > GRID_SIZE/2) {
        y += GRID_SIZE;
      }
      
      position = XVec2f(x,y);
    }
    
    float origX = x;
    if (firstMouse.x() <  x) {
      origX = firstMouse.x();
    }
    
    float origY = y;
    if (firstMouse.y() <  y) {
      origY = firstMouse.y();
    }
    
    float w = fabs(firstMouse.x() - x);
    float h = fabs(firstMouse.y() - y);
    
    if (w != 0 && h != 0) {
      clipView = XVec4f(origX, origY, w, h);
    }
    
    /* clear mouse data */
    cancelDrawing();
    return false;
  }
  
  /* if the mouse was pressed and released 'in the same place' and
     no drawing has begun, then the user is selecting a triangle */
  if (areNbors(position, firstMouse) &&
      secondMouse.x() == -1 && tempSecondMouse.x() == -1) {
    
    /* When interacting with graphical interfaces, picking/selection is
       always an issue (finding what object is under the mouse). The
       application has a simple selection model. This checks if the click
       location is nearby any vertices of the already selected triangle and
       if so sets that vertex to be the current selection. If the click
       point is not close to any vertex of the selected triangle, then the
       selection is cleared. It then loops through all of the shapes
       and finds the top-most triangle containing the click point. Index 0
       of the vector is the lowest triangle so the one it finds is that
       with the highest index. If such a triangle is found the selection
       is set to be that triangle. Then it finds out which vertex is
       closest to the click point. */
    
    /* reset the mouse-down location */
    firstMouse = XVec2f(-1,-1);
    
    /* if the click point is near one of the vertices of the selected
       triangle, then select that vertex. this allows the user to click
       on the 'box' at the endpoints even though most of the box does
       not actually contain the triangle and thus the click point may
       not be inside of it. */
    if (indexOfSelected != -1) {
      if (areNbors(shapes[indexOfSelected]->vertex0, position)) {
        setSelectedVertex(0);
        return true;

      } else if (areNbors(shapes[indexOfSelected]->vertex1, position)) {
        setSelectedVertex(1);
        return true;

      } else if (shapes[indexOfSelected]->type() == TRIANGLE) {
        if (areNbors(((Triangle *)shapes[indexOfSelected])->vertex2, position)) {
          setSelectedVertex(2);
          return true;
        }
      }
    }
    
    /* select nothing */
    setSelected(-1);
    
    /* loop through all shapes and find the top-most one (closest to
       the end of the array) that contains the point 'position', if none
       was found, then return false. Otherwise call setSelected with the 
       given index, that is if it is the bottom-most triangle, call
       setSelected(0). Then find out which vertex is closest to 'position'
       and call setSelectedVertex() with 0, 1, or 2 and return false. */
    /* PICKING */
    
    /* this is a dummy since pointIsInTriangle needs a color passed in */
    XVec4f color;
    
    /* loop though all shapes and find the top-most containing
       the mouse click */
    int count = shapes.size();
    for (int i = count - 1; i >= 0; i--) {
      if (shapes[i]->type() == TRIANGLE) {
        if (((Triangle *)shapes[i])->containsPoint(position, color)) {
          setSelected(i);
          break;
        }

      } else {

        /* find the distance between the click point and the line */
        /* given vector BA and point C, dist vec is C - proj of CA onto BA */
        XVec2f lineBA = shapes[i]->vertex1 - shapes[i]->vertex0;
        XVec2f lineCA = position - shapes[i]->vertex0;
        
        XVec2f distanceVec = lineCA - lineCA.project(lineBA);
                                
        /* if length of distance vec is small, select the line */
        if (distanceVec.dot() < 2*GRID_SIZE*GRID_SIZE) {
          setSelected(i);
          break;
        }
      }
    }
    /* if no triangle was found, return that nothing was selected */
    if (indexOfSelected == -1) {
      return false;
    }
    
    /* find the distance to each vertex of the shapes */
    float dist0 = shapes[indexOfSelected]->vertex0.dist(position);
    float dist1 = shapes[indexOfSelected]->vertex1.dist(position);
    float dist2 = 1E10;
    
    if (shapes[indexOfSelected]->type() == TRIANGLE) {
      dist2 = ((Triangle *)shapes[indexOfSelected])->vertex2.dist(position);
    }
    
    /* set the closest vertex to be the currently selected one */
    if (dist0 <= dist1 && dist0 <= dist2) {
      setSelectedVertex(0);
      return true;

    } else if (dist1 <= dist0 && dist1 <= dist2) {
      setSelectedVertex(1);
      return true;

    } else if (dist2 <= dist0 && dist2 <= dist1) {
      setSelectedVertex(2);
      return true;
    }

  } else {                
    /* if drawing has begun or the user pressed, dragged, and released,
       then a triangle is being drawn */

    if (snapOn) {
      /* if snapping is on convert x and y to the
         nearest grid points */
      int gridOffsetX = x % GRID_SIZE;
      int gridOffsetY = y % GRID_SIZE;
      
      x -= gridOffsetX;
      if (gridOffsetX > GRID_SIZE/2) {
        x += GRID_SIZE;
      }
      
      y -= gridOffsetY;
      if (gridOffsetY > GRID_SIZE/2) {
        y += GRID_SIZE;
      }
      
      position = XVec2f(x,y);
    }
    
    /* if the user was dragging, tempSecondMouse was the mouse
       location, then set the second mouse location and clear out
       the temporary mouse location */
    if (secondMouse.x() == -1) {
      secondMouse = position;
      
      tempThirdMouse = position;
      tempSecondMouse = XVec2f(-1,-1);
    } else {
      /* else a third point was clicked and a new shape is made */

      /* a line was drawn */
      if (secondMouse == tempThirdMouse) {
        Line* newLine = new Line();
        newLine->vertex0 = firstMouse;
        newLine->vertex1 = secondMouse;
        newLine->color0 = color0;
        newLine->color1 = color1;
        
        newLine->isAntialiased = false;
        
        shapes.push_back(newLine);
        setSelected(shapes.size() - 1);
        
        /* clear mouse data */
        cancelDrawing();
        
        /* a new shape was made */
        return true;
      } else {
        /* a triangle was drawn */
        Triangle* newTriangle = new Triangle();
        newTriangle->vertex0 = firstMouse;
        newTriangle->vertex1 = secondMouse;
        newTriangle->vertex2 = tempThirdMouse;
        newTriangle->color0 = color0;
        newTriangle->color1 = color1;
        newTriangle->color2 = color2;
        
        newTriangle->isAntialiased = false;
        
        shapes.push_back(newTriangle);
        setSelected(shapes.size() - 1);
        
        /* clear mouse data */
        cancelDrawing();
        
        /* a new shape was made */
        return true;
      }
    }
  }

  return false;
}

void Canvas::
mouseMovedTo(int x, int y)
{
  /* if drawing has not begun, then nothing
     happens when the mouse it moved */
  if (firstMouse.x() == -1) {
    return;
  }
  
  /* flip the y coordinate since GLUT's origin
     for the mouse is at the upper left and
     OpenGL's is at the lower left */
  y = height - y;
  
  XVec2f position(x,y);
  
  /* if snapping is on convert x and y to the
     nearest grid points */
  if (snapOn) {
    int gridOffsetX = x % GRID_SIZE;
    int gridOffsetY = y % GRID_SIZE;
    
    x -= gridOffsetX;
    if (gridOffsetX > GRID_SIZE/2) {
      x += GRID_SIZE;
    }
    
    y -= gridOffsetY;
    if (gridOffsetY > GRID_SIZE/2) {
      y += GRID_SIZE;
    }
    
    position = XVec2f(x,y);
  }
  
  if (isDrawingClipArea) {
    float origX = x;
    if (firstMouse.x() <  x) {
      origX = firstMouse.x();
    }
    
    float origY = y;
    if (firstMouse.y() <  y) {
      origY = firstMouse.y();
    }
    
    float w = fabs(firstMouse.x() - x);
    float h = fabs(firstMouse.y() - y);
    
    if (w != 0 && h != 0) {
      clipView = XVec4f(origX, origY, w, h);
    }
  }
  
  /* if the mouse was clicked once already then store
     its location temporarily as the second click */
  if (secondMouse.x() == -1) {
    tempSecondMouse = position;

  } else {
    /* if the mouse was clicked twice already then store
       its location temporarily as the third click */
    tempThirdMouse = position;
  }

  return;
}

void Canvas::
drawInRect(XVec4f &clipWin, bool inColor)
{
  /* draw all the shapes stored thus far */
  int count = shapes.size();
  
  /* set where to draw for triangles */
  XVec2i corner = offset();
  glScissor((int)clipWin(0) + corner.x(), (int)clipWin(1) + corner.y(), (int)clipWin(2), (int)clipWin(3));
  
  for (int i = 0; i < count; i++) {
    /* if drawing in blank and white temporarily change the colors */
    XVec4f color0, color1, color2;
    if (!inColor) {
      color0 = shapes[i]->color0;
      shapes[i]->color0 = inBW(shapes[i]->color0);
      
      color1 = shapes[i]->color1;
      shapes[i]->color1 = inBW(shapes[i]->color1);
      
      if (shapes[i]->type() == TRIANGLE) {
        color2 = ((Triangle *)shapes[i])->color2;
        ((Triangle *)shapes[i])->color2 = inBW(((Triangle *)shapes[i])->color2);
      }
    }
    
    /* if software rendering, use student's code */
    if (isHardwareRender == false) {
      
      if (shapes[i]->type() == TRIANGLE) {
        glEnable(GL_SCISSOR_TEST);
      }
      
      glBegin(GL_POINTS);
      shapes[i]->drawInRect(clipWin);
      glEnd();
      
      glDisable(GL_SCISSOR_TEST);

    } else {
    /* otherwise use OpenGL to render */
      if (shapes[i]->isAntialiased) {
        glEnable(GL_POLYGON_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
      }
      
      glEnable(GL_SCISSOR_TEST);
      
      if (shapes[i]->type() == LINE) {
        glBegin(GL_LINES);
        glColor4fv(shapes[i]->color0);
        glVertex2fv(shapes[i]->vertex0);
        glColor4fv(shapes[i]->color1);
        glVertex2fv(shapes[i]->vertex1);
        glEnd();
      } else {
        glBegin(GL_TRIANGLES);
        glColor4fv(shapes[i]->color0);
        glVertex2fv(shapes[i]->vertex0);
        glColor4fv(shapes[i]->color1);
        glVertex2fv(shapes[i]->vertex1);
        glColor4fv(((Triangle *)shapes[i])->color2);
        glVertex2fv(((Triangle *)shapes[i])->vertex2);
        glEnd();
      }
      
      glDisable(GL_SCISSOR_TEST);
      glDisable(GL_POLYGON_SMOOTH);
      glDisable(GL_LINE_SMOOTH);
    }
    
    /* return original colors */
    if (!inColor) {
      shapes[i]->color0 = color0;
      shapes[i]->color1 = color1;
      
      if (shapes[i]->type() == TRIANGLE) {
        ((Triangle *)shapes[i])->color2 = color2;
      }
    }
  }
  
  /* if a triangle is under construction, draw it */
  if (tempThirdMouse.x() != -1) {
    
    if (isHardwareRender) {
      glEnable(GL_SCISSOR_TEST);
      
      glBegin(GL_TRIANGLES);
      glColor4fv((inColor) ? color0 : inBW(color0));
      glVertex2fv(firstMouse);
      glColor4fv((inColor) ? color1 : inBW(color1));
      glVertex2fv(secondMouse);
      glColor4fv((inColor) ? color2 : inBW(color2));
      glVertex2fv(tempThirdMouse);
      glEnd();
      
      glDisable(GL_SCISSOR_TEST);
    } else {
      Triangle t;
      
      t.vertex0 = firstMouse;
      t.vertex1 = secondMouse;
      t.vertex2 = tempThirdMouse;
      
      t.color0 = (inColor) ? color0 : inBW(color0);
      t.color1 = (inColor) ? color1 : inBW(color1);
      t.color2 = (inColor) ? color2 : inBW(color2);
      t.isAntialiased = false;
      
      glEnable(GL_SCISSOR_TEST);
      
      glBegin(GL_POINTS);
      t.drawInRect(clipWin);
      glEnd();
      
      glDisable(GL_SCISSOR_TEST);
    }
  }
  
  /* if the user has clicked once and is dragging, draw a line */
  if (firstMouse.x() != -1 && tempSecondMouse.x() != -1) {
    if (!isDrawingClipArea) {
      
      XVec2f p0(firstMouse);
      XVec2f p1(tempSecondMouse);
      
      if (isHardwareRender) {
        glBegin(GL_LINES);
        glColor4fv((inColor) ? color0 : inBW(color0));
        glVertex2fv(p0);
        glColor4fv((inColor) ? color1 : inBW(color1));
        glVertex2fv(p1);
        glEnd();
      } else {
        Line l;
        
        l.vertex0 = p0;
        l.color0 = (inColor) ? color0 : inBW(color0);
        l.vertex1 = p1;
        l.color1 = (inColor) ? color1 : inBW(color1);
        l.isAntialiased = false;
        
        glBegin(GL_POINTS);
        l.drawInRect(clipWin);
        glEnd();
      }
    }
  }
  
  /* if the grid is on, draw a grid */
  if (gridOn) {
    glColor4f(0.3,0.3,0.3,0.4);
    glBegin(GL_LINES);
    
    for (int i = 1; i <= canvasWidth; i += GRID_SIZE) {
      glVertex2f(i,0);
      glVertex2f(i,canvasHeight);
    }
    for (int i = 1; i <= canvasHeight; i += GRID_SIZE) {
      glVertex2f(0,i);
      glVertex2f(canvasWidth,i);
    }
    glEnd();
  }

  return;
}

void Canvas::
draw()
{
  /* draw only where the canvas is */
  glViewport((width % GRID_SIZE)/2 + 10, (height % GRID_SIZE)/2 + 10,
              canvasWidth, canvasHeight);
        
  /* prepare an orthographic projection such that drawing
     commands directly specify pixel coordinates */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
        
  glOrtho(0, canvasWidth, 0, canvasHeight, -1.f, 1.f);
        
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /* for pixel-accurate rendering */
  glTranslatef(0.375f, 0.375f, 0.0f);
        
  /* draw a light grey background for the canvas */
  glColor4f(0.9, 0.9, 0.9, 1.0);
  glBegin(GL_QUADS);
  glVertex2f(width, 0);
  glVertex2f(width, height);    
  glVertex2f(0, height);        
  glVertex2f(0, 0);
  glEnd();
        
  XVec4f screen(0,0,canvasWidth,canvasHeight);
        
  /* draw the entire screen, it is in color if not
     drawing everything clipped */
  drawInRect(screen, !isDrawingClipped);
        
  if (isDrawingClipped) {
    /* draw a light grey background for the canvas */
    if (!isDrawingClipArea) {
      glColor4f(0.9, 0.9, 0.9, 1.0);
      glBegin(GL_QUADS);
      glVertex2f(clipView(0) - GRID_SIZE, clipView(1) - GRID_SIZE);
      glVertex2f(clipView(0) + clipView(2) + GRID_SIZE, clipView(1) - GRID_SIZE);
      glVertex2f(clipView(0) + clipView(2) + GRID_SIZE, clipView(1) + clipView(3) + GRID_SIZE);
      glVertex2f(clipView(0) - GRID_SIZE, clipView(1) + clipView(3) + GRID_SIZE);
      glEnd();
    }
    /* draw rect in smaller view to force clipping code.
       drawn in color. */
    drawInRect(clipView, true);
  }
        
  /* if a triangle is selected, draw a box at each vertex,
     but draw the selected vertex with a yellow box */
  if (indexOfSelected != -1) {
                
    glBegin(GL_LINES);
    glColor4f(0.0,0.0,0.0,0.7);
    glVertex2fv(shapes[indexOfSelected]->vertex0);
    glVertex2fv(shapes[indexOfSelected]->vertex1);
                
    if (shapes[indexOfSelected]->type() == TRIANGLE) {
      glVertex2fv(shapes[indexOfSelected]->vertex0);
      glVertex2fv(((Triangle *)shapes[indexOfSelected])->vertex2);
      glVertex2fv(shapes[indexOfSelected]->vertex1);
      glVertex2fv(((Triangle *)shapes[indexOfSelected])->vertex2);
    }
    glEnd();
                
    if (selectedVertex == 0) {
      glColor4f(1.0,1.0,0.0,1.0);
    } else {
      glColor4f(1.0,1.0,1.0,1.0);
    }
    drawBox(shapes[indexOfSelected]->vertex0);
                
    if (selectedVertex == 1) {
      glColor4f(1.0,1.0,0.0,1.0);
    } else {
      glColor4f(1.0,1.0,1.0,1.0);
    }
    drawBox(shapes[indexOfSelected]->vertex1);
                
    /* if it is a line don't draw the third box */
    if (shapes[indexOfSelected]->type() == TRIANGLE) {
      if (selectedVertex == 2) {
        glColor4f(1.0,1.0,0.0,1.0);
      } else {
        glColor4f(1.0,1.0,1.0,1.0);
      }
      drawBox(((Triangle *)shapes[indexOfSelected])->vertex2);
    }
  }
        
  /* if snapping is on, draw a red magnet in
     the upper left corner of the canvas */
  if (snapOn) {
    glBegin(GL_TRIANGLE_STRIP);
    glColor4f(0.0,0.0,0.0,0.5);
    glVertex2f(10,height-33);
    glVertex2f(15,height-33);
                
    glVertex2f(10,height-30);
    glVertex2f(15,height-30);
                
    glColor4f(1.0,0.0,0.0,0.5);
    glVertex2f(10,height-30);
    glVertex2f(15,height-30);

    glVertex2f(8,height-20);
    glVertex2f(13,height-20);

    glVertex2f(11,height-11);
    glVertex2f(15,height-16);

    glVertex2f(20,height-7);
    glVertex2f(20,height-14);
                
    glVertex2f(29,height-11);
    glVertex2f(25,height-16);
                
    glVertex2f(32,height-20);
    glVertex2f(27,height-20);
                
    glVertex2f(30,height-30);
    glVertex2f(25,height-30);
                
    glColor4f(0.0,0.0,0.0,0.5);
    glVertex2f(30,height-30);
    glVertex2f(25,height-30);
                
    glVertex2f(30,height-33);
    glVertex2f(25,height-33);
                
    glEnd();
  }
                
  /* draw rectangle to denote clipping selection */
  if (isDrawingClipArea) {
    glColor4f(0.0,0.0,0.0,1.0);
    glBegin(GL_LINE_LOOP);
                
    glVertex2f(3,3);
    glVertex2f(canvasWidth-3,3);
    glVertex2f(canvasWidth-3,canvasHeight-3);
    glVertex2f(3,canvasHeight-3);
                
    glEnd();
                
    glColor4f(0.3,0.3,0.3,1.0);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(5, 0xAAAA);
    glBegin(GL_LINE_LOOP);
                
    glVertex2f(clipView(0), clipView(1));
    glVertex2f(clipView(0)+clipView(2), clipView(1));
    glVertex2f(clipView(0)+clipView(2), clipView(1)+clipView(3));
    glVertex2f(clipView(0), clipView(1)+clipView(3));
                
    glEnd();
    glDisable(GL_LINE_STIPPLE);

  } else if (isDrawingClipped) {
    /* draw dark region outside clipView */
    glColor4f(0.3,0.3,0.3,0.6);
    glBegin(GL_QUADS);
                
    glVertex2f(0,0);
    glVertex2f(clipView(0), clipView(1));
    glVertex2f(clipView(0), clipView(1)+clipView(3));
    glVertex2f(0, canvasHeight);
                
    glVertex2f(0,0);
    glVertex2f(canvasWidth, 0);
    glVertex2f(clipView(0)+clipView(2), clipView(1));
    glVertex2f(clipView(0), clipView(1));
                
    glVertex2f(canvasWidth, 0);
    glVertex2f(canvasWidth, canvasHeight);
    glVertex2f(clipView(0)+clipView(2), clipView(1)+clipView(3));
    glVertex2f(clipView(0)+clipView(2), clipView(1));
                
    glVertex2f(canvasWidth, canvasHeight);
    glVertex2f(0, canvasHeight);
    glVertex2f(clipView(0), clipView(1)+clipView(3));
    glVertex2f(clipView(0)+clipView(2), clipView(1)+clipView(3));
                
    glEnd();
  }

  return;
}

void Canvas::
setGridOn(bool isOn)
{
  gridOn = isOn;

  return;
}

void Canvas::
setSnapOn(bool isOn)
{
  snapOn = isOn;

  return;
}

void Canvas::
setSelected(int index)
{
  indexOfSelected = index;
        
  /* if nothing is selected, the drawing colors are red, green, blue */
  if (indexOfSelected == -1) {
    color0 = XVec4f(1,0,0,1);
    color1 = XVec4f(0,1,0,1);
    color2 = XVec4f(0,0,1,1);

  } else {
    /* otherwise, the drawing colors are those of the selection */
    color0 = shapes[indexOfSelected]->color0;
    color1 = shapes[indexOfSelected]->color1;
                
    if (shapes[indexOfSelected]->type() == TRIANGLE) {
      color2 = ((Triangle *)shapes[indexOfSelected])->color2;
    }
  }

  return;
}

void Canvas::
toggleSelectedAntialiased()
{
  if (indexOfSelected != -1) {
    shapes[indexOfSelected]->isAntialiased = !(shapes[indexOfSelected]->isAntialiased);
  }

  return;
}

void Canvas::
setSelectedVertex(char v)
{
  selectedVertex = v;

  return;
}

void Canvas::
deleteSelected() 
{
  if (indexOfSelected == -1) {
    return;
  }

  Line *l = shapes[indexOfSelected];
        
  shapes.erase(shapes.begin() + indexOfSelected);
        
  delete l;
        
  if (shapes.size() == 0) {
    setSelected(-1);
  } else {
    setSelected(shapes.size() - 1);
  }
        
  return;
}

void Canvas::
nudge(int direction)
{
  if (indexOfSelected == -1) {
    return;
  }

  XVec2f *vertex = &(shapes[indexOfSelected]->vertex0);

  if (selectedVertex == 1) {
    vertex = &(shapes[indexOfSelected]->vertex1);
  } else if (selectedVertex == 2) {
    vertex = &(((Triangle*)(shapes[indexOfSelected]))->vertex2);
  }

  if (direction == LEFT) {
    vertex->x() -= 1;
  } else if (direction == RIGHT) {
    vertex->x() += 1;
  } else if (direction == DOWN) {
    vertex->y() -= 1;
  } else if (direction == UP) {
    vertex->y() += 1;
  }

  return;
}

void Canvas::
bringForward()
{
        
  if (indexOfSelected == -1) {
    return;
  }
        
  if (indexOfSelected == (int)(shapes.size() - 1)) {
    return;
  }
        
  Line *l = shapes[indexOfSelected];
        
  shapes.erase(shapes.begin() + indexOfSelected);
  indexOfSelected++;
  shapes.insert(shapes.begin() + indexOfSelected, l);
        
  setSelected(indexOfSelected);

  return;
}

void Canvas::
bringToFront()
{
  if (indexOfSelected == -1) {
    return;
  }
        
  if (indexOfSelected == (int)(shapes.size() - 1)) {
    return;
  }
        
  Line *l = shapes[indexOfSelected];
        
  shapes.erase(shapes.begin() + indexOfSelected);
  shapes.push_back(l);
        
  setSelected(shapes.size() - 1);

  return;
}

void Canvas::
sendBackward()
{
  if (indexOfSelected == -1) {
    return;
  }
        
  if (indexOfSelected == 0) {
    return;
  }
        
  Line *l = shapes[indexOfSelected];
        
  shapes.erase(shapes.begin() + indexOfSelected);
  indexOfSelected--;
  shapes.insert(shapes.begin() + indexOfSelected, l);
        
  setSelected(indexOfSelected);

  return;
}

void Canvas::
sendToBack()
{
  if (indexOfSelected == -1) {
    return;
  }
        
  if (indexOfSelected == 0) {
    return;
  }
        
  Line *t = shapes[indexOfSelected];
        
  shapes.erase(shapes.begin() + indexOfSelected);
  shapes.insert(shapes.begin(), t);
        
  setSelected(0);

  return;
}

void Canvas::
newScene()
{
  int count = shapes.size();
  for (int i = 0; i < count; i++)
    delete shapes[i];
        
  shapes.clear();
  setSelected(-1);

  return;
}

void Canvas::
saveScene(string location)
{
  ofstream file;
  file.open(location.c_str(), ofstream::out);
        
  if (!file.is_open()) {
    cout << "Unable to open file " << location << "." << endl;
    return;
  }
        
  int count = shapes.size();
  for (int i = 0; i < count; i++) {
    if (shapes[i]->type() == TRIANGLE) {
      file << 't';
    } else {
      file << 'l';
    }
                
    if (shapes[i]->isAntialiased == true) {
      file << 'a';
    } else {
      file << '_';
    }
                
    file
      << (unsigned char)(255*shapes[i]->color0.red()) << " "
      << (unsigned char)(255*shapes[i]->color0.green()) << " "
      << (unsigned char)(255*shapes[i]->color0.blue()) << " "
      << (unsigned char)(255*shapes[i]->color0.alpha()) << " ";
                
    file
      << (int)shapes[i]->vertex0.x() << " "
      << (int)shapes[i]->vertex0.y() << " ";
                
    file
      << (unsigned char)(255*shapes[i]->color1.red()) << " "
      << (unsigned char)(255*shapes[i]->color1.green()) << " "
      << (unsigned char)(255*shapes[i]->color1.blue()) << " "
      << (unsigned char)(255*shapes[i]->color1.alpha()) << " ";
                
    file
      << (int)shapes[i]->vertex1.x() << " "
      << (int)shapes[i]->vertex1.y();
                
    if (shapes[i]->type() == TRIANGLE) {
      file  << " "
            << (unsigned char)(255*((Triangle *)shapes[i])->color2.red()) << " "
            << (unsigned char)(255*((Triangle *)shapes[i])->color2.green()) << " "
            << (unsigned char)(255*((Triangle *)shapes[i])->color2.blue()) << " "
            << (unsigned char)(255*((Triangle *)shapes[i])->color2.alpha()) << " ";
                
      file
        << (int)((Triangle *)shapes[i])->vertex2.x() << " "
        << (int)((Triangle *)shapes[i])->vertex2.y();
    }
    if (i != count - 1) {
      file << " ";
    }
  }
        
  cout << "Scene saved to " << location << "." << endl;
        
  file.close();

  return;
}

bool Canvas::
openScene(string location)
{
  ifstream file;
  file.open(location.c_str(), ifstream::in);
        
  if (!file.is_open()) {
    cout << "Unable to open file " << location << "." << endl;
    return false;
  }
        
  newScene();
        
  unsigned char c, r, g, b, a;
  int x, y;
        
  while (file) {
                
    Line *newShape;
                
    file >> c;
    bool isTriangle = false;

    if (c == 't') {
      newShape = new Triangle;
      isTriangle = true;
    } else {
      newShape = new Line;
    }
                
    file >> c;

    if (c == 'a') {
      newShape->isAntialiased = true;
    } else {
      newShape->isAntialiased = false;
    }
                
    int numVertices = 2;
    if (isTriangle) {
      numVertices++;
    }
                
    for (int i = 0; i < numVertices; i++) {
      file >> r;
      file >> g;
      file >> b;
      file >> a;
        
      if (i == 0) {
        newShape->color0 = XVec4f(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
      } else if (i == 1) {
        newShape->color1 = XVec4f(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
      } else {
        ((Triangle *)newShape)->color2 = XVec4f(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
      }

      file >> x;
      file >> y;

      if (i == 0) {
        newShape->vertex0 = XVec2f(x,y);
      } else if (i == 1) {
        newShape->vertex1 = XVec2f(x,y);
      } else {
        ((Triangle *)newShape)->vertex2 = XVec2f(x,y);
      }
    }
                
    shapes.push_back(newShape);
  }
  file.close();

  setSelected(shapes.size()-1);
        
  cout << "Scene " << location << " successfully opened." << endl;
        
  return true;
}

void Canvas::
cancelDrawing()
{
  /* clear mouse data */
  firstMouse = XVec2f(-1,-1);
  secondMouse = XVec2f(-1,-1);
  tempSecondMouse = XVec2f(-1,-1);
  tempThirdMouse = XVec2f(-1,-1);
        
  isDrawingClipArea = false;

  return;
}

void Canvas::
setHardwareRender(bool isOn)
{
  isHardwareRender = isOn;

  return;
}

void Canvas::
startDrawingClipView()
{
  isDrawingClipArea = true;
  isDrawingClipped = true;

  return;
}

void Canvas::
toggleIsDrawingClipped()
{
  isDrawingClipped = !isDrawingClipped;

  return;
}

XVec4f Canvas::
currentColor()
{
  if (indexOfSelected == -1) {
    return XVec4f(1.0,0.0,0.0,1.0);
  }
        
  if (selectedVertex == 0) {
    return shapes[indexOfSelected]->color0;
  } else if (selectedVertex == 1) {
    return shapes[indexOfSelected]->color1;
  } else {
    return ((Triangle *)shapes[indexOfSelected])->color2;
  }
}

void Canvas::
setCurrentColor(XVec4f &v)
{
  if (indexOfSelected == -1) {
    return;
  }
        
  if (selectedVertex == 0) {
    shapes[indexOfSelected]->color0 = v;
  } else if (selectedVertex == 1) {
    shapes[indexOfSelected]->color1 = v;
  } else {
    ((Triangle *)shapes[indexOfSelected])->color2 = v;
  }

  return;
}

Line* Canvas::
selected()
{
  if (indexOfSelected == -1) {
    return NULL;
  }
        
  return shapes[indexOfSelected];
}

XVec4f Canvas::
clippingView()
{
  return clipView;
}

XVec2i Canvas::
offset()
{
  return XVec2i((width % GRID_SIZE)/2 + 10, (height % GRID_SIZE)/2 + 10);
}

XVec4f Canvas::
canvasRect()
{
  if (isDrawingClipped) {
    return clipView;
  } else {
    return XVec4f(0.0, 0.0, (float)canvasWidth, (float)canvasHeight);
  }
}
