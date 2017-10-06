/*
 * Copyright (c) 2007, 2011, 2012 University of Michigan, Ann Arbor.
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
 * Authors: Igor Guskov, Sugih Jamin
 *
*/
#version 120

#define MAX_LIGHTS 20
uniform int numlights;

varying vec4 position;
varying vec3 normal;
varying vec3 viewVec;
varying vec3 lightVec[MAX_LIGHTS];

void 
main(void) 
{
  position = gl_ModelViewMatrix * gl_Vertex;
  gl_Position = gl_ProjectionMatrix*position;

  normal = normalize(gl_NormalMatrix * gl_Normal);
  viewVec = -position.xyz;
  for (int i = 0; i < numlights; i++) {
  	lightVec[i] = vec3(gl_LightSource[i].position) - vec3(position);
  }
}
