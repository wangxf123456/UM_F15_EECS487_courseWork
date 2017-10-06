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
 * Authors: Igor Guskov, Sugih Jamin
 *
*/
// X3 Scene graph classes implementation. Look for YOUR CODE HERE sections.
// Classes are given in alphabetical order in this file.

// NOTE: Constructors of node objects take a list of name-attribute string 
// pairs list to get initialized. It is being called by the X3D parser.

#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>

#if defined(_WIN32) && !(defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__))
#define sscanf sscanf_s
#endif
#endif

#include "scene.h"

using namespace std;

// Auxilliary function to eliminate commas in attribute lists.
static inline void erase_char(string& s, char ch) 
{
  for (int i = 0; i < (int)s.length(); ++i) {
    if (s[i]==ch) {
      s[i] = ' ';
    }
  }

  return;
}

// Add method adds children to the nodes (only those allowed unless
// this is a grouping node). For instance, the Appearance node only 
// allows children of Material kind to be added, and everything else
// is skipped.
void
X3Appearance::Add(X3NodeType type, X3Node* node)
{
  if (type==X3NODE_MATERIAL) {
    material_ = reinterpret_cast<X3Material*>(node);
  } else {
    X3Node::Add(type, node);
  }

  return;
}

void
X3Appearance::Print(std::ostream& ost, int offset) const
{
  ost << std::string(offset, ' ') << Name() << std::endl;
  if (material_) {
    material_->Print(ost, offset+1);
  }

  return;
}

X3Box::X3Box(const char **atts) : size_(2.0f, 2.0f, 2.0f)
{
  if (atts==0) {
    return;
  }
  for (const char** ref = atts; *ref!=NULL; ++ref) {
    const char* name = *ref;
    ++ref;
    if (*ref==NULL) {
      break;
    }
    if (strcmp(name, "size")==0) {
      sscanf(*ref, "%f %f %f", &size_(0), &size_(1), &size_(2));
    }
  }

  return;
}

void
X3Box::Render() const
{
  glBegin(GL_QUADS);
  glNormal3f(0.0f, 0.0f, 1.0f);
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));

  glNormal3f(0.0f, 0.0f, -1.0f);
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));

  glNormal3f(1.0f, 0.0f, 0.0f);
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));

  glNormal3f(-1.0f, 0.0f, 0.0f);
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));

  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));

  glNormal3f(0.0f, -1.0f, 0.0f);
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));
  glEnd();

  return;
}


X3Cone::X3Cone(const char** atts) 
  : bottom_(true), side_(true),
    height_(2.0f), bottom_radius_(1.0f) 
{
  if (atts==0) {
    return;
  }
  for (const char** ref = atts; *ref!=NULL; ++ref) {
    const char* name = *ref;
    ++ref;
    if (*ref==NULL) {
      break;
    }
    if (strcmp(name, "height")==0) {
      sscanf(*ref, "%f", &height_);
    } else if (strcmp(name, "bottomRadius")==0) {
      sscanf(*ref, "%f", &bottom_radius_);
    } else if (strcmp(name, "bottom")==0) {
      if (strcmp(*ref, "false")==0) {
        bottom_ = false;
      } else if (strcmp(*ref, "true")==0) {
        bottom_ = true;
      }
    } else if (strcmp(name, "side")==0) {
      if (strcmp(*ref, "false")==0) {
        side_ = false;
      } else if (strcmp(*ref, "true")==0) {
        side_ = true;
      }
    }
  }

  return;
}

void
X3Cone::Render() const
{
  const int N = 10;
  const float step = 2.0f * M_PI / N;
  if (side_) {
    glBegin(GL_QUAD_STRIP);
    for (int k = 0; k < N+1; ++k) {
      glNormal3f(cos(k*step), bottom_radius_ / height_, sin(k*step));
      glVertex3f(bottom_radius_*cos(k*step), -0.5f*height_, bottom_radius_*sin(k*step));
      glVertex3f(0.0f, 0.5f*height_, 0.0f);
    }
    glEnd();
  }
  if (bottom_) {
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, -0.5f*height_, 0.0f);
    for (int k = 0; k < N+1; ++k) {
      glVertex3f(bottom_radius_*cos(k*step), -0.5f*height_, bottom_radius_*sin(k*step));
    }
    glEnd();
  }

  return;
}

X3Coordinate::X3Coordinate(const char** atts)
{
  if (atts==0) {
    return;
  }
  for (const char** ref = atts; *ref!=NULL; ++ref) {
    const char* name = *ref;
    ++ref;
    if (*ref==NULL) {
      break;
    }
    string ref_copy(*ref);
    erase_char(ref_copy, ',');
    if (strcmp(name, "point")==0) {
      point_.clear();
      istringstream ist(ref_copy);
      while(ist.good()) {
        XVec3f xyz;
        ist >> xyz(0) >> xyz(1) >> xyz(2);
        if (!ist.fail()) {
          point_.push_back(xyz);
        }
      }
    }
  }

  return;
}

X3Cylinder::X3Cylinder(const char** atts) 
  : top_(true), bottom_(true), side_(true),
    height_(2.0f), radius_(1.0f) 
{
  if (atts==0) {
    return;
  }
  for (const char** ref = atts; *ref!=NULL; ++ref) {
    const char* name = *ref;
    ++ref;
    if (*ref==NULL) {
      break;
    }
    if (strcmp(name, "height")==0) {
      sscanf(*ref, "%f", &height_);
    } else if (strcmp(name, "radius")==0) {
      sscanf(*ref, "%f", &radius_);
    } else if (strcmp(name, "bottom")==0) {
      if (strcmp(*ref, "false")==0) {
        bottom_ = false;
      } else if (strcmp(*ref, "true")==0) {
        bottom_ = true;
      }
    } else if (strcmp(name, "top")==0) {
      if (strcmp(*ref, "false")==0) {
        top_ = false;
      } else if (strcmp(*ref, "true")==0) {
        top_ = true;
      }
    } else if (strcmp(name, "side")==0) {
      if (strcmp(*ref, "false")==0) {
        side_ = false;
      } else if (strcmp(*ref, "true")==0) {
        side_ = true;
      }
    }
  }

  return;
}

void
X3Cylinder::Render() const
{
  // TASK 1: YOUR CODE HERE: Modify this function to render a cylinder.
  // Make sure to use top_, side_, and bottom_ flags and to compute 
  // and set the normals properly.
  const int N = 10;
  const float step = 2.0f * M_PI / N;
  if (top_) {
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.5f*height_, 0.0f);
    for (int k = N; k >= 0; --k) {
      glVertex3f(radius_*cos(k*step), 0.5f*height_, radius_*sin(k*step));
    }
    glEnd();   
  }
  if (side_) {
    glBegin(GL_QUAD_STRIP);
    for (int k = 0; k < N+1; ++k) {
      glNormal3f(cos(k*step), 0, sin(k*step));
      glVertex3f(radius_*cos(k*step), -0.5f*height_, radius_*sin(k*step));
      glVertex3f(radius_*cos(k*step), 0.5f*height_, radius_*sin(k*step));
    }
    glEnd();
  }
  if (bottom_) {
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, -0.5f*height_, 0.0f);
    for (int k = 0; k < N+1; ++k) {
      glVertex3f(radius_*cos(k*step), -0.5f*height_, radius_*sin(k*step));
    }
    glEnd();
  }
  return;
}

void
X3GroupingNode::Print(std::ostream& ost, int offset) const
{
  ost << std::string(offset, ' ') << "{" << std::endl;
  for (int i = 0; i < (int)children_.size(); ++i) {
    children_[i]->Print(ost, offset+2);
  }
  ost << std::string(offset, ' ') << "}" << std::endl;

  return;
}

void
X3GroupingNode::Render() const
{
  for (int i = 0; i < (int)children_.size(); ++i) {
    children_[i]->Render();
  }

  return;
}

void
X3GroupingNode::SetupLights(int* light_count) const
{
  for (int i = 0; i < (int)children_.size(); ++i) {
    children_[i]->SetupLights(light_count);
  }

  return;
}

X3IndexedFaceSet::X3IndexedFaceSet(const char** atts) 
  : coordinate_(NULL)
{
  if (atts==0) {
    return;
  }
  for (const char** ref = atts; *ref!=NULL; ++ref) {
    const char* name = *ref;
    ++ref;
    if (*ref==NULL) {
      break;
    }
    string ref_copy(*ref);
    erase_char(ref_copy, ',');
    if (strcmp(name, "coordIndex")==0) {
      coord_index_.clear();
      istringstream ist(ref_copy);
      while(ist.good()) {
        int index;
        ist >> index;
        if (!ist.fail()) {
          coord_index_.push_back(index);
        }
      }
    }
  }
  // Now pre-process the list into triangles and quads, ignore everything else.
  int istart = 0;
  for (int i = 0; i <= (int)coord_index_.size(); ++i) {
    if (i==(int)coord_index_.size() || coord_index_[i]==-1) {
      switch(i-istart) {
      case 3:
        triangles_.push_back(XVec3i(coord_index_[istart], 
                                    coord_index_[istart+1], 
                                    coord_index_[istart+2]));
        break;
      case 4:
        quads_.push_back(XVec4i(coord_index_[istart], 
                                coord_index_[istart+1], 
                                coord_index_[istart+2], 
                                coord_index_[istart+3]));
        break;
      }
      istart = i+1;
    }
  }

  return;
}

void
X3IndexedFaceSet::Render() const
{
  if (!coordinate_) {
    return;
  }

  /* TASK 6: YOUR CODE HERE:
   * Modify this function to render sets of triangles 
   * and sets of quads.  For each triangle, draw triples
   * of vertices in a loop within the glBegin(GL_TRIANGLES)
   * rendering mode.  Remember to specify glNormal()
   * before each call to glVertex().
  */

  for (unsigned int i = 0; i < triangles_.size(); i++) {
    glBegin(GL_TRIANGLES);
    for (int k = 0; k < 3; k++) {
      glNormal3f(normals_[triangles_[i](k)](0), normals_[triangles_[i](k)](0), normals_[triangles_[i](k)](0));
      glVertex3f(coordinate_->point(triangles_[i][k])(0), 
                coordinate_->point(triangles_[i][k])(1), 
                coordinate_->point(triangles_[i][k])(2));
    }
    glEnd();       
  }
  for (unsigned int i = 0; i < quads_.size(); i++) {
    glBegin(GL_TRIANGLES);
    for (int k = 0; k < 3; k++) {
      glNormal3f(normals_[quads_[i](k)](0), normals_[quads_[i](k)](0), normals_[quads_[i](k)](0));
      glVertex3f(coordinate_->point(quads_[i][k])(0), 
                coordinate_->point(quads_[i][k])(1), 
                coordinate_->point(quads_[i][k])(2));
    }
    for (int k = 0; k < 4; k++) {
      if (k == 1) {
        continue;
      }
      glNormal3f(normals_[quads_[i](k)](0), normals_[quads_[i](k)](0), normals_[quads_[i](k)](0));
      glVertex3f(coordinate_->point(quads_[i][k])(0), 
                coordinate_->point(quads_[i][k])(1), 
                coordinate_->point(quads_[i][k])(2));
    }
    glEnd();       
  }
  return;
}

void
X3IndexedFaceSet::Add(X3NodeType type, X3Node* node)
{
  if (type==X3NODE_COORDINATE) {
    coordinate_ = reinterpret_cast<X3Coordinate*>(node);
    normals_.clear();
    normals_.resize(coordinate_->size(), XVec3f(0.0f, 0.0f, 0.0f));

    cerr << (int)triangles_.size() << " triangles and "
         << (int)quads_.size() << " quads." << endl;

    /* TASK 6: YOUR CODE HERE:
     * Accumulate normals for every vertex from every triangle
     * and quad it is a part of. Go through every triangle and
     * every quad accumulating normals for their vertices, and
     * then normalize the normal vectors.
     *
     * triangles_, quads_, and normals_ are C++ stdlib vector class.
     * Member function vector::size() returns the number of elements
     * in the vector.
     *
     * The triangles_ list consists of a vector of vertex list.
     * Each vertex list represents the three vertices of a single
     * triangle, in the form of an index into the coordinate list.
     * Thus triangles_[0] contains the three indices into the
     * coordinate list forming triangle 0. These three indices are
     * accessed as triangle_[0](0), triangle_[0](1), and
     * triangle_[0](2).  The coordinates of each vertex themselves
     * are accessed as coordinate_->point(triangle_[0](0)),
     * coordinate_->point(triangle_[0](1)), and
     * coordinate_->point(triangle_[0](2)).
    */
    vector<int> normal_count(coordinate_->size(), 0);
    for (unsigned int i = 0; i < triangles_.size(); i++) {
      XVec3f u = coordinate_->point(triangles_[i][1]) - coordinate_->point(triangles_[i][0]);
      XVec3f v = coordinate_->point(triangles_[i][2]) - coordinate_->point(triangles_[i][0]);
      XVec3f nA = u.cross(v);
      nA.normalize();
      normals_[triangles_[i][0]] += nA;
      normals_[triangles_[i][1]] += nA;
      normals_[triangles_[i][2]] += nA;
      normal_count[triangles_[i][0]] += 1;
      normal_count[triangles_[i][1]] += 1;
      normal_count[triangles_[i][2]] += 1;
    }
    for (unsigned int i = 0; i < quads_.size(); i++) {
      XVec3f u = coordinate_->point(quads_[i][1]) - coordinate_->point(quads_[i][0]);
      XVec3f v = coordinate_->point(quads_[i][2]) - coordinate_->point(quads_[i][0]);
      XVec3f nA = u.cross(v);
      nA.normalize();
      normals_[quads_[i][0]] += nA;
      normals_[quads_[i][1]] += nA;
      normals_[quads_[i][2]] += nA;
      normals_[quads_[i][3]] += nA;
      normal_count[quads_[i][0]] += 1;
      normal_count[quads_[i][1]] += 1;
      normal_count[quads_[i][2]] += 1;
      normal_count[quads_[i][3]] += 1;
    }
    for (unsigned int i = 0; i < normals_.size(); i++) {
      normals_[i] /= (float)normal_count[i];
    }
  } else {
    X3Node::Add(type, node);
  }

  return;
}

X3LightNode::X3LightNode(const char** atts) 
  : ambient_intensity_(0.0f),
    color_(1.0f, 1.0f, 1.0f, 1.0f),
    intensity_(1.0f)
{
  if (atts==0) {
    return;
  }
  for (const char** ref = atts; *ref!=NULL; ++ref) {
    const char* name = *ref;
    ++ref;
    if (*ref==NULL) {
      break;
    }
    if (strcmp(name, "color")==0) {
      sscanf(*ref, "%f %f %f", &color_(0), &color_(1), &color_(2));
    } else if (strcmp(name, "ambientIntensity")==0) {
      sscanf(*ref, "%f", &ambient_intensity_);
    } else if (strcmp(name, "intensity")==0) {
      sscanf(*ref, "%f", &intensity_);
    }
  }

  return;
}

void
X3LightNode::SetupLights(int* light_count) const
{
  glEnable(GL_LIGHT0 + *light_count);
  ++(*light_count);

  return;
}


X3Material::X3Material(const char** atts) 
  : ambient_intensity_(0.2f),
    diffuse_color_(0.8f, 0.8f, 0.8f, 1.0f),
    emissive_color_(0.0f, 0.0f, 0.0f, 1.0f),
    shininess_(0.2f),
    specular_color_(0.0f, 0.0f, 0.0f, 1.0f)
{
  if (atts==0) {
    return;
  }
  for (const char** ref = atts; *ref!=NULL; ++ref) {
    const char* name = *ref;
    ++ref;
    if (*ref==NULL) {
      break;
    }
    if (strcmp(name, "diffuseColor")==0) {
      sscanf(*ref, "%f %f %f", &diffuse_color_(0), &diffuse_color_(1), &diffuse_color_(2));
    } else if (strcmp(name, "specularColor")==0) {
      sscanf(*ref, "%f %f %f", &specular_color_(0), &specular_color_(1), &specular_color_(2));
    } else if (strcmp(name, "emissiveColor")==0) {
      sscanf(*ref, "%f %f %f", &emissive_color_(0), &emissive_color_(1), &emissive_color_(2));
    } else if (strcmp(name, "ambientIntensity")==0) {
      sscanf(*ref, "%f", &ambient_intensity_);
    } else if (strcmp(name, "shininess")==0) {
      sscanf(*ref, "%f", &shininess_);
    }
  }

  return;
}

void
X3Material::Print(std::ostream& ost, int offset) const
{
  ost << std::string(offset, ' ') << Name();
  ost << ": diffuse=( " << diffuse_color_ << ")" 
      << ", specular=( " << specular_color_ << ")"
      << ", emissive=( " << emissive_color_ << ")"
      << ", shininess= " << shininess_ << ""
      << ", ambient= " << ambient_intensity_ << "."
      << std::endl;

  return;
}

void
X3Material::Render() const
{
  /* TASK 5: YOUR CODE HERE:
   * Modify this function to properly setup OpenGL material properties.
   * Again, basically pass along all the material's properties read
   * from the X3D file to OpenGL by calling glMaterial().  Be careful
   * that shininess must be multiplied by 128 for X3D.
   * Also assume materials are two sided and the given properties
   * apply to both sides.
  */

  GLfloat mat_shininess[] = { shininess_ * 128 };
  XVec4f ambient = diffuse_color_ * ambient_intensity_;
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_color_);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_color_);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive_color_);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
  return;
}

X3PointLight::X3PointLight(const char** atts) 
  : X3LightNode(atts),
    attenuation_(1.0f, 0.0f, 0.0f),
    location_(0.0f, 0.0f, 0.0f)
{
  if (atts==0) {
    return;
  }
  for (const char** ref = atts; *ref!=NULL; ++ref) {
    const char* name = *ref;
    ++ref;
    if (*ref==NULL) {
      break;
    }
    if (strcmp(name, "attenuation")==0) {
      sscanf(*ref, "%f %f %f", &attenuation_(0), &attenuation_(1), &attenuation_(2));
    } else if (strcmp(name, "location")==0) {
      sscanf(*ref, "%f %f %f", &location_(0), &location_(1), &location_(2));
    }
  }

  return;
}

void
X3PointLight::SetupLights(int* light_count) const
{
  /* TASK 4: YOUR CODE HERE:
   * Modify this function to properly 
   * setup point light properties as described in section 17.2 of X3D specs.
   * *light_count is a running count of the number of lights in your scene.
   * The current light's GL_LIGHT number is GL_LIGHT0+*light_count.
   * You don't need to increment it, X3LightNode::SetupLights() called
   * at the end of this method does the incrementing (for the next light).
   * You must set up light with id GL_LIGHT0 + *light_count.
   * Basically, pass along all the light's properties read from the X3D file
   * to OpenGL by calling glLight().
  */

  // Keep this call at the end to do proper light counting.
  // It also turns on (glEnable()) all of your lights for you.
   
  XVec4f light_position = XVec4f(location_(0), location_(1), location_(2), 1.0);
  XVec4f ambient_color = ambient_intensity() * color();
  ambient_color.w() = 1.0;
  XVec4f diffuse_color = intensity() * color();
  diffuse_color.w() = 1.0;
  glLightf(GL_LIGHT0 + *light_count, GL_QUADRATIC_ATTENUATION, attenuation_(2));
  glLightf(GL_LIGHT0 + *light_count, GL_LINEAR_ATTENUATION, attenuation_(1));
  glLightf(GL_LIGHT0 + *light_count, GL_CONSTANT_ATTENUATION, attenuation_(0));
  glLightfv(GL_LIGHT0 + *light_count, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0 + *light_count, GL_AMBIENT, ambient_color);
  glLightfv(GL_LIGHT0 + *light_count, GL_DIFFUSE, diffuse_color);
  glLightfv(GL_LIGHT0 + *light_count, GL_SPECULAR, diffuse_color);
  X3LightNode::SetupLights(light_count);
  return;
}

void
X3Scene::Add(X3NodeType type, X3Node* node)
{
  if (type==X3NODE_VIEWPOINT) {
    if (viewpoint_) {
      delete viewpoint_;
    }
    viewpoint_ = reinterpret_cast<X3Viewpoint*>(node);
  } else {
    X3GroupingNode::Add(type, node);
  }

  return;
}

// Scene rendering: first setup viewing transform then render the scene contents.
void
X3Scene::Render() const
{
  if (viewpoint_) {
    viewpoint_->Render();
  }
  X3GroupingNode::Render();

  return;
}

// Scene lighting setup: first setup viewing transform then traverse the scene contents
// looking for lights.
void
X3Scene::SetupLights(int* light_count) const
{
  if (viewpoint_) {
    viewpoint_->SetupLights(light_count);
  }
  X3GroupingNode::SetupLights(light_count);

  return;
}

void
X3Scene::Print(std::ostream& ost, int offset) const
{
  ost << std::string(offset, ' ') << Name() << std::endl;
  if (viewpoint_) {
    viewpoint_->Print(ost, offset+1);
  }
  X3GroupingNode::Print(ost, offset);

  return;
}

void
X3Shape::Print(std::ostream& ost, int offset) const
{
  ost << std::string(offset, ' ') << Name() << std::endl;
  if (appearance_) {
    appearance_->Print(ost, offset+1);
  }
  if (geometry_) {
    geometry_->Print(ost, offset+1);
  }

  return;
}

// If you are adding a new type of geometry node it needs to be accounted for
// here so that the shape class knows about it.
void
X3Shape::Add(X3NodeType type, X3Node* node)
{
  if (type==X3NODE_BOX || type==X3NODE_CYLINDER 
     || type==X3NODE_CONE || type==X3NODE_INDEXEDFACESET) {
    geometry_ = reinterpret_cast<X3GeometryNode*>(node);
  } else if (type==X3NODE_APPEARANCE) {
    appearance_ = reinterpret_cast<X3Appearance*>(node);
  } else {
    X3Node::Add(type, node);
  }

  return;
}

// Shape rendering: setup the material in the appearance then proceed to render
// the geometry. 
// For the second assignment: if there is no appearance for the node, then its material
// properties are undefined. You can assume that each shape will have appearance set up
// explicitly.
void
X3Shape::Render() const
{
  if (appearance_) {
    appearance_->Render();
  }
  if (geometry_) {
    geometry_->Render();
  }

  return;
}

// Transform grouping node.
X3Transform::X3Transform(const char** atts) 
  : translation_(0.0f, 0.0f, 0.0f),
    rotation_(1.0f, 0.0f, 0.0f, 0.0f),
    scale_(1.0f, 1.0f, 1.0f),
    center_(0.0f, 0.0f, 0.0f)
{
  if (atts==0) {
    return;
  }
  for (const char** ref = atts; *ref!=NULL; ++ref) {
    const char* name = *ref;
    ++ref;
    if (*ref==NULL) {
      break;
    }
    if (strcmp(name, "translation")==0) {
      sscanf(*ref, "%f %f %f", &translation_(0), &translation_(1), &translation_(2));
    } else if (strcmp(name, "rotation")==0) {
      sscanf(*ref, "%f %f %f %f", &rotation_.axis(0), &rotation_.axis(1), 
             &rotation_.axis(2), &rotation_.angle_rad);
    } else if (strcmp(name, "scale")==0) {
      sscanf(*ref, "%f %f %f", &scale_(0), &scale_(1), &scale_(2));
    } else if (strcmp(name, "center")==0) {
      sscanf(*ref, "%f %f %f", &center_(0), &center_(1), &center_(2));
    } 

  }

  return;
}

void
X3Transform::Render() const
{
  /* TASK 3: YOUR CODE HERE
   * Modify this function to setup the transforms for
   * rendering its children.  Remember to properly push
   * and pop the OpenGL state. Once the transform is
   * set up, render the children nodes by calling
   * X3GroupingNode::Render().
   * A point (P) is transformed to its parent coordinates (P')
   * as follows: P' = T * C * R * S * - C * P
   * See explanation on notation in Section 10.4.4 of the X3D specs
  */

  glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(translation_(0), translation_(1), translation_(2));
    glTranslatef(center_(0), center_(1), center_(2));
    glRotatef(rotation_.angle_rad * 180 / M_PI, rotation_.axis(0), rotation_.axis(1), rotation_.axis(2));
    glScalef(scale_(0), scale_(1), scale_(2));
    glTranslatef(-center_(0), -center_(1), -center_(2));
    X3GroupingNode::Render();
  glPopMatrix();
  return;
}

void
X3Transform::SetupLights(int* light_count) const
{
  /* TASK 4: YOUR CODE HERE
   * Modify this function to properly setup lights at their
   * transformed locations.  This function is exactly the same
   * as the X3Transform::Render() method, except that instead of
   * calling X3GroupingNode::Render(), you call X3GroupingNode::SetupLights()
   * to set up the children's lights.
  */

  glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glTranslatef(translation_(0), translation_(1), translation_(2));
    glTranslatef(center_(0), center_(1), center_(2));
    glRotatef(rotation_.angle_rad * 180 / M_PI, rotation_.axis(0), rotation_.axis(1), rotation_.axis(2));
    glScalef(scale_(0), scale_(1), scale_(2));
    glTranslatef(-center_(0), -center_(1), -center_(2));
    X3GroupingNode::SetupLights(light_count);
  glPopMatrix();
  return;
}

X3Viewpoint::X3Viewpoint(const char** atts)
  : position_(0.0f, 0.0f, 10.0f)
{
  if (atts != 0) {
    for (const char** ref = atts; *ref!=NULL; ++ref) {
      const char* name = *ref;
      ++ref;
      if (*ref==NULL) {
        break;
      }
      if (strcmp(name, "position")==0) {
        sscanf(*ref, "%f %f %f", &position_(0), &position_(1), &position_(2));
      }
    }
  }

  position0_ = position_;
  up_ = XVec3f(0.0, 1.0, 0.0);
  gaze_ = -position_;
  gaze_.normalize();
  u_ = up_.cross(gaze_);
  u_.normalize();
  up_ = gaze_.cross(u_);
  radius = position_.norm();
  return;
}

void
X3Viewpoint::track_latlong(float dx, float dy)
{
  /* TASK 2: YOUR CODE HERE
   * When the user drags the mouse, you need to update/store
   * some states to effect camera movement along the
   * longitude/latitude of the spherical space around the
   * world when Render() is called.
  */
  XVec3f t = XVec3f(0.0, 1.0, 0.0);
  u_ = cos(dx) * u_ + (1 - cos(dx)) * (t.dot(u_)) * t + sin(dx) * (t.cross(u_));
  position_ = cos(dx) * position_ + (1 - cos(dx)) * (t.dot(position_)) * t + sin(dx) * (t.cross(position_));
  position_ = cos(dy) * position_ + (1 - cos(dy)) * (u_.dot(position_)) * t + sin(dy) * (u_.cross(position_));
  up_ = -position_.cross(u_);
  return;
}

void
X3Viewpoint::dolly(float dz)
{
  /* TASK 2: YOUR CODE HERE
   * When the user drags the mouse, you need to update/store
   * some states to effect camera movement along the
   * gaze direction when Render() is called.
  */
  position_ *= (1 + dz);
  return;
}

void
X3Viewpoint::Print(std::ostream& ost, int offset) const
{
  ost << std::string(offset, ' ') << Name();
  ost << ": pos=( " << position_ << ")" 
      << std::endl;

  return;
}

void
X3Viewpoint::Render() const
{
  gluLookAt(position_(0), position_(1), position_(2),
            0.0f, 0.0f, 0.0f,        // camera always pointing at the origin of world space
            up_(0), up_(1), up_(2));


  return;
}
