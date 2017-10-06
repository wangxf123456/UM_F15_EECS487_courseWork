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
 * Authors: Sugih Jamin and Igor Guskov
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
  /* YOUR CODE HERE: you might want to consult Example 4: Ivory
   * from the Progammable Shaders lecture slides 
   * Replace the content of this function with your implementation
   * of per-pixel shading using the original Phong algorithm.  
   * You would want to use the built-in uniform variables
   * gl_FrontMaterial, gl_LightModel, and gl_LightSource[] (see lecture notes).  
   * Be sure to compute all components of the formula, including 
   * emission, global and local ambient, and effect of attenuation.
   *
   * After you get Phong working, make a copy of your phong.fs and
   * name it blinn.fs and make the slight modification to implement
   * the Blinn-Phong variant of per-pixel lighting.
  */
  
  vec3 v = normalize(viewVec);
  vec3 n = normalize(normal);
  vec4 diffuse;
  vec4 ambient;
  vec4 specular;
  vec4 global_ambient;
  vec4 result = vec4(0.0, 0.0, 0.0, 0.0);
  for (int i = 0; i < numlights; i++) {
    vec3 l = normalize(lightVec[i]);
    vec3 r = -l + 2 * dot(n, l) * n; 
    float d = length(vec3(gl_LightSource[i].position) - vec3(position));

    float NdotL = max(dot(n, l), 0.0);
    float RdotV = max(dot(r, v), 0.0);
    float pf = pow(RdotV, gl_FrontMaterial.shininess);
    if (NdotL == 0.0) {
      pf = 0.0;
    }
    diffuse = gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse;
    ambient = gl_FrontMaterial.ambient * gl_LightSource[i].ambient;  
    specular = gl_FrontMaterial.specular * gl_LightSource[i].specular * pf;
    float attenuation;
    attenuation = 1.0 / (gl_LightSource[i].constantAttenuation + 
                      gl_LightSource[i].linearAttenuation * d + 
                      gl_LightSource[i].quadraticAttenuation * d * d);
    result = result + (NdotL * diffuse + ambient + specular) * attenuation;
  }
  result = result + gl_FrontMaterial.emission + gl_FrontMaterial.ambient * gl_LightModel.ambient;
  gl_FragColor.rgb = result.xyz;
  gl_FragColor.a = 1.0;
}
