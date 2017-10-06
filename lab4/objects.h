/*
 * Copyright (c) 2011, 2012 University of Michigan, Ann Arbor.
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

#include "viewing.h"

extern XVec4f cube_array0[5][5][8];
extern XVec4f cube_array[5][5][8];
extern XVec4f pyramid0[5];
extern XVec4f pyramid[5];

extern void initCubeArray(void);
extern void initPyramid(void);
extern void drawCubeArray(void);
extern void drawPyramid(void);

extern void drawWorld(transform_t mode, int w, int h);
extern void resetWorld(transform_t mode);
extern void transformWorld(transform_t mode);

#endif //__OBJECTS__
