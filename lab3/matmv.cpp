/*
 * Copyright (c) 2011 University of Michigan, Ann Arbor.
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
 * Author: Sugih Jamin
*/
#include "matmv.h"

/*
 * YOUR goal here is to set up the current transformation matrix
 * ((*this)) with the appropriate matrix to perform the transformation
 * you want.
*/
MatModView CTM;  // current transformation matrix
    
void MatModView::
translate(float tx, float ty, float tz)
{
  /* YOUR CODE HERE */
  XMat4f mat_r = XMat4f();
  mat_r(0, 3) = tx;
  mat_r(1, 3) = ty;
  mat_r(2, 3) = tz;
  CTM *= mat_r;
  return;
}
  
void MatModView::
scale(float sx, float sy, float sz)
{
  /* YOUR CODE HERE */
  XMat4f mat_r = XMat4f();
  mat_r(0, 0) = sx;
  mat_r(1, 1) = sy;
  mat_r(2, 2) = sz;
  CTM *= mat_r;
  return;
}

void MatModView::
Xshearby(float hxy, float hxz)
{
  /* YOUR CODE HERE */
  XMat4f mat_r = XMat4f();
  mat_r(0, 1) = hxy;
  mat_r(0, 2) = hxz;
  CTM *= mat_r;
  return;
}

void MatModView::
Yshearby(float hyx, float hyz)
{
  /* YOUR CODE HERE */
  XMat4f mat_r = XMat4f();
  mat_r(1, 0) = hyx;
  mat_r(1, 2) = hyz;
  CTM *= mat_r;
  return;
}

void MatModView::
Zshearby(float hzx, float hzy)
{
  /* YOUR CODE HERE */
  XMat4f mat_r = XMat4f();
  mat_r(2, 0) = hzx;
  mat_r(2, 1) = hzy;
  CTM *= mat_r;
  return;
}

void MatModView::
rotateX(float theta)
{
  /* YOUR CODE HERE */
  XMat4f mat_r = XMat4f();
  mat_r(1, 1) = cos(theta);
  mat_r(1, 2) = -sin(theta);
  mat_r(2, 1) = sin(theta);
  mat_r(2, 2) = cos(theta);
  CTM *= mat_r;
  return;
}

void MatModView::
rotateY(float theta)
{
  /* YOUR CODE HERE */  
  XMat4f mat_r = XMat4f();
  mat_r(0, 0) = cos(theta);
  mat_r(0, 2) = -sin(theta);
  mat_r(2, 0) = sin(theta);
  mat_r(2, 2) = cos(theta);
  CTM *= mat_r;
  return;
}

void MatModView::
rotateZ(float theta)
{
  /* YOUR CODE HERE */
  XMat4f mat_r = XMat4f();
  mat_r(0, 0) = cos(theta);
  mat_r(0, 1) = -sin(theta);
  mat_r(1, 0) = sin(theta);
  mat_r(1, 1) = cos(theta);
  CTM *= mat_r;
  return;
}
