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
#ifndef __MATMODVIEW__
#define __MATMODVIEW__

#include "xmat.h"

class MatModView:public XMat4f {
 public:
  virtual ~MatModView() {};

  void translate(float tx, float ty, float tz);
  void scale(float sx, float sy, float sz);
  void Xshearby(float hxy, float hxz);
  void Yshearby(float hyx, float hyz);
  void Zshearby(float hzx, float hzy);
  void rotateX(float theta);  // theta in radian
  void rotateY(float theta);  // theta in radian
  void rotateZ(float theta);  // theta in radian
};

extern MatModView CTM;  // current transformation matrix

#endif // __MATMODVIEW__
