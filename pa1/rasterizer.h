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
 * Authors:
 *        Ari Grant, grantaa@umich.edu
 *        Sugih Jamin, jamin@eecs.umich.edu
 */

#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "xvec.h"

#define LINE            0
#define TRIANGLE        1

class Line_eqn {
// used to represent 3 edges in a triangle
  private:
    double A;
    double B;
    double C;

  public:
    Line_eqn() {
      A = 0;
      B = 0;
      C = 0;
    }

    Line_eqn(XVec2f v1, XVec2f v2) {
      A = v1.y() - v2.y();
      B = v2.x() - v1.x();
      C = v1.x() * v2.y() - v2.x() * v1.y();
    }
    void flip() {
      A = -A;
      B = -B;
      C = -C;
    }

    double calculate(XVec2f v) {
      return A * v.x() + B * v.y() + C;
    }

    double calculate(int x, int y) {
      return A * x + B * y + C;
    }
};

class Line {
 public: 
  Line();
  virtual ~Line() {}
  virtual void drawInRect(XVec4f &clipWin);
  virtual char type() { return LINE; }
  
  XVec2f vertex0, vertex1;
  XVec4f color0, color1;

  XVec2f im_v0, im_v1; // the corresponding vertex in base case
  XVec2f clip_v0, clip_v1; // clipped vertex
  XVec4f im_c0, im_c1; // the corresponding color in base case
  double A, B, C;
  int mode; // record which case this line is in before conversion
  bool isAntialiased;

  void make_basic(); // convert to the base case then rasterizing lines
  void get_line_func(); // calculate A, B and C for this line
  void draw_with_mode(XVec2f point, double h); 
  // draw point with correct color. If anti-aliased, also draw the corresponding pixel

  double calculate(XVec2f &point); // return Ax+By+C
};

class Triangle:public Line {    
 public:
  Triangle();
  virtual ~Triangle() {}
  virtual void drawInRect(XVec4f &clipWin);
  virtual char type() { return TRIANGLE; }
  
  bool containsPoint(XVec2f &point, XVec4f &pointColor);
  
  XVec2f vertex2;
  XVec4f color2;
  int init;

  void get_bound(); // find the minimum rectangle which contains this triangle
  void get_line_func(); // get three line functions
  double get_area(XVec2f v0, XVec2f v1, XVec2f v2); // get the traingle area with vertex v0, v1, v2
  Line_eqn l0, l1, l2; // 3 edges in triangle
  int xmin, xmax, ymin, ymax; // four vertex of the minimum rectangle which contains this triangle
  double area; // area of this triangle
};

#endif // RASTERIZER_H
