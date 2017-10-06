/*
 * Copyright (c) 2010, 2011, 2012, 2015 University of Michigan, Ann Arbor.
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

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <vector>
using namespace std;

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#else
#include <GL/glut.h>
#endif

#include "rasterizer.h"

void drawPoint(XVec2f &point, XVec4f &pointColor);

void Line::draw_with_mode(XVec2f point, double h) {
  XVec4f color;
  XVec4f color_anti;
  XVec2f point_anti = point;
  double alpha = 1;
  int is_above = 1;
  if ((h < 0 && h > -0.5)) {
    alpha = 0.5 + fabs(h);
    is_above = -1;
  } else if (h > 0.5) {
    alpha = 1.5 - h;
    is_above = -1;
  } else if (h >= 0 && h <= 0.5) {
    alpha = 0.5 + h;
  } else if (h <= -0.5) {
    alpha = 1.5 - fabs(h);
  } else if (fabs(h) == 0.5) {
    alpha = 1;
    is_above = 0;
  }

  // convert to original color
  XVec2f r = XVec2f(point.x() - im_v0.x(), point.y() - im_v0.y());
  XVec2f e = XVec2f(im_v1.x() - im_v0.x(), im_v1.y() - im_v0.y());
  double t = r.dot(e) / pow(e.norm(), 2);
  switch(mode) {
    case 0:
    case 1:    
    case 2:
    case 3: {
      color = color_anti = im_c1 * t + im_c0 * (1 - t);
      if (isAntialiased) {
        color_anti.w() = 1 - alpha;
        color.w() = alpha;
      }
      break;
    }
    case 4:
    case 5: 
    case 6:
    case 7: {
      color = color_anti = im_c0 * t + im_c1 * (1 - t);
      if (isAntialiased) {
        color_anti.w() = 1 - alpha;
        color.w() = alpha;
      }
      break;
    }
    default: {
      break;
    }    
  }

  // convert back to original coordinate
  switch(mode) {
    case 0: {
      if (is_above != 0) {
        if (is_above == 1) {
          point_anti.y() += 1;
        } else if (is_above == -1){
          point_anti.y() -= 1;
        }
      }      
      break;
    }
    case 1: {
      swap(point.x(), point.y());
      point_anti = point;
      if (is_above != 0) {
        if (is_above == 1) {
          point_anti.x() += 1;
        } else if (is_above == -1){
          point_anti.x() -= 1;
        } 
      }
      break;      
    }
    case 2: {
      point.y() = 2 * im_v0.y() - point.y();
      point_anti = point;
      if (is_above != 0) {
        if (is_above == 1) {
          point_anti.y() -= 1;
        } else if (is_above == -1){
          point_anti.y() += 1;
        } 
      }
      break;
    }
    case 3: {
      swap(point.x(), point.y());
      point.y() = 2 * im_v0.x() - point.y(); 
      point_anti = point;
      if (is_above != 0) {
        if (is_above == 1) {
          point_anti.x() += 1;
        } else if (is_above == -1){
          point_anti.x() -= 1;
        } 
      }
      break;
    }
    case 4: {
      if (is_above != 0) {
        if (is_above == 1) {
          point_anti.y() += 1;
        } else if (is_above == -1){
          point_anti.y() -= 1;
        } 
      }
      break;
    }
    case 5: {
      swap(point.x(), point.y());
      point_anti = point;
      if (is_above != 0) {
        if (is_above == 1) {
          point_anti.x() += 1;
        } else if (is_above == -1){
          point_anti.x() -= 1;
        } 
      }
      break;      
    }
    case 6: {
      point.y() = 2 * im_v0.y() - point.y();
      point_anti = point;
      if (is_above != 0) {
        if (is_above == 1) {
          point_anti.y() -= 1;
        } else if (is_above == -1){
          point_anti.y() += 1;
        } 
      }
      break;
    }
    case 7: {
      swap(point.x(), point.y());
      point.y() = 2 * im_v0.x() - point.y(); 
      point_anti = point; 
      if (is_above != 0) {
        if (is_above == 1) {
          point_anti.x() += 1;
        } else if (is_above == -1){
          point_anti.x() -= 1;
        } 
      }
      break;
    }
    default: {
      break;
    }
  }

  // check whether this point is between two clipped vertex
  r = XVec2f(point.x() - clip_v0.x(), point.y() - clip_v0.y());
  e = XVec2f(clip_v1.x() - clip_v0.x(), clip_v1.y() - clip_v0.y());
  t = r.dot(e) / pow(e.norm(), 2);
  if (t >= 0.0 && t <= 1.0001) {
    drawPoint(point, color); 
    if (isAntialiased) {
      drawPoint(point_anti, color_anti);
    }
  }
}

void Line::make_basic()
{
  im_v0 = vertex0;
  im_v1 = vertex1;
  im_c0 = color0;
  im_c1 = color1;
  mode = 0;

  if (im_v0.x() > im_v1.x()) {
    mode = 4;
    swap(im_v0, im_v1);
  }

  // line direction downward
  if (im_v1.y() < im_v0.y()) {
    mode += 2;
    im_v1.y() = 2 * im_v0.y() - im_v1.y();
  }

  // slope > 1
  if (im_v1.x() == im_v0.x() || (im_v1.y() - im_v0.y()) / (im_v1.x() - im_v0.x()) > 1) {
    mode += 1;
    swap(im_v0.x(), im_v0.y());
    swap(im_v1.x(), im_v1.y());
  }
}

void Line::get_line_func() {
  A = im_v0.y() - im_v1.y();
  B = im_v1.x() - im_v0.x();
  C = im_v0.x() * im_v1.y() - im_v1.x() * im_v0.y();
}

double Line::calculate(XVec2f &point) {
  return A * point.x() + B * point.y() + C;
}

Line::
Line()
{
  vertex0 = XVec2f(-1,-1);
  vertex1 = XVec2f(-1,-1);

  color0 = XVec4f(1,0,0,1);
  color1 = XVec4f(0,1,0,1);

  isAntialiased = false;
  mode = 0;

  return;
}

/*
 * Line::drawInRect(XVec4f &clipWin)
 * Pre: assumes all class members have been initialized with valid values.
 *      clipWin contains the clipping window
 */
void Line::
drawInRect(XVec4f &clipWin)
{
  /* This function draws the line from vertex0 to vertex1 with endpoints
     colored color0 and color1 respectively. It should call drawPoint() 
     to set each pixel. */
  
  /* The color should be interpolated across the line, it should be
     one (virtual) pixel thick, and clipped against the given rect. */
  
  /* YOUR CODE HERE */

  clip_v0 = im_v0 = vertex0;
  clip_v1 = im_v1 = vertex1;

  // Cohen-Sutherland algorithm implementation
  int is_out_xmin = vertex0.x() < clipWin.x();
  int is_out_xmax = vertex0.x() > clipWin.x() + clipWin.z();
  int is_out_ymin = vertex0.y() < clipWin.y();
  int is_out_ymax = vertex0.y() > clipWin.y() + clipWin.w();
  int l0 = (is_out_ymax << 3) + (is_out_ymin << 2) + (is_out_xmax << 1) + is_out_xmin;
  is_out_xmin = vertex1.x() < clipWin.x();
  is_out_xmax = vertex1.x() > clipWin.x() + clipWin.z();
  is_out_ymin = vertex1.y() < clipWin.y();
  is_out_ymax = vertex1.y() > clipWin.y() + clipWin.w();
  int l1 = (is_out_ymax << 3) + (is_out_ymin << 2) + (is_out_xmax << 1) + is_out_xmin;

  // both points are outside clip window
  if ((l0 & l1) != 0) {
    return;
  }

  // need clipping
  if ((l0 | l1) != 0) {
    XVec2f normal_vectors[4] = { XVec2f(-1, 0), XVec2f(1, 0), XVec2f(0, -1), XVec2f(0, 1) };
    XVec2f points_on_edge[4] = { XVec2f(clipWin.x(), 0), XVec2f(clipWin.x() + clipWin.z(), 0),
                                 XVec2f(0, clipWin.y()), XVec2f(0, clipWin.y() + clipWin.w()) };
    XVec2f u = vertex1 - vertex0;
    double pl[2] = {0};
    double pe[2] = {0};
    int index_l = 0, index_e = 0;
    bool is_parallel = false;
    double tl = 0, te = 0;
    for (int i = 0; i < 4; i++) {
      double temp = normal_vectors[i].dot(u);
      if (temp < 0) {
        pe[index_e++] = normal_vectors[i].dot(points_on_edge[i] - vertex0) / temp;
      } else if (temp > 0) {
        pl[index_l++] = normal_vectors[i].dot(points_on_edge[i] - vertex0) / temp;
      } else { // handle parallel cases
        if (i < 2) { // parallel with y axis
          if (vertex0.y() > vertex1.y()) {
            clip_v0.y() = clipWin.y() + clipWin.w();
            clip_v1.y() = clipWin.y();
            te = (clip_v0.y() - vertex0.y()) / double(vertex1.y() - vertex0.y());
            tl = (clip_v1.y() - vertex0.y()) / double(vertex1.y() - vertex0.y());
          } else {
            clip_v0.y() = clipWin.y();
            clip_v1.y() = clipWin.y() + clipWin.w();  
            te = (clip_v1.y() - vertex0.y()) / double(vertex1.y() - vertex0.y());
            tl = (clip_v0.y() - vertex0.y()) / double(vertex1.y() - vertex0.y());         
          }
        } else { // parallel with x axis
          if (vertex0.x() > vertex1.x()) {
            clip_v0.x() = clipWin.x() + clipWin.z();
            clip_v1.x() = clipWin.x();
            te = (clip_v0.x() - vertex0.x()) / double(vertex1.x() - vertex0.x());
            tl = (clip_v1.x() - vertex0.x()) / double(vertex1.x() - vertex0.x());
          } else {
            clip_v0.x() = clipWin.x();
            clip_v1.x() = clipWin.x() + clipWin.z(); 
            te = (clip_v1.x() - vertex0.x()) / double(vertex1.x() - vertex0.x());
            tl = (clip_v0.x() - vertex0.x()) / double(vertex1.x() - vertex0.x());          
          }
        }
        is_parallel = true;
        break;
      }
    }

    if (!is_parallel) {
      tl = min(pl[0], pl[1]);
      te = max(pe[0], pe[1]);
      if (tl < te) { // leaving before entering
        return;
      } else {
        tl = min(1.0, tl);
        te = max(0.0, te);
        clip_v0 = vertex0 + te * (vertex1 - vertex0);
        clip_v1 = vertex0 + tl * (vertex1 - vertex0);
      }
    }
  }

  // convert to base cases
  make_basic();

  // calculate A, B, C
  get_line_func();

  // mid point algorithm implementation
  int y = im_v0.y();
  int dx = im_v1.x() - im_v0.x();
  int dy = im_v1.y() - im_v0.y();
  XVec2f temp_point = XVec2f(im_v0.x() + 1, im_v0.y() + 0.5);
  double fmid = calculate(temp_point);
  double last_fmid = dx * 0.5;
  for (int x = im_v0.x(); x <= im_v1.x(); x++) {
    temp_point = XVec2f(x, y);
    draw_with_mode(temp_point, last_fmid / dx); // h = last_fmid / dx
    last_fmid = fmid;
    if (fmid < 0) {
      y++;
      fmid += dx - dy;
    } else {
      fmid -= dy;
    }
  }
  return;
}

Triangle::
Triangle()
{
  Line();
  
  vertex2 = XVec2f(-1,-1);
  color2 = XVec4f(0,0,1,1);

  return;
}

void Triangle::get_bound() 
// get the boundary of triangle
{
  xmin = vertex0.x();
  if (xmin > min(vertex1.x(), vertex2.x())) {
    xmin = min(vertex1.x(), vertex2.x());
  }
  xmax = vertex0.x();
  if (xmax < max(vertex1.x(), vertex2.x())) {
    xmax = max(vertex1.x(), vertex2.x());
  }
  ymin = vertex0.y();
  if (ymin > min(vertex1.y(), vertex2.y())) {
    ymin = min(vertex1.y(), vertex2.y());
  }
  ymax = vertex0.y();
  if (ymax < max(vertex1.y(), vertex2.y())) {
    ymax = max(vertex1.y(), vertex2.y());
  }
}

void Triangle::get_line_func()
// calculate the three line functions of triangle
{
  l0 = Line_eqn(vertex0, vertex1);
  l1 = Line_eqn(vertex1, vertex2);
  l2 = Line_eqn(vertex2, vertex0);
  if (l0.calculate(vertex2) < 0) {
    l0.flip();
  }
  if (l1.calculate(vertex0) < 0) {
    l1.flip();
  }
  if (l2.calculate(vertex1) < 0) {
    l2.flip();
  }
}

double Triangle::get_area(XVec2f v0, XVec2f v1, XVec2f v2)
// calculate the area of triangle given 3 points
{
  double ux = v0.x() - v1.x();
  double uy = v0.y() - v1.y();
  double vx = v0.x() - v2.x();
  double vy = v0.y() - v2.y();
  return fabs(ux * vy - uy * vx) / 2;
}

bool Triangle::
containsPoint(XVec2f &point, XVec4f &pointColor)
{
  /* This function returns whether the given point is inside the given
     triangle. If it is, then pointColor is set to  the color of the point
     inside the triangle by interpolating the colors of the vertices. */
  
  // set the point's color to gray for now.
  // replace the following line with your code.
  pointColor = XVec4f(0.5,0.5,0.5,1.0);
  /* YOUR CODE HERE */

  // barycentric coordinate calculation
  if (l0.calculate(point) >= 0 && l1.calculate(point) >= 0 && l2.calculate(point) >= 0) {
    double area0 = get_area(point, vertex0, vertex1);
    double area1 = get_area(point, vertex1, vertex2);
    double area2 = get_area(point, vertex2, vertex0);
    double gamma = area0 / area;
    double alpha = area1 / area;
    double beta = area2 / area;
    pointColor = alpha * color0 + beta * color1 + gamma * color2;
    return true;
  }
  return false;
}

void Triangle::
drawInRect(XVec4f &clipWin)
{
  /* This function draws the triangle t. It must call drawPoint()
     to set each pixel. */
  
  /* YOUR CODE HERE */
  get_bound();
  get_line_func();

  area = get_area(vertex0, vertex1, vertex2);
  for (int x = xmin; x <= xmax; x++) {
    for (int y = ymin; y <= ymax; y++) {
      XVec2f point = XVec2f(x, y);
      XVec4f color;
      if (xmin != xmax && ymin != ymax) {
        if (!isAntialiased) {
          if (containsPoint(point, color)) {
            drawPoint(point, color);
          }         
        } else {
          if (point == vertex0 || point == vertex1 || point == vertex2) {
            continue;
          }
          XVec2f sample_points[9] = { XVec2f(-0.25, 0.25), XVec2f(0.25, 0.25), XVec2f(0.25, 0.25), 
                                      XVec2f(-0.25, 0), XVec2f(0, 0), XVec2f(0.25, 0), 
                                      XVec2f(-0.25, -0.25), XVec2f(0, -0.25), XVec2f(0.25, -0.25) };
          XVec4f result = XVec4f(0, 0, 0, 0);
          for (int i = 0; i < 9; i++) {
            XVec2f temp_point = point + sample_points[i];
            if (containsPoint(temp_point, color)) {
              result += color;
            }   
          }
          result /= 9.0;
          drawPoint(point, result);
        }
      }
    }
  }

  return;
}
