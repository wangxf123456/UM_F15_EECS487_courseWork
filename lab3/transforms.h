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
#ifndef __TRANSFORMS__
#define __TRANSFORMS__

typedef enum { TRANSLATION=0, ROTATION, MAGNIFICATION, SHEAR } transform_t;
extern transform_t mode;

extern void rotate(unsigned char key);
extern void translate(unsigned char key);
extern void scale(unsigned char key);
extern void shear(unsigned char key);

#endif //__TRANSFORMS__