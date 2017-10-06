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
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sstream>
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#ifdef _MSC_VER
#define sscanf sscanf_s
#endif
#include <GL/glew.h>
#endif
#include <GL/glut.h>
#endif

#include "image.h"
#include "scene.h"

static inline void
erase_char(string& s, char ch)
{
  for (int i = 0; i < (int)s.length(); ++i) {
    if (s[i]==ch) {
      s[i] = ' ';
    }
  }

  return;
}

void
X3Appearance::Add(X3NodeType type, X3Node* node)
{
  if (type==X3NODE_MATERIAL) {
    material_ = reinterpret_cast<X3Material*>(node);
  } else if (type==X3NODE_IMAGETEXTURE) {
    texture_ = reinterpret_cast<X3ImageTexture*>(node);
  } else if (type==X3NODE_TEXTURETRANSFORM) {
    texture_transform_ = reinterpret_cast<X3TextureTransform*>(node);
  } else {
    X3Node::Add(type, node);
  }

  return;
}

void
X3Appearance::Render() const
{
  if (material_) {
    material_->Render();
  } else {
    X3Material::DefaultRender();
  }
        
  if (texture_) {
    texture_->Render();
  } else {
    X3ImageTexture::DefaultRender();
  }
        
  if (texture_transform_) {
    texture_transform_->Render();
  } else {
    X3TextureTransform::DefaultRender();
  }

  return;
}

void
X3Appearance::DefaultRender()
{
  X3ImageTexture::DefaultRender();
  X3TextureTransform::DefaultRender();

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
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));
        
  glNormal3f(0.0f, 0.0f, -1.0f);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));
        
  glNormal3f(1.0f, 0.0f, 0.0f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));
        
  glNormal3f(-1.0f, 0.0f, 0.0f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));
        
  glNormal3f(0.0f, 1.0f, 0.0f);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f( 0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1), -0.5f*size_(2));
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-0.5f*size_(0),  0.5f*size_(1),  0.5f*size_(2));
        
  glNormal3f(0.0f, -1.0f, 0.0f);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1), -0.5f*size_(2));
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f( 0.5f*size_(0), -0.5f*size_(1),  0.5f*size_(2));
  glEnd();
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
      if (strcmp(*ref, "false")==0)
        bottom_ = false;
      else if (strcmp(*ref, "true")==0)
        bottom_ = true;
    } else if (strcmp(name, "side")==0) {
      if (strcmp(*ref, "false")==0)
        side_ = false;
      else if (strcmp(*ref, "true")==0)
        side_ = true;
    }
  }
}

void
X3Cone::Render() const
{
  const int N = 10;
  const float step = 2.0f * M_PI / N;
  const float step_tex = 1.0f / N;

  if (side_) {
    glBegin(GL_QUAD_STRIP);
    for (int k = 0; k < N+1; ++k) {
      //glNormal3f(-sin(k*step), bottom_radius_ / height_, -cos(k*step));
      glNormal3f(cos(k*step), bottom_radius_ / height_, sin(k*step));
      glTexCoord2f(k*step_tex, 1.0f);
      glVertex3f(0.0f, 0.5f*height_, 0.0f);
      glTexCoord2f(k*step_tex, 0.0f);
      glVertex3f(-bottom_radius_*sin(k*step), -0.5f*height_, 
                 -bottom_radius_*cos(k*step));
    }
    glEnd();
  }

  if (bottom_) {
    glFrontFace(GL_CW);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.5, 0.5f);
    glVertex3f(0.0f, -0.5f*height_, 0.0f);
    for (int k = 0; k < N+1; ++k) {
      glTexCoord2f(0.5f - 0.5f * sin(k*step), 0.5f - 0.5f * cos(k*step));
      glVertex3f(-bottom_radius_*sin(k*step), -0.5f*height_, -bottom_radius_*cos(k*step));
    }
    glEnd();
    glFrontFace(GL_CCW);
  }
}

void*
X3Cone::GetFieldPointer(const std::string& field_name, 
                        X3ValueType value_type_id)
{
  if (field_name=="bottomRadius" && value_type_id==X3VALUE_FLOAT) {
    return (void*)&bottom_radius_;
  } else {
    cerr << "WARNING: could not link type " << value_type_id
         << " to " << Name() << " node, field " << field_name << endl;
  }
  return NULL;
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
}

X3Curve::X3Curve(const char **atts) 
  : position_interpolator_(NULL), steps_(100)
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
    if (strcmp(name, "steps")==0) {
      sscanf(*ref, "%d", &steps_);
    }
  }
}

void
X3Curve::Add(X3NodeType type, X3Node* node)
{
  if (type==X3NODE_POSITIONINTERPOLATOR) {
    position_interpolator_ = reinterpret_cast<X3PositionInterpolator*>(node);
  } else {
    X3Node::Add(type, node);
  }
}

void
X3Curve::Render() const
{
  if (position_interpolator_==NULL) {
    return;
  }
  glDisable(GL_LIGHTING);
  glColor3f(1.0f, 0.2f, 0.2f);
  float t = position_interpolator_->start_key();
  float tend = position_interpolator_->end_key();
  float step = (tend-t)/steps_;
  glBegin(GL_LINE_STRIP);
  for (int k=0; k<steps_+1; ++k) {
    XVec3f pt;
    position_interpolator_->Evaluate(t, &pt);
    glVertex3fv(pt);
    t += step;
  }
  glEnd();
  glEnable(GL_LIGHTING);
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
}

void
X3Cylinder::Render() const
{
  /* YOUR CODE HERE TASK 2
   *
   * Specify texture coordinates to enable textured cylinders.  The
   * texture coordinates assignments MUST follow the X3D spec, "When a
   * texture is applied to a cylinder, it is applied differently to
   * the sides, top, and bottom. On the sides, the texture wraps
   * counterclockwise (from above) starting at the back of the
   * cylinder. The texture has a vertical seam at the back,
   * intersecting the X=0 plane. For the top and bottom caps, a circle
   * is cut out of the unit texture squares centred at (0, +-height/2,
   * 0) with dimensions 2*radius by 2*radius. The top texture
   * appears right side up when the top of the cylinder is tilted
   * toward the +Z-axis, and the bottom texture appears right side up
   * when the top of the cylinder is tilted toward the -Z-axis."
   * (http://www.web3d.org/documents/specifications/19775-1/V3.2/Part01/components/geometry3D.html#Cylinder)
   * You may want to consult X3Box::Render() and X3Cone::Render()
   * to see how the texture image is oriented vis-a-vis the object.
   */
  const int N = 20;
  const float step = 2.0f * M_PI / N;
  const float step_tex = 1.0f / N;

  if (top_) {
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.5f*height_, 0.0f);
    for (int k = 0; k < N+1; ++k) {
      glTexCoord2f(0.5f - 0.5f * sin(k*step), 0.5f + 0.5f * cos(k*step));
      glVertex3f(-radius_*sin(k*step), 0.5f*height_, -radius_*cos(k*step));
    }
    glEnd();
  }
        
  if (bottom_) {
    glFrontFace(GL_CW);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, -0.5f*height_, 0.0f);
    for (int k = 0; k < N+1; ++k) {
      glTexCoord2f(0.5f - 0.5f * sin(k*step), 0.5f - 0.5f * cos(k*step));
      glVertex3f(-radius_*sin(k*step), -0.5f*height_, -radius_*cos(k*step));
    }
    glEnd();
    glFrontFace(GL_CCW);
  }
        
  if (side_) {
    glBegin(GL_QUAD_STRIP);
    for (int k = 0; k < N+1; ++k) {
      glNormal3f(-sin(k*step), 0.0f, -cos(k*step));
      glTexCoord2f(k*step_tex, 1.0f);
      glVertex3f(-radius_*sin(k*step), 0.5f*height_, -radius_*cos(k*step));
      glTexCoord2f(k*step_tex, 0.0f);
      glVertex3f(-radius_*sin(k*step), -0.5f*height_, -radius_*cos(k*step));
    }
    glEnd();
  }
}

void
X3GroupingNode::Print(std::ostream& ost, int offset) const
{
  ost << std::string(offset, ' ') << "{" << std::endl;
  for (int i = 0; i < (int)children_.size(); ++i) {
    children_[i]->Print(ost, offset+2);
  }
  ost << std::string(offset, ' ') << "}" << std::endl;
}

void
X3GroupingNode::Render() const
{
  for (int i = 0; i < (int)children_.size(); ++i) {
    children_[i]->Render();
  }
}

void
X3GroupingNode::SetupLights(int* light_count) const
{
  for (int i = 0; i < (int)children_.size(); ++i) {
    children_[i]->SetupLights(light_count);
  }
}

X3ImageTexture::X3ImageTexture(const string& dirname, const char** atts) 
  : image_(NULL),
    texture_handle_(0),
    repeat_s_(true),
    repeat_t_(true)
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
    if (strcmp(name, "url")==0) {
      string ref_copy(*ref);
      erase_char(ref_copy, '"');
      istringstream ist(ref_copy);
      ist >> url_;
      url_ = dirname + "/" + url_;
      image_ = LoadImage(url_);
      
      //url_.assign(*ref);
    } else if (strcmp(name, "repeatS")==0) {
      if (strcmp(*ref, "false")==0) {
        repeat_s_ = false;
      } else if (strcmp(*ref, "true")==0) {
        repeat_s_ = true;
      }
    } else if (strcmp(name, "repeatT")==0) {
      if (strcmp(*ref, "false")==0) {
        repeat_t_ = false;
      } else if (strcmp(*ref, "true")==0) {
        repeat_t_ = true;
      }
    }
  }

  if (image_) {
    SetupTexture(image_);
  } else {
    cerr << "WARNING: no image loaded for " << url_ << endl;
  }
                        
  Print(std::cerr, 0);
}

void
X3ImageTexture::Add(X3NodeType type, X3Node *node)
{
}

void
X3ImageTexture::Print(std::ostream &ost, int offset) const
{
  ost << std::string(offset, ' ') << Name();
  ost << ": url=(" << url_ << ")" 
      << ", repeats=(" << repeat_s_ << "," << repeat_t_ << ")"
      << std::endl;
}

void
X3ImageTexture::Render() const
{
  if (texture_handle_==0 || image_==NULL) {
    // cerr << "WARNING: texture not rendered" << endl;
    return;
  }
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture_handle_);
  if (image_->hasAlpha()) {
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}

void
X3ImageTexture::DefaultRender()
{
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_ALPHA_TEST);
}

Image*
X3ImageTexture::LoadImage(const std::string& filename)
{
  switch(ImageIO::recognize_image_format(filename.c_str())) {
  case ImageIO::FORMAT_PNG:
    return ImageIO::LoadPNG(filename.c_str());
  case ImageIO::FORMAT_JPEG:
    return ImageIO::LoadJPEG(filename.c_str());
  default:
    cerr << "WARNING: unrecognized image format in " << filename << endl;
    break;
  }
  return NULL;
}

void
X3ImageTexture::SetupTexture(const Image* image)
{
  if (image==NULL) {
    texture_handle_ = 0;
    return;
  }
        
  /* YOUR CODE HERE TASK 1
   *
   * This function is called by the X3ImageTexture constructor after
   * an image has been loaded from file.  The X3ImageTexture class
   * members are assumed populated.  The purpose of the present
   * function is to set up OpenGL texturing: generate texture handle, 
   * bind the handle to a texture type, specify texture image/mipmaps, 
   * as well as set some texture environments and parameters.  You 
   * are REQUIRED to set up a mipmapped texture.  The characteristics 
   * of the image, such as its width, height, hasAlpha, etc. are 
   * stored in the Image object passed in.  The texture parameters in 
   * OpenGL should be set to agree with the parameters stored in this 
   * X3ImageTexture, wherever applicable.  Consult the class definitions 
   * for Image and X3ImageTexture. Modulate the texture with existing 
   * color instead of replacing it.
  */
  glGenTextures(1, &texture_handle_);
  glBindTexture(GL_TEXTURE_2D, texture_handle_);

  GLenum type = GL_RGB;
  if (image->hasAlpha()) {
    type = GL_RGBA;
  }
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);

  glTexImage2D(GL_TEXTURE_2D, 0, type, image->width(),
                image->height(), 0, type, GL_UNSIGNED_BYTE, 
                image->get_pixels());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

X3IndexedFaceSet::X3IndexedFaceSet(const char** atts) 
  : coordinate_(NULL), texture_coordinate_(NULL)
{
  std::vector<int> coord_index;
  std::vector<int> tex_coord_index;
        
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
      coord_index.clear();
      istringstream ist(ref_copy);
      while(ist.good()) {
        int index;
        ist >> index;
        if (!ist.fail()) {
          coord_index.push_back(index);
        }
      }
    } else if (strcmp(name, "texCoordIndex")==0) {
      tex_coord_index.clear();
      istringstream ist(ref_copy);
      while(ist.good()) {
        int index;
        ist >> index;
        if (!ist.fail()) {
          tex_coord_index.push_back(index);
        }
      }
    }
  }
  // Now pre-process the list into triangles and quads, ignore everything else.
  int istart = 0;
  for (int i = 0; i <= (int)coord_index.size(); ++i) {
    if (i==(int)coord_index.size() || coord_index[i]==-1) {
      switch(i-istart) {
      case 3:
        triangles_.push_back(XVec3i(coord_index[istart], 
                                    coord_index[istart+1], 
                                    coord_index[istart+2]));
        break;
      case 4:
        quads_.push_back(XVec4i(coord_index[istart], 
                                coord_index[istart+1], 
                                coord_index[istart+2], 
                                coord_index[istart+3]));
        break;
      }
      istart = i+1;
    }
  }
        
  // Process texture indices.
  if (!tex_coord_index.empty()) {
    int istart = 0;
    for (int i = 0; i <= (int)tex_coord_index.size(); ++i) {
      if (i==(int)tex_coord_index.size() || tex_coord_index[i]==-1) {
        switch(i-istart) {
        case 3:
          tex_triangles_.push_back(XVec3i(tex_coord_index[istart], 
                                          tex_coord_index[istart+1], 
                                          tex_coord_index[istart+2]));
          break;
        case 4:
          tex_quads_.push_back(XVec4i(tex_coord_index[istart], 
                                      tex_coord_index[istart+1], 
                                      tex_coord_index[istart+2], 
                                      tex_coord_index[istart+3]));
          break;
        }
        istart = i+1;
      }
    }
  }
}

void
X3IndexedFaceSet::Render() const
{
  if (!coordinate_) {
    return;
  }
        
  if (!texture_coordinate_) {
    // If texture_coordinate==NULL no need to specify texture coordinates.
        
    glBegin(GL_TRIANGLES);
    for (int k = 0; k < (int)triangles_.size(); ++k) {
      glNormal3fv(normals_[triangles_[k](0)]);
      glVertex3fv(coordinate_->point(triangles_[k](0)));
      glNormal3fv(normals_[triangles_[k](1)]);
      glVertex3fv(coordinate_->point(triangles_[k](1)));
      glNormal3fv(normals_[triangles_[k](2)]);
      glVertex3fv(coordinate_->point(triangles_[k](2)));
    }
    glEnd();
    glBegin(GL_QUADS);
    for (int k = 0; k < (int)quads_.size(); ++k) {
      glNormal3fv(normals_[quads_[k](0)]);
      glVertex3fv(coordinate_->point(quads_[k](0)));
      glNormal3fv(normals_[quads_[k](1)]);
      glVertex3fv(coordinate_->point(quads_[k](1)));
      glNormal3fv(normals_[quads_[k](2)]);
      glVertex3fv(coordinate_->point(quads_[k](2)));
      glNormal3fv(normals_[quads_[k](3)]);
      glVertex3fv(coordinate_->point(quads_[k](3)));
    }
    glEnd();

  } else {

    glBegin(GL_TRIANGLES);
    for(int k = 0; k < (int)triangles_.size(); ++k) {
      glNormal3fv(normals_[triangles_[k](0)]);
      //if( tex_triangles_.empty() == false ) {
      glTexCoord2fv(texture_coordinate_->point(tex_triangles_[k](0)));
      glVertex3fv(coordinate_->point(triangles_[k](0)));
      glNormal3fv(normals_[triangles_[k](1)]);
      glTexCoord2fv(texture_coordinate_->point(tex_triangles_[k](1)));
      glVertex3fv(coordinate_->point(triangles_[k](1)));
      glNormal3fv(normals_[triangles_[k](2)]);
      glTexCoord2fv(texture_coordinate_->point(tex_triangles_[k](2)));
      glVertex3fv(coordinate_->point(triangles_[k](2)));
      //}
    }
    glEnd();
    glBegin(GL_QUADS);
    for(int k = 0; k < (int)quads_.size(); ++k) {
      //if( tex_quads_.empty() == false ) {
      glNormal3fv(normals_[quads_[k](0)]);
      glTexCoord2fv(texture_coordinate_->point(tex_quads_[k](0)));
      glVertex3fv(coordinate_->point(quads_[k](0)));
      glNormal3fv(normals_[quads_[k](1)]);
      glTexCoord2fv(texture_coordinate_->point(tex_quads_[k](1)));
      glVertex3fv(coordinate_->point(quads_[k](1)));
      glNormal3fv(normals_[quads_[k](2)]);
      glTexCoord2fv(texture_coordinate_->point(tex_quads_[k](2)));
      glVertex3fv(coordinate_->point(quads_[k](2)));
      glNormal3fv(normals_[quads_[k](3)]);
      glTexCoord2fv(texture_coordinate_->point(tex_quads_[k](3)));
      glVertex3fv(coordinate_->point(quads_[k](3)));
      //}
    }
    glEnd();
  }
}

void
X3IndexedFaceSet::Add(X3NodeType type, X3Node* node)
{
  if (type==X3NODE_COORDINATE) {
    coordinate_ = reinterpret_cast<X3Coordinate*>(node);
    normals_.clear();
    normals_.resize(coordinate_->size(), XVec3f(0.0f, 0.0f, 0.0f));
    // Compute normals here...
    for (int k = 0; k < (int)triangles_.size(); ++k) {
      XVec3f e01 = coordinate_->point(triangles_[k](1))
        - coordinate_->point(triangles_[k](0));
      XVec3f e02 = coordinate_->point(triangles_[k](2))
        - coordinate_->point(triangles_[k](0));
      XVec3f normal = e01.cross(e02);
      normal.normalize();
      normals_[triangles_[k](0)] += normal;
      normals_[triangles_[k](1)] += normal;
      normals_[triangles_[k](2)] += normal;
    }
    for (int k = 0; k < (int)quads_.size(); ++k) {
      XVec3f e02 = coordinate_->point(quads_[k](2))
        - coordinate_->point(quads_[k](0));
      XVec3f e13 = coordinate_->point(quads_[k](3))
        - coordinate_->point(quads_[k](1));
      XVec3f normal = e02.cross(e13);
      normal.normalize();
      normals_[quads_[k](0)] += normal;
      normals_[quads_[k](1)] += normal;
      normals_[quads_[k](2)] += normal;
      normals_[quads_[k](3)] += normal;
    }
                
    cerr << (int)triangles_.size() << " triangles and "
         << (int)quads_.size() << " quads." << endl;
                
    for (int i = 0; i < (int)normals_.size(); ++i) {
      normals_[i].normalize();
    }
                
  } else if (type==X3NODE_TEXTURECOORDINATE) {
    texture_coordinate_ = reinterpret_cast<X3TextureCoordinate*>(node);
  } else {
    X3Node::Add(type, node);
  }
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
}

void
X3LightNode::SetupLights(int* light_count) const
{
  glEnable(GL_LIGHT0 + *light_count);
  ++(*light_count);
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
}

void
X3Material::Render() const
{
  glEnable(GL_LIGHTING);
        
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f*shininess_);
        
  XVec4f ambient_material(ambient_intensity_ * diffuse_color_(0),
                          ambient_intensity_ * diffuse_color_(1),
                          ambient_intensity_ * diffuse_color_(2),
                          1.0f);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_material);
        
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_color_);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_color_);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive_color_);
}

void
X3Material::DefaultRender()
{
  glDisable(GL_LIGHTING);
  glColor3f(1.0f, 1.0f, 1.0f);
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
}

void
X3PointLight::SetupLights(int* light_count) const
{
  XVec4f light_position(location_(0), location_(1), location_(2), 1.0f);
  glLightfv(GL_LIGHT0 + *light_count, GL_POSITION, light_position);
        
  XVec4f light_ambient = ambient_intensity() * color();
  glLightfv(GL_LIGHT0 + *light_count, GL_AMBIENT, light_ambient);
  XVec4f light_diffuse_specular = intensity() * color();
  glLightfv(GL_LIGHT0 + *light_count, GL_DIFFUSE, light_diffuse_specular);
  glLightfv(GL_LIGHT0 + *light_count, GL_SPECULAR, light_diffuse_specular);
        
  glLightf(GL_LIGHT0 + *light_count, GL_CONSTANT_ATTENUATION, attenuation_(0));
  glLightf(GL_LIGHT0 + *light_count, GL_LINEAR_ATTENUATION, attenuation_(1));
  glLightf(GL_LIGHT0 + *light_count, GL_QUADRATIC_ATTENUATION, attenuation_(2));
        
  X3LightNode::SetupLights(light_count);
}

void
X3Scene::Add(X3NodeType type, X3Node* node)
{
  if (type==X3NODE_VIEWPOINT) {
    // do not delete the previous viewpoint -- it was registered 
    // and will be deallocated later.
    viewpoint_ = reinterpret_cast<X3Viewpoint*>(node);
  } else {
    X3GroupingNode::Add(type, node);
  }
}

bool
X3Scene::AddLink(X3InterpolatorNode* src_node, X3Timer* timer_node,
                 X3Node* dest_node, const std::string& dest_field_name)
{
  void* dest_field = dest_node->GetFieldPointer(dest_field_name, 
                                                src_node->ValueTypeId());
  if (dest_field) {
    links_.push_back(link_t(timer_node, src_node, dest_field));
    return true;
  }
  return false;
}

// Look inside this function to see how interpolators are called and used.
void
X3Scene::Render() const
{
  // First of all:
  // Evaluate all the interpolators and assign linked fields.
  for (int i=0; i<(int)links_.size(); ++i) {
    links_[i].interpolator_->Evaluate(links_[i].timer_->ConvertTime(time_), 
                                      links_[i].field_);
  }
  // Setup viewing transform
  if (viewpoint_) {
    viewpoint_->Render();
  }
  // Render all the children now.
  X3GroupingNode::Render();
}

void
X3Scene::SetupLights(int* light_count) const
{
  if (viewpoint_) {
    viewpoint_->Render();
  }
  X3GroupingNode::SetupLights(light_count);
}

void
X3Scene::Print(std::ostream& ost, int offset) const
{
  ost << std::string(offset, ' ') << Name() << std::endl;
  if (viewpoint_) {
    viewpoint_->Print(ost, offset+1);
  }
  X3GroupingNode::Print(ost, offset);
}

void
X3Shape::Print(std::ostream& ost, int offset) const
{
  ost << std::string(offset, ' ') << Name() << std::endl;
  if (geometry_) {
    geometry_->Print(ost, offset+1);
  }
  if (appearance_) {
    appearance_->Print(ost, offset+1);
  }
}

void
X3Shape::Add(X3NodeType type, X3Node* node)
{
  if (type==X3NODE_BOX || type==X3NODE_CYLINDER 
      || type==X3NODE_CONE || type==X3NODE_INDEXEDFACESET
      || type==X3NODE_CURVE) {
    geometry_ = reinterpret_cast<X3GeometryNode*>(node);
  } else if (type==X3NODE_APPEARANCE) {
    appearance_ = reinterpret_cast<X3Appearance*>(node);
  } else {
    X3Node::Add(type, node);
  }
}

void
X3Shape::Render() const
{
  if (appearance_) {
    appearance_->Render();
  } else {
    X3Appearance::DefaultRender();
  }
        
  if (geometry_) {
    geometry_->Render();
  }
}

X3TextureCoordinate::X3TextureCoordinate(const char** atts)
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
        XVec2f st;
        ist >> st(0) >> st(1);
        if (!ist.fail()) {
          point_.push_back(st);
        }
      }
    }
  }
}

X3TextureTransform::X3TextureTransform(const char** atts) 
  : center_(0.0f, 0.0f),
    rotation_rad_(0.0f),
    scale_(1.0f, 1.0f),
    translation_(0.0f, 0.0f)
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
    if (strcmp(name, "center")==0) {
      sscanf(*ref, "%f %f", &center_(0), &center_(1));
    } else if (strcmp(name, "rotation")==0) {
      sscanf(*ref, "%f", &rotation_rad_);
    } else if (strcmp(name, "scale")==0) {
      sscanf(*ref, "%f %f", &scale_(0), &scale_(1));
    } else if (strcmp(name, "translation")==0) {
      sscanf(*ref, "%f %f", &translation_(0), &translation_(1));
    } 
  }
}

void
X3TextureTransform::Render() const
{
  /* YOUR CODE HERE TASK 3
   *
   * Make the appropriate OpenGL calls to implement
   * texture transform as described in X3D specs:
   * Tc' = -C * S * R * C * T * Tc
   * See the spec for explanation of the notation:
   * http://www.web3d.org/documents/specifications/19775-1/V3.2/Part01/components/texturing.html#TextureTransform
   * Don't forget to specify which OpenGL matrix stack
   * you want the matrices multiplied to.
  */

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(-center_(0), -center_(1), 0);  
  glScalef(scale_(0), scale_(1), 0);
  glRotatef(rotation_rad_ * 180.0 / M_PI, 0, 0, 1);
  glTranslatef(center_(0), center_(1), 0);
  glTranslatef(translation_(0), translation_(1), 0);
  return;
}

void
X3TextureTransform::DefaultRender()
{
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
}

X3Timer::X3Timer(const char** atts) 
  : shift_(0.0f), period_(-1.0f)
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
    if (strcmp(name, "shift")==0) {
      sscanf(*ref, "%f", &shift_);
    } else if (strcmp(name, "period")==0) {
      sscanf(*ref, "%f", &period_);
    } 
  }
}

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
}

void
X3Transform::Render() const
{
  // setup transforms
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(translation_(0), translation_(1), translation_(2));
  glTranslatef(center_(0), center_(1), center_(2));
  glRotatef(rotation_.angle_rad * 180.0 / M_PI, 
            rotation_.axis(0), rotation_.axis(1), rotation_.axis(2));
  glScalef(scale_(0), scale_(1), scale_(2));
  glTranslatef(-center_(0), -center_(1), -center_(2));
  // then render the group
  X3GroupingNode::Render();
  // then pop the state
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void
X3Transform::SetupLights(int* light_count) const
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(translation_(0), translation_(1), translation_(2));
  glTranslatef(center_(0), center_(1), center_(2));
  glRotatef(rotation_.angle_rad * 180.0 / M_PI, 
            rotation_.axis(0), rotation_.axis(1), rotation_.axis(2));
  glScalef(scale_(0), scale_(1), scale_(2));
  glTranslatef(-center_(0), -center_(1), -center_(2));
  // then render the group
  X3GroupingNode::SetupLights(light_count);
  // then pop the state
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void*
X3Transform::GetFieldPointer(const std::string& field_name, 
                             X3ValueType value_type_id)
{
  if (field_name=="translation" && value_type_id==X3VALUE_XVEC3F) {
    return (void*)&translation_;
  } else if (field_name=="scale" && value_type_id==X3VALUE_XVEC3F) {
    return (void*)&scale_;
  } else if (field_name=="rotation" && value_type_id==X3VALUE_ROTATION) {
    return (void*)&rotation_;
  } else {
    cerr << "WARNING: could not link type " << value_type_id
         << " to " << Name() << " node, field " << field_name << endl;
  }
  return NULL;
}

X3Viewpoint::X3Viewpoint(const char** atts) 
  : position_(0.0f, 0.0f, 10.0f),
    zoff_(0.0f),
    phi_(0.0f),
    theta_(0.0f)
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
    if (strcmp(name, "position")==0) {
      sscanf(*ref, "%f %f %f", &position_(0), &position_(1), &position_(2));
    }
  }
  // initialize the angles and offset from position
  zoff_ = position_.norm();
  float xzlen = sqrtf(position_(0) * position_(0) + position_(2) * position_(2));
  theta_ = 180.0 / M_PI * atan(position_(1) / xzlen);
  phi_ = - 180.0 / M_PI * atan2(position_(0), position_(2));
}

void
X3Viewpoint::Print(std::ostream& ost, int offset) const
{
  ost << std::string(offset, ' ') << Name();
  ost << ": pos=( " << position_ << ")" 
      << std::endl;
}

void
X3Viewpoint::Render() const
{
  glTranslatef(0.0f, 0.0f, -zoff_);
  glRotatef(theta_, 1.0f, 0.0f, 0.0f);
  glRotatef(phi_, 0.0f, 1.0f, 0.0f);
}

X3InterpolatorNode::X3InterpolatorNode(const char** atts) : smooth_(false)
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
    if (strcmp(name, "smooth")==0) {
      if (strcmp(*ref, "false")==0) {
        smooth_ = false;
      } else if (strcmp(*ref, "true")==0) {
        smooth_ = true;
      }
    } else if (strcmp(name, "key")==0) {
      string ref_copy(*ref);
      erase_char(ref_copy, ',');
      key_.clear();
      istringstream ist(ref_copy);
      while(ist.good()) {
        float t;
        ist >> t;
        if (!ist.fail()) {
          key_.push_back(t);
        }
      }
    }
  }
}

// Does binary search to return the key interval.
// Returns -1 if t is to the left of the first key,
// and N-1 if it is to the right of the last key.
int
X3InterpolatorNode::FindKeyInterval(float t) const
{
  if (key_.empty()) {
    return -1;
  }
  if (t<key_[0]) {
    return -1;
  }
  if (t>key_.back()) {
    return (int)(key_.size()-1);
  }
  int left = 0;
  int right = (int) (key_.size()-1);
  while(left<right) {
    int center = (left + right + 1) / 2;
    if (t < key_[center]) {
      right = center - 1;
    } else {
      left = center;
    }
  }
  return left;
}

X3OrientationInterpolator::X3OrientationInterpolator(const char** atts)
  : X3InterpolatorNode(atts)
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
    if (strcmp(name, "keyValue")==0) {
      string ref_copy(*ref);
      erase_char(ref_copy, ',');
      keyValue_.clear();
      istringstream ist(ref_copy);
      while(ist.good()) {
        rotation_t rot;
        ist >> rot.axis(0) >> rot.axis(1) >> rot.axis(2) >> rot.angle_rad;
        if (!ist.fail()) {
          keyValue_.push_back(rot);
        }
      }
    }
  }
}

void
X3OrientationInterpolator::Evaluate(float time, void* dest)
{
  rotation_t* dest_rot = reinterpret_cast<rotation_t*>(dest);
        
  // No splines for rotation data -- just do linear.
  *dest_rot = LinearInterpolation(time);
}

rotation_t
X3OrientationInterpolator::LinearInterpolation(float time)
{
  assert(keyValue_.size()==key().size());
  // first locate the interval with binary search
  int index = FindKeyInterval(time);
        
  /* YOUR CODE HERE TASK 4
   *
   * Replace the following line with a linear interpolation of
   * rotation values.  Consult the X3OrientationInterpolator class and
   * X3ScalarInterpolator::LinearInterpolation() to see what fields
   * need to be updated and how to update them.
   * It is basically the same linear interpolation as with X3ScalarInterpolation,
   * you just need to apply the interpolation to the axis and angles
   * separately and then put the results together into a rotation_t,
   * which you then return to the caller.
  */

  if (index < 0) {
    return keyValue_[0];
  } 
  if (index >= (int)keyValue_.size()-1) {
    return keyValue_.back();
  }
        
  float t = (time - key()[index])/(key()[index+1] - key()[index]);
  rotation_t result;
  result.angle_rad = (1.0 - t) * keyValue_[index].angle_rad + t * keyValue_[index+1].angle_rad;
  result.axis = (1.0 - t) * keyValue_[index].axis + t * keyValue_[index+1].axis;
  return result;
}

X3ScalarInterpolator::X3ScalarInterpolator(const char** atts) 
  : X3InterpolatorNode(atts)
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
    if (strcmp(name, "keyValue")==0) {
      string ref_copy(*ref);
      erase_char(ref_copy, ',');
      keyValue_.clear();
      istringstream ist(ref_copy);
      while(ist.good()) {
        float x;
        ist >> x;
        if (!ist.fail()) {
          keyValue_.push_back(x);
        }
      }
    }
  }
}

void
X3ScalarInterpolator::Evaluate(float time, void* dest)
{
  float* dest_val = reinterpret_cast<float*>(dest); 
  if (smooth()) {
    *dest_val = SmoothInterpolation(time);
  } else {
    *dest_val = LinearInterpolation(time);
  }
}

float
X3ScalarInterpolator::LinearInterpolation(float time)
{
  assert(keyValue_.size()==key().size());
  // first locate the interval with binary search
  int index = FindKeyInterval(time);
        
  if (index < 0) {
    return keyValue_[0];
  } 
  if (index >= (int)keyValue_.size()-1) {
    return keyValue_.back();
  }
        
  float t = (time - key()[index])/(key()[index+1] - key()[index]);
  return (1.0 - t) * keyValue_[index] + t * keyValue_[index+1];
}


float
X3ScalarInterpolator::SmoothInterpolation(float time)
{
  assert(keyValue_.size()==key().size());
  // first locate the interval with binary search
  int index = FindKeyInterval(time);
        
  /* YOUR CODE HERE TASK 5
   *
   * Replace the following line with spline interpolation of scalar
   * values using the Catmull-Rom spline.  Consult X3ScalarInterpolator 
   * class and X3ScalarInterpolator::LinearInterpolation().
   * Make sure you handle the border cases at the two end points.
   * You may consult or adapt your code from Lab8.
  */
  if (index < 0) {
    return keyValue_[0];
  } 
  if (index >= (int)keyValue_.size()-1) {
    return keyValue_.back();
  }

  float p0, p1 = keyValue_[index], p2 = keyValue_[index + 1], p3;
  float result;
  // does not interpolate p0p1 and pn-1pn
  if (index < 1) {
    p0 = keyValue_[index];
  } else {
    p0 = keyValue_[index - 1];
  }
  if (index >= (int)keyValue_.size()-2) {
    p3 = keyValue_[index + 1];
  } else {
    p3 = keyValue_[index + 2];
  }

  float t = (time - key()[index])/(key()[index+1] - key()[index]);
  float t2 = t * t, t3 = t * t2;

  result = 2 * p1 + (p2 - p0) * t + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2 + (-p0 + 3 * p1 - 3 * p2 + p3) * t3;
  return result / 2.0;
}

X3PositionInterpolator::X3PositionInterpolator(const char** atts) 
  : X3InterpolatorNode(atts)
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
    if (strcmp(name, "keyValue")==0) {
      string ref_copy(*ref);
      erase_char(ref_copy, ',');
      keyValue_.clear();
      istringstream ist(ref_copy);
      while(ist.good()) {
        XVec3f xyz;
        ist >> xyz(0) >> xyz(1) >> xyz(2);
        if (!ist.fail()) {
          keyValue_.push_back(xyz);
        }
      }
    }
  }
}

void
X3PositionInterpolator::Evaluate(float time, void* dest)
{
  XVec3f* dest_pt = reinterpret_cast<XVec3f*>(dest);
  //cerr << time << endl;
  if (smooth()) {
    *dest_pt = SmoothInterpolation(time);
  } else {
    *dest_pt = LinearInterpolation(time);
  }
}

XVec3f
X3PositionInterpolator::LinearInterpolation(float time)
{
  assert(keyValue_.size()==key().size());
  // first locate the interval with binary search
  int index = FindKeyInterval(time);
        
  if (index < 0) {
    return keyValue_[0];
  } 
  if (index >= (int)keyValue_.size()-1) {
    return keyValue_.back();
  }
        
  float t = (time - key()[index])/(key()[index+1] - key()[index]);
  return (1.0 - t) * keyValue_[index] + t * keyValue_[index+1];
}


XVec3f
X3PositionInterpolator::SmoothInterpolation(float time)
{
  assert(keyValue_.size()==key().size());
  // first locate the interval with binary search
  int index = FindKeyInterval(time);
  
  /* YOUR CODE HERE TASK 5
   *
   * Do spline interpolation of position values.
   * Due to polymorphism, you can simply copy your
   * smooth interpolation code for scalar values
   * here, almost verbatim.
  */
  if (index < 0) {
    return keyValue_[0];
  } 
  if (index >= (int)keyValue_.size()-1) {
    return keyValue_.back();
  }

  XVec3f p0, p1 = keyValue_[index], p2 = keyValue_[index + 1], p3;
  XVec3f result;

  // does not interpolate p0p1 and pn-1pn
  if (index < 1) {
    p0 = keyValue_[index];
  } else {
    p0 = keyValue_[index - 1];
  }
  if (index >= (int)keyValue_.size()-2) {
    p3 = keyValue_[index + 1];
  } else {
    p3 = keyValue_[index + 2];
  }

  float t = (time - key()[index])/(key()[index+1] - key()[index]);
  float t2 = t * t, t3 = t * t2;

  result = 2 * p1 + (p2 - p0) * t + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2 + (-p0 + 3 * p1 - 3 * p2 + p3) * t3;
  return result / 2.0;
}

