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

/*
 * TASK 6: YOUR CODE HERE
 * input interpolated texture coordinates
 * and uniform texture sampler
*/

varying vec2 texcoords;
uniform sampler2D mytexture;

/*
 * TASK 7: YOUR CODE HERE
 * input interpolated tangent
 * and uniform normal map sampler
*/

uniform sampler2D mynormaltexture;

void 
main(void)
{
  vec3 n, h;
  float ndotl;

  /* TASK 7: YOUR CODE HERE 
   *
   * Replace the next line by sampling the normal
   * from the normal map passed in by the application,
   * at coordinates passed in by the vertex shader.
   * Remember to convert the coordinates of normal
   * from [0,1] to [-1,1].
  */
  n = normalize(texture2D(mynormaltexture, texcoords).xyz * 2.0 - 1.0);

  /* TASK 6: YOUR CODE HERE 
   *
   * Replace the following line giving gl_FragColor a constant color
   * with a sample of the texture passed in by the application at
   * the texture coordinates passed in by the vertex shader.
  */

  gl_FragColor = texture2D(mytexture, texcoords);

  /*
   * Blend/modulate the texel with per-fragment Blinn lighting computation.
   * All of normal, light, and view vectors are assumed to be in tangent space.
   */
  ndotl = dot(n, light);
  h = normalize(light+view);

  gl_FragColor.rgb = (ndotl < 0.0 ? 0.0 : 1.0)*gl_FragColor.rgb*
    (gl_LightModel.ambient.rgb+     // global ambient
     gl_LightSource[0].ambient.rgb+ // per-light ambient
     gl_LightSource[0].diffuse.rgb*max(ndotl, 0.0) +
     gl_LightSource[0].specular.rgb*
     pow(max(dot(n, h), 0.0), gl_FrontMaterial.shininess));
  gl_FragColor.a = 1.0;

  /* To help debugging, you can view your normal,
     view, or light vector displayed as gl_FragColor
     and see if they are pointing in the right direction
     (it helps to know what the right direction is supposed
     to be, of course) */
  // DEBUG
  //gl_FragColor.rgb = .5*normalize(tangent)+.5;
}
