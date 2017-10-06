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

#include "colorpicker.h"

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#include <iostream>

#define HUE_ORIGIN_X            160
#define HUE_ORIGIN_Y            255
#define HUE_WIDTH                       15
#define HUE_HEIGHT                      105

#define OPACITY_ORIGIN_X        20
#define OPACITY_ORIGIN_Y        375
#define OPACITY_WIDTH           155
#define OPACITY_HEIGHT          15

#define SAT_VALUE_ORIGIN_X      20
#define SAT_VALUE_ORIGIN_Y      255
#define SAT_VALUE_WIDTH         130
#define SAT_VALUE_HEIGHT        105

/* draws a small black and white diamond at point p */
void
drawMarker(XVec2f p)
{
  int size = 5;
        
  glColor4f(1.0,1.0,1.0,1.0);
  glBegin(GL_LINE_LOOP);
  glVertex2f(p.x() + size, p.y());
  glVertex2f(p.x(), p.y() - size);
  glVertex2f(p.x() - size, p.y());
  glVertex2f(p.x(), p.y() + size);
  glEnd();
        
  glColor4f(0.0,0.0,0.0,1.0);
  glBegin(GL_LINE_LOOP);
  glVertex2f(p.x() + size - 1, p.y());
  glVertex2f(p.x(), p.y() - size + 1);
  glVertex2f(p.x() - size + 1, p.y());
  glVertex2f(p.x(), p.y() + size - 1);
  glEnd();

  return;
}

/* converts RGB to HSV, storing in h, s, v */
void
RGBtoHSV(float r, float g, float b, float &h, float &s, float &v)
{
  float componentMin = min(r, min(g, b));
  float componentMax = max(r, max(g, b));
        
  float componentDelta = componentMax - componentMin;
        
  v = componentMax;
        
  if (componentMax == componentMin) {
    h = 0.0;
    s = 0.0;
  } else {
    s = componentDelta/componentMax;
                
    float newR = (componentMax-r)/(6.0f*componentDelta) + 0.5f;
    float newG = (componentMax-g)/(6.0f*componentDelta) + 0.5f;
    float newB = (componentMax-b)/(6.0f*componentDelta) + 0.5f;
                
    if (componentMax == r) {
      h = newB - newG;
    } else if (componentMax == g) {
      h = 1.0f/3 + newR - newB;
    } else {
      h = 2.0f/3 + newG - newR;
    }
                
    if (h < 0.0) {
      h += 1.0;
    }
    if (h > 1.0) {
      h -= 1.0;
    }
  }

  return;
}

/* converts HSV to RGB, storing in r, g, b */
void
HSVtoRGB(float h, float s, float v, float &r, float &g, float &b)
{
  if (s == 0) {
    r = v;
    g = v;
    b = v;
  } else {
    if (h == 1.0) {
      h = 0.0;
    }
    h *= 6.0;
                
    int iH = (int)h;
    float l = v*(1-s);
    float m = v*(1-s*(h-iH));
    float n = v*(1-s*(1-(h-iH)));
                
    if (iH == 0) {
      r = v;
      g = n;
      b = l;
    } else if (iH == 1) {
      r = m;
      g = v;
      b = l;
    } else if (iH == 2) {
      r = l;
      g = v;
      b = n;
    } else if (iH == 3) {
      r = l;
      g = m;
      b = v;
    } else if (iH == 4) {
      r = n;
      g = l;
      b = v;
    } else {
      r = v;
      g = l;
      b = m;
    }
  }

  return;
}

/* a quick, simply way to have GLUT render a c-string */
inline void
renderString(void *font, char *s)
{
  for (; *s != '\0'; s++) {
    glutBitmapCharacter(font, *s);
  }

  return;
}

ColorPicker::
ColorPicker()
{
  /* set the current color to red */
  color = XVec4f(1, 0, 0, 1);

  return;
}

void ColorPicker::
drawNoSelection()
{
  /* draw only where the picker is */
  glViewport(originX, originY, width, height);
        
  /* prepare an orthographic projection such that drawing
     commands directly specify pixel coordinates */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
        
  glOrtho(0, width, 0, height, -1.f, 1.f);
        
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
        
  /* draw a medium gray background */
  glColor4f(0.6f, 0.6f, 0.6f, 1.0f);
  glBegin(GL_QUADS);
  glVertex2f((float)width, 0.0f);
  glVertex2f((float)width, (float)height);        
  glVertex2f(0.0f, (float)height);        
  glVertex2f(0.0f, 0.0f);
  glEnd();
        
  /* draw text */
  glColor4f(0.4f, 0.4f, 0.4f, 1.0f);
  char select[] = "No Selection.";
  glRasterPos2f(5, height - 25);
  renderString(GLUT_BITMAP_HELVETICA_18, select);

  return;
}

void ColorPicker::
draw()
{
  /* draw only where the picker is */
  glViewport(originX, originY, width, height);
        
  /* prepare an orthographic projection such that drawing
     commands directly specify pixel coordinates */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
        
  gluOrtho2D(0, width, 0, height);
        
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /* draw a medium gray background */
  glColor4f(0.6f, 0.6f, 0.6f, 1.0f);
  glBegin(GL_QUADS);
  glVertex2f((float)width, 0.0f);
  glVertex2f((float)width, (float)height);        
  glVertex2f(0.0f, (float)height);        
  glVertex2f(0.0f, 0.0f);
  glEnd();
        
  /* draw the text 'Current Color' at the top of the picker */
  glColor4f(0.4f, 0.4f, 0.4f, 1.0f);
  char currentColor[] = "Current Color:";
  glRasterPos2f(5, height - 25);
  renderString(GLUT_BITMAP_HELVETICA_18, currentColor);
        
  /* draw a rectangle at the top with the selected color */
  glColor4f(color.red(), color.green(), color.blue(), color.alpha());
  glBegin(GL_QUADS);
  glVertex2f((float)width - 5.0f, (float)height - 30.0f);
  glVertex2f((float)width - 5.0f, (float)height - 5.0f);  
  glVertex2f((float)width - 45.0f, (float)height - 5.0f); 
  glVertex2f((float)width - 45.0f, (float)height - 30.0f);
  glEnd();
        
  /* draw the text 'Previous Colors' at the bottom of the picker */
  glColor4f(0.4f, 0.4f, 0.4f, 1.0);
  char historyColor[] = "Previous Colors:";
  glRasterPos2f(5, 220);
  renderString(GLUT_BITMAP_HELVETICA_18, historyColor);
        
  /* draw rectangle at the bottom with all the previous colors */
  int x = 4;
  int y = 6*30+5;
  int count = historyColors.size();
  for (int i = 0; i < count; i++) {
    glColor4f(historyColors[i].red(), historyColors[i].green(), historyColors[i].blue(), historyColors[i].alpha());
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x, y+25);    
    glVertex2i(x+54, y+25); 
    glVertex2i(x+54, y);
    glEnd();
                
    x += 59;
    if (x > 130) {
      x = 4;
      y -= 30;
    }
  }
        
  /* get the current selected color in HSV */
  float hue, saturation, value;
  RGBtoHSV(color.red(), color.green(), color.blue(), hue, saturation, value);
        
  /* get the RGB of that hue */
  float red, green, blue;
  HSVtoRGB(hue, 1.0, 1.0, red, green, blue);
        
  /* get the RGB value of the hue half-saturated and half-light */
  float mRed, mGreen, mBlue;
  HSVtoRGB(hue, 0.5, 0.5, mRed, mGreen, mBlue);
        
  /* draw the word saturation */
  glColor4f(0.0,0.0,0.0,1.0);
  char saturationStr[] = "Saturation";
        
  glRasterPos2f(SAT_VALUE_ORIGIN_X+5, SAT_VALUE_ORIGIN_Y+SAT_VALUE_HEIGHT + 2);
  renderString(GLUT_BITMAP_9_BY_15, saturationStr);
        
  /* draw the word value */
  glColor4f(0.0,0.0,0.0,1.0);
  char valueStr[] = "Value";
  int index = 0;
  for (char *c = valueStr; *c != '\0'; index++,c++) {
    glRasterPos2f(6, SAT_VALUE_ORIGIN_Y+70-11*index);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
  }
        
  /* draw a box that has the hue in the upper right,
     white in the upper left, and black along the bottom.
     The middle value was needed so that the middle of
     the box is not grey */
  glBegin(GL_TRIANGLE_FAN);
  glColor4f(mRed, mGreen, mBlue, 1.0);
  glVertex2f(SAT_VALUE_ORIGIN_X+SAT_VALUE_WIDTH/2.0f, SAT_VALUE_ORIGIN_Y+SAT_VALUE_HEIGHT/2.0f);
  glColor4f(0.0, 0.0, 0.0, 1.0);
  glVertex2f(SAT_VALUE_ORIGIN_X, SAT_VALUE_ORIGIN_Y);
  glColor4f(1.0, 1.0, 1.0, 1.0);
  glVertex2f(SAT_VALUE_ORIGIN_X, SAT_VALUE_ORIGIN_Y+SAT_VALUE_HEIGHT);
  glColor4f(red, green, blue, 1.0);
  glVertex2f(SAT_VALUE_ORIGIN_X+SAT_VALUE_WIDTH, SAT_VALUE_ORIGIN_Y+SAT_VALUE_HEIGHT);
  glColor4f(0.0, 0.0, 0.0, 1.0);
  glVertex2f(SAT_VALUE_ORIGIN_X+SAT_VALUE_WIDTH, SAT_VALUE_ORIGIN_Y);
  glColor4f(0.0, 0.0, 0.0, 1.0);
  glVertex2f(SAT_VALUE_ORIGIN_X, SAT_VALUE_ORIGIN_Y);
  glEnd();
        
  /* draw the word opacity */
  glColor4f(0.0,0.0,0.0,1.0);
  char opacity[] = "Opacity";
  glRasterPos2f(OPACITY_ORIGIN_X+5, OPACITY_ORIGIN_Y+OPACITY_HEIGHT+2);
  renderString(GLUT_BITMAP_9_BY_15, opacity);
        
  /* draw a horiz rect with the alpha going from 0 to 1 */
  glBegin(GL_QUADS);
  glColor4f(red, green, blue, 0.0);
  glVertex2f(OPACITY_ORIGIN_X, OPACITY_ORIGIN_Y+OPACITY_HEIGHT);
  glVertex2f(OPACITY_ORIGIN_X, OPACITY_ORIGIN_Y);
  glColor4f(red, green, blue, 1.0);
  glVertex2f(OPACITY_ORIGIN_X+OPACITY_WIDTH, OPACITY_ORIGIN_Y);
  glVertex2f(OPACITY_ORIGIN_X+OPACITY_WIDTH, OPACITY_ORIGIN_Y+OPACITY_HEIGHT);
  glEnd();
        
  /* draw the word hue */
  glColor4f(0.0,0.0,0.0,1.0);
  char hueStr[] = "Hue";
  glRasterPos2f(HUE_ORIGIN_X - 6, HUE_ORIGIN_Y + HUE_HEIGHT + 2);
  renderString(GLUT_BITMAP_9_BY_15, hueStr);
        
  /* draw a vertical rect with the hue 0 at the bottom and
     1 at the top. Many small boxes are drawn since the hue
     changes drastically over one cycle */
  int yCoord = HUE_ORIGIN_Y;
  HSVtoRGB(0.0, 1.0, 1.0, red, green, blue);
  glBegin(GL_QUADS);
  for (; yCoord < HUE_ORIGIN_Y+HUE_HEIGHT; yCoord += 5) {
    glColor4f(red, green, blue, 1.0);
    glVertex2i(HUE_ORIGIN_X+HUE_WIDTH, yCoord);
    glVertex2i(HUE_ORIGIN_X, yCoord);
                
    HSVtoRGB((yCoord + 5.0f - HUE_ORIGIN_Y)/HUE_HEIGHT, 1.0f, 1.0f, red, green, blue);
    glColor4f(red, green, blue, 1.0);
    glVertex2i(HUE_ORIGIN_X, yCoord+5);
    glVertex2i(HUE_ORIGIN_X+HUE_WIDTH, yCoord+5);
  }
  glEnd();
        
  /* draw markers on the previously drawn pieces to denote
     hue, saturation, value, and alpha */
  drawMarker(XVec2f(155*color.alpha() + 20, 382));
  drawMarker(XVec2f(168, 105*hue + 255));
        
  drawMarker(XVec2f(130*saturation + 20, 105*value + 255));

  return;
}

void ColorPicker::
resize(int w, int h, int x, int y)
{
  /* store the new size and origin */
  width = w;
  height = h;
  originX = x;
  originY = y;

  return;
}

XVec4f& ColorPicker::
currentColor()
{
  return color;
}

void ColorPicker::
setPreviousColors(XVec4f &a, XVec4f &b, XVec4f &c)
{
  /* check if a,b,c are in the previous colors, if any is not, add it */
  bool foundA = false, foundB = false, foundC = false;
        
  int count = historyColors.size();
  for (int i = 0; i < count; i++) {
    if (historyColors[i] == a) {
      foundA = true;
    }
    if (historyColors[i] == b) {
      foundB = true;
    }
    if (historyColors[i] == c) {
      foundC = true;
    }
  }
        
  if (!foundA) {
    historyColors.insert(historyColors.begin(), a);
  }
  if (!foundB) {
    historyColors.insert(historyColors.begin(), b);
  }
  if (!foundC) {
    historyColors.insert(historyColors.begin(), c);
  }
        
  /* the panel shows 21 previous colors, if there are more, remove them */
  if (historyColors.size() > 21) {
    historyColors.erase(historyColors.begin() + 21, historyColors.end());
  }

  return;
}

/* find which part of the view a mouse click is in. this is
   so that the user can drag outside. this allows the user to
   move a marker to the end/corner of a component easily */
void ColorPicker::
mousePressedAt(int x, int y)
{
        
  if (x >= SAT_VALUE_ORIGIN_X && x < SAT_VALUE_ORIGIN_X+SAT_VALUE_WIDTH &&
      y >= SAT_VALUE_ORIGIN_Y && y < SAT_VALUE_ORIGIN_Y+SAT_VALUE_HEIGHT) {
    viewClicked = SATURATION_VALUE_VIEW;

  } else if (x >= HUE_ORIGIN_X && x < HUE_ORIGIN_X + HUE_WIDTH &&
           y >= HUE_ORIGIN_Y && y < HUE_ORIGIN_Y + HUE_HEIGHT) {
    viewClicked = HUE_VIEW;

  } else if (x >= OPACITY_ORIGIN_X && x < OPACITY_ORIGIN_X+OPACITY_WIDTH &&
           y >= OPACITY_ORIGIN_Y && y < OPACITY_ORIGIN_Y+OPACITY_HEIGHT) {
    viewClicked = OPACITY_VIEW;

  } else {
    viewClicked = OTHER_VIEW;
  }
        
  /* update sliders */
  mouseReleasedAt(x,y);

  return;
}

void ColorPicker::
mouseReleasedAt(int x, int y)
{

  /* if the click begin in the saturation/value view then
     update the current color with location of the click */
  if (viewClicked == SATURATION_VALUE_VIEW) {
                
    /* get the current color is HSV */
    float hue, saturation, value;
    RGBtoHSV(color.red(), color.green(), color.blue(), hue, saturation, value);
                
    /* get the new saturation */
    if (x < SAT_VALUE_ORIGIN_X) {
      saturation = 0.0;
    } else if (x > SAT_VALUE_ORIGIN_X+SAT_VALUE_WIDTH) {
      saturation = 1.0;
    } else {
      saturation = float(x-SAT_VALUE_ORIGIN_X)/SAT_VALUE_WIDTH;
    }

    /* get the new value */
    if (y < SAT_VALUE_ORIGIN_Y) {
      value = 0.001f;
    } else if (y > SAT_VALUE_ORIGIN_Y+SAT_VALUE_HEIGHT) {
      value = 1.0;
    } else {
      value = float(y-SAT_VALUE_ORIGIN_Y)/SAT_VALUE_HEIGHT;
    }
                
    /* find and store the new RGB */
    float red, green, blue;
    HSVtoRGB(hue, saturation, value, red, green, blue);
                
    color = XVec4f(red, green, blue, color.alpha());
                
  } else if (viewClicked == HUE_VIEW) {
  /* if the click begin in the hue view then update
     the current color with location of the click */
                
    /* get the current color is HSV */
    float hue, saturation, value;
    RGBtoHSV(color.red(), color.green(), color.blue(), hue, saturation, value);
                
    /* get the new hue */
    if (y < HUE_ORIGIN_Y) {
      hue = 0.0f;
    } else if (y > HUE_ORIGIN_Y + HUE_HEIGHT) {
      hue = 0.999f;
    } else {
      hue =(y-HUE_ORIGIN_Y)/(HUE_HEIGHT + 0.001f);
      /* a hue of 0 is the same as a hue of 1, so
         the 0.001f in the denominator makes it so hue
         cannot quite become 1.0; the 0.999f above is
         for the same reason. */
    }

    /* find and store the new RGB */
    float red, green, blue;
    HSVtoRGB(hue, saturation, value, red, green, blue);
                
    color = XVec4f(red, green, blue, color.alpha());

  } else if (viewClicked == OPACITY_VIEW) {
    /* if the click begin in the opacity view then update
       the current color with location of the click */

    /* set the new alpha value */
    if (x < OPACITY_ORIGIN_X) {
      color.alpha() = 0.0;
    } else if (x > OPACITY_ORIGIN_X+OPACITY_WIDTH) {
      color.alpha() = 1.0;    
    } else {          
      color.alpha() = float(x-OPACITY_ORIGIN_X)/OPACITY_WIDTH;
    }
  } else {
    /* if the click is in the bottom of the panel,
       then find out which previous color was clicked */
    if (y < 210) {
      int i = x/60 + (6-y/30) * 3;
                        
      if (i >= 0 && i < (int)historyColors.size()) {
        color = historyColors[i];
      }
    }

  }

  return;
}

