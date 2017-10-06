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

#define INITRAD 0.5
#define MAGFAC 1.1
#define MINFAC .9 

extern void drawAxes(int w, int h, float alpha);
extern void initWorld();
extern void drawWorld(int w, int h);

typedef enum { TORUS=0, SPHERE } object_t;
extern object_t object;
extern float sphere_radius;

extern bool useGLU;

#endif //__OBJECTS__
