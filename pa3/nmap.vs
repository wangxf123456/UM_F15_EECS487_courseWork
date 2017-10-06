/*
 * Copyright (c) 2012, 2013 University of Michigan, Ann Arbor.
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
 * Authors: Sugih Jamin
 *
 * Per-vertex eye-to-tangent transformation of light and view 
 * vectors with per-fragment tangent-space lighting computation 
 * using normal map
*/
#version 120

varying vec4 position;
varying vec3 normal, view, light;

/* TASK 5:
 * Define vertex attribute variables
 * for position and normal.
 */

attribute vec4 va_Position;
attribute vec3 va_Normal;

/* TASK 6: YOUR CODE HERE
 * You need to input texture coordinates
 * from the application and pass it
 * along to the fragment shader.
*/

attribute vec2 va_TexCoords;
varying vec2 texcoords;

/* TASK 7: YOUR CODE HERE
 * Input tangent and pass tangent-space
 * view and light vectors to fragment shader
*/

attribute vec3 va_Tangent;

void 
main(void) 
{
  /* TASK 5: YOUR CODE HERE
   * Replace the use of gl_Vertex and gl_Normal
   * below with your own vertex attributes.
  */
  position = gl_ModelViewMatrix*va_Position;
  gl_Position = gl_ModelViewProjectionMatrix*va_Position;
  normal = gl_NormalMatrix*va_Normal;
  vec3 T = gl_NormalMatrix*va_Tangent;

  /* TASK 6: YOUR CODE HERE
   * 
   * pass texure coordinates from application
   * to the fragment shader.
  */

  texcoords = va_TexCoords;

  /* TASK 7: YOUR CODE HERE
   *
   * Compute orthonormal B', T', and [T'B'N]
   *
   * Replace the following two lines with your
   * eye and light vectors in tangent space
  */

  normal = normalize(normal);
  vec3 B = normalize(cross(normal, T));
  T = normalize(cross(B, normal));

  mat3 TBN = mat3(T, B, normal);
  view = vec3(0.0, 0.0, 1.0) * TBN;
  light = vec3(0.0, 0.0, 1.0) * TBN;
}
