/*
 * Copyright (c) 2007, 2011 University of Michigan, Ann Arbor.
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
 * Author: Igor Guskov, Sugih Jamin
 *
 */
// Shaders support code. You should be able to complete the assignment
// without looking into this file.  Generally, you should not modify
// this file unless you know what you are doing. Make changes to the shader 
// files (*.vs and *.fs) instead to complete the assignment.

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else 
#include "GL/glew.h"
#include <GL/glut.h>
#endif

using namespace std;

// Reads the shader source code from the file into a string.
static bool
ReadProgram(string& s, const string& name)
{
  ifstream ist(name.c_str());

  if (!ist.good())
    return false;

  s.clear();

  while(ist.good()) {
    string line;
    getline(ist, line);
    s += line;
    s += "\n";
  }

  return true;
}

// Compiles the shader, returning false on errors.
bool
CompileShader(GLuint hso, string& src)
{
  const char* str = src.c_str();
  GLint len = (int)src.length();
  glShaderSource(hso, 1, &str, &len);
  glCompileShader(hso);
  GLint compiled;
  glGetShaderiv(hso, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    cerr << " could not compile shader:" << endl;
    /*
    GLint loglen;
    glGetShaderiv(hso, GL_INFO_LOG_LENGTH, &loglen);
    char *log = malloc(loglen*sizeof(char));
    glGetShaderInfoLog(hso, loglen, NULL, log);
    */
    char log[2048];
    glGetShaderInfoLog(hso, 2047, NULL, log);
    cerr << log << endl;
    //free(log);
    return false;
  }

  return true;
}


// Load shaders vsname.vs and fsname.fs and link them into the program object.
bool
InitShaders(const char* vsname, const char *fsname, GLuint* hpo)
{
  if (!hpo) {
    return false;
  }

  if (*hpo==0) {
    *hpo = glCreateProgram();
  }

  cerr << "Shader:" << endl;
  
  string src;
  if (vsname) {
    GLuint hvso = glCreateShader(GL_VERTEX_SHADER);
    string name = vsname + string(".vs");
    
    cerr << "    vertex: " << vsname;
    
    if (ReadProgram(src, name)) {
      if (CompileShader(hvso, src)) { 
        glAttachShader(*hpo, hvso);
      }
      cerr << endl;
    } else {
      cerr << " could not read vertex shader " << name << endl;
      return false;
    }
  }
  
  if (fsname) {
    GLuint hfso = glCreateShader(GL_FRAGMENT_SHADER);
    string name = fsname + string(".fs");
    
    cerr << "  fragment: " << fsname;
    
    if (ReadProgram(src, name)) {
      if (CompileShader(hfso, src)) {
        glAttachShader(*hpo, hfso);
      }
      cerr << endl;
    } else {
      cerr << " could not read fragment shader " << name << endl;
      return false;
    }
  }
  
  glLinkProgram(*hpo);
  GLint linked;
  glGetProgramiv(*hpo, GL_LINK_STATUS, &linked);
  if (!linked) {
    if (vsname) {
      cerr << " could not link " << vsname << " vertex shader" << endl;
    } if (fsname) {
      cerr << " could not link " << fsname << " fragment shader" << endl;
    }
    return false;
  }
  
#if 0
  glDetachShader(*hpo, hvso);
  glDeleteShader(hvso);
  glDetachShader(*hpo, hfso);
  glDeleteShader(hfso);
#endif
  
  return true;
}

