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
#ifndef __OBJECTS__
#define __OBJECTS__

#include "xvec.h"
#include "xmat.h"
#include "transforms.h"

class Cube {
 public:
  XVec4f center;
  XVec4f vert[8];
  
  Cube();
  virtual ~Cube(){};

  void draw(void);
  void transform(void);
};

#define SLICES 20

class Cone {
 public:
  XVec4f vert[SLICES+3];  // vert[0] is base center, vert[1] is the pole
  float radius;           // base radius
  float height;           // base to pole
  XVec4f center;          // halfway between base and pole

  unsigned int cone[SLICES+2];  // indices of vert[] forming the cone
  unsigned int base[SLICES+2];  // indices of vert[] forming the base

  Cone();
  virtual ~Cone(){};

  void draw(void);
  void transform(void);
};

typedef enum { CUBE=0, CONE } object_t;
extern object_t object;

extern void drawWorld(transform_t mode, int w, int h);
extern void resetWorld(transform_t mode);
extern void setupWorld(transform_t mode);
extern void transformWorld(transform_t mode);

#endif //__OBJECTS__
