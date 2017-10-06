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
  /* YOUR CODE HERE: you might want to consult Example 4: Ivory
   * from the Progammable Shaders lecture slides 
   * Replace this whole function with your own interesting shader.
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
    float d = length(vec3(gl_LightSource[i].position) - vec3(position));
    float NdotL = max(dot(n, l), 0.0);
    float NdotH = max(dot(n, normalize(l + v)), 0.0);
    float pf = pow(NdotH, gl_FrontMaterial.shininess);
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

  // add a series of circles on the base of blinn shader
  float r = 0.001;
  float r2 = pow(position.x, 2) + pow(position.y, 2);
  for (int i = 0; i < 5; i++) { 
    if (r2 <= r * (2 * i + 1) && r2 >= r * 2 * i) {
      result = vec4(1.0, 0.0, 0.0, 1.0);
    } else if (r2 > r * (2 * i + 1) && r2 < (r * 2 * (i + 2))) {
      result = vec4(1.0, 1.0, 1.0, 1.0);
    } 
  }
  
  gl_FragColor.rgb = result.xyz;
  gl_FragColor.a = 1.0;
}
