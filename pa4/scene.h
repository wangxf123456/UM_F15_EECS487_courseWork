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
#ifndef __SCENE_H__
#define __SCENE_H__

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <string>
#include <iostream>

#include "xvec.h"
#include "image.h"

enum X3NodeType {
  X3NODE_UNKNOWN = -1,
  X3NODE_X3D,
  X3NODE_SCENE,
  X3NODE_GROUP,
  X3NODE_TRANSFORM,
  X3NODE_SHAPE,
  X3NODE_VIEWPOINT,
  X3NODE_BOX,
  X3NODE_CYLINDER,
  X3NODE_CONE,
  X3NODE_INDEXEDFACESET,
  X3NODE_APPEARANCE,
  X3NODE_MATERIAL,
  X3NODE_COORDINATE,
  X3NODE_POINTLIGHT,
  X3NODE_IMAGETEXTURE,
  X3NODE_LINK,
  X3NODE_POSITIONINTERPOLATOR,
  X3NODE_ORIENTATIONINTERPOLATOR,
  X3NODE_SCALARINTERPOLATOR,
  X3NODE_TIMER,
  X3NODE_TEXTURECOORDINATE,
  X3NODE_TEXTURETRANSFORM,
  X3NODE_CURVE,
};

// These are the type ids for nodes to communicate with interpolators.
enum X3ValueType {
  X3VALUE_UNKNOWN = -1,
  X3VALUE_FLOAT,
  X3VALUE_XVEC3F,
  X3VALUE_ROTATION,
};

// Base class for all the X3D nodes class hierarchy.
class X3Node {
 public:
  virtual ~X3Node() {
  }
  virtual void Add(X3NodeType type, X3Node* node) {
    std::cerr << "cannot add" << std::endl;
  }
  virtual const char* Name() const {
    return "Node";
  }
  virtual X3NodeType Type() const {
    return X3NODE_UNKNOWN;
  }

  // These two methods are for checking whether a given object supports
  // special interpolator or timer methods.
  virtual bool IsInterpolator() const {
    return false;
  }
  virtual bool IsTimer() const {
    return false;
  }

  virtual void Print(std::ostream& ost, int offset) const {
    ost << std::string(offset, ' ') << Name() << std::endl;
  }

  // By default no rendering
  virtual void Render() const {
  }
  // and no light setup.
  virtual void SetupLights(int* light_count) const {
  }

  // Any class may expose its internal fields so that the links can be
  // established between interpolator node outputs and this field.
  // This method is called by a class that wants to put values of type
  // value_type_id into fields of a given node. If the particular field is
  // not of a requested type, the method should return NULL. See X3Transform
  // class for an example of non-trivial method.
  virtual void* GetFieldPointer(const std::string& field_name, 
                                X3ValueType value_type_id) {
    return NULL;
  }
};

class X3GeometryNode : public X3Node {
 public:
  virtual ~X3GeometryNode() {
  }
};

class X3Material : public X3Node {
 public:
  X3Material(const char** atts = 0);
  virtual ~X3Material() {
  }
  virtual const char* Name() const {
    return "Material";
  }
  virtual void Print(std::ostream& ost, int offset) const;
  virtual void Render() const;
  static void DefaultRender();
 private:
  float ambient_intensity_;
  XVec4f diffuse_color_;
  XVec4f emissive_color_;
  float shininess_;
  XVec4f specular_color_;
};

// This class implements an image texture node.
class X3ImageTexture : public X3Node {
 public:
  X3ImageTexture(const std::string& dirname, const char** atts = 0);
  virtual ~X3ImageTexture() {
  }
  virtual void Add(X3NodeType type, X3Node *node);
  virtual const char * Name() const {
    return "ImageTexture";
  }
  virtual void Print(std::ostream &ost, int offset) const;
  // Rendering function should bind the texture name of this node and enable
  // texturing.
  virtual void Render() const;

  // This static function is called when no texture is provided.
  static void DefaultRender();
 private:
  // This function loads an image from a file.
  Image* LoadImage(const std::string& filename);

  // This function should setup all the texture modes. But first it should
  // generate a new texture name, bind it, and specify a texture image/mipmap 
  // hierarchy.
  void SetupTexture(const Image* image);
 private:
  std::string url_; // filename
  Image* image_; // image pointer
  unsigned int texture_handle_; // texture handle
  bool repeat_s_; // true if texture is to be repeated in s-direction, otherwise 
                  // clamp it
  bool repeat_t_; // same for t.
};

class X3TextureTransform : public X3Node {
 public:
  X3TextureTransform(const char** atts = 0);
  virtual ~X3TextureTransform() {
  }
  virtual const char * Name() const {
    return "TextureTransform";
  }
  virtual void Render() const;

  // This will be called when no texture transform is specified.
  static void DefaultRender();

 private:
  // Fields as in specification.
  XVec2f center_;
  float rotation_rad_;
  XVec2f scale_;
  XVec2f translation_;
};

class X3Appearance : public X3Node {
 public:
 X3Appearance() : material_(NULL), texture_(NULL), texture_transform_(NULL) {
  }
  virtual ~X3Appearance() {
  }
  virtual const char* Name() const {
    return "Appearance";
  }
  virtual void Add(X3NodeType type, X3Node* node);
  virtual void Print(std::ostream& ost, int offset) const;
  virtual void Render() const;
  static void DefaultRender();
 private:
  X3Material* material_;
  X3ImageTexture* texture_;
  X3TextureTransform* texture_transform_;
};

class X3GroupingNode: public X3Node {
 public:
  virtual ~X3GroupingNode() {
  }
  virtual void Add(X3NodeType type, X3Node* node) {
    children_.push_back(node);
  }
  virtual const char* Name() const {
    return "GroupingNode";
  }
  virtual void Print(std::ostream& ost, int offset) const;
  virtual void Render() const;
  virtual void SetupLights(int* light_count) const;
 private:
  std::vector<X3Node*> children_;
};

struct rotation_t {
rotation_t() : axis(1.0f, 0.0f, 0.0f), angle_rad(0.0f) {
}
rotation_t(float ax, float ay, float az, float phi) 
: axis(ax, ay, az), angle_rad(phi) {
}
  XVec3f axis;
  float angle_rad;
};

class X3Viewpoint: public X3Node {
 public:
  X3Viewpoint(const char** atts = 0);
  virtual ~X3Viewpoint() {
  }
  virtual const char* Name() const {
    return "Viewpoint";
  }
  virtual void Print(std::ostream& ost, int offset) const;
  const XVec3f& position() const {
    return position_;
  }
  virtual void Render() const;

  float zoff() const {
    return zoff_;
  }
  void set_zoff(float zoff) {
    zoff_ = zoff;
  }

  float phi() const {
    return phi_;
  }
  void set_phi(float phi) {
    phi_ = phi;
  }

  float theta() const {
    return theta_;
  }
  void set_theta(float theta) {
    theta_ = theta;
  }

 private:
  XVec3f position_;
  float zoff_;
  float phi_;
  float theta_;
};

// Non-standard Timer node: 
// converts the global time in the system into either a periodic value
// or non-periodic scaled value. See ConvertTime function for details.
class X3Timer : public X3Node {
 public:
  X3Timer(const char** atts = 0);
  virtual ~X3Timer() {
  }
  virtual bool IsTimer() const {
    return true;
  }
  virtual const char * Name() const {
    return "Timer";
  }

  // If the period is positive, this function 
  // returns [(t-shift_) mod period_] / period_
  // so that the output is always between zero and one, and it goes around 
  // every period_.
  // If the period is negative it returns (t - shift_) / (-period_)
  // thus it keeps increasing but the speedup is the same as for the 
  // corresponding periodic timer with period == -period_.
  // 
  // This function is called to produce the input value for the interpolator
  // function.
  float ConvertTime(float time) {
    time = time - shift_;
    if (period_>=0.0f) {
      int n = (int)floor(time / period_);
      time -= n*period_;
      time /= period_;
    } else {
      time = - time / period_;
    }
    return time;
  }
 private:
  float shift_, period_;
};

// Base class for all interpolator nodes. Provides common functionality, that 
// is, dealing with key sequence, as well as common interface for evaluation.
// 
// The convention is that all periodic evaluators have their keys starting from 
// zero and going till one.
// Non-periodic interpolators can have any key sequences.
class X3InterpolatorNode : public X3Node {
 public:
  X3InterpolatorNode(const char** atts = 0);
  virtual ~X3InterpolatorNode() {
  }
  virtual bool IsInterpolator() const {
    return true;
  }

  // Each derived interpolator will need to provide its value type id.
  virtual X3ValueType ValueTypeId() const = 0;

  // Evaluator function used by the animation. Takes the input time
  // (which is the result of Timer::ConvertTime called on the global time),
  // produces a value of a function, and copies it into the result destination
  // (the destination is typically a field of another node like 
  // X3Transform::translation_ but it may be something else like in 
  // X3Curve::Render function.
  virtual void Evaluate(float time, void* dest) = 0; // evaluates the 
                                                     // function and puts 
                                                     // result into dest
  float start_key() const {
    return key_.front();
  }

  float end_key() const {
    return key_.back();
  }

  // Does binary search to return the key interval.
  // Returns -1 if t is to the left of the first key,
  // and N-1 if it is to the right of the last key.
  int FindKeyInterval(float t) const;

  const std::vector<float>& key() const {
    return key_;
  }

  // If smooth is true, the interpolator needs to do smooth interpolation
  // using Catmull-Rom cubic splines.
  bool smooth() const {
    return smooth_;
  }
 private:
  std::vector<float> key_;
  bool smooth_;
};

// Link node does not produce a usual X3D node, rather it is processed into
// this structure that links together three pieces of data:
// timer, interpolator and the pointer to the field to be updated.
struct link_t {
link_t(X3Timer* timer, X3InterpolatorNode* interpolator, void* field) 
: timer_(timer), interpolator_(interpolator), field_(field) {
}
  X3Timer* timer_;
  X3InterpolatorNode* interpolator_;
  void* field_;
};

class X3Scene: public X3GroupingNode {
 public:
  X3Scene() {
    viewpoint_ = new X3Viewpoint;
  }
  virtual ~X3Scene() {
    std::cout << "Scene will delete "
              << registered_nodes_.size() << " nodes." << std::endl;
    for(std::vector<X3Node*>::iterator ni = registered_nodes_.begin();
        ni != registered_nodes_.end(); ++ni) {
      delete *ni;
    }
  }
  virtual void Add(X3NodeType type, X3Node* node);
  virtual const char* Name() const {
    return "Scene";
  }
  virtual void Print(std::ostream& ost, int offset) const;
  virtual void Render() const;
  virtual void SetupLights(int* light_count) const;

  X3Viewpoint* viewpoint() {
    return viewpoint_;
  }

  void set_time(float time) {
    time_ = time;
  }

  // A new function that links together three components of an animation link
  // the timer node that provides time, the interpolator node that produces a 
  // value, and a destination field of some node.
  bool AddLink(X3InterpolatorNode* src_node, X3Timer* timer_node,
               X3Node* dest_node, const std::string& dest_field_name);

  // This is just some accounting of nodes in the scene for later deallocation
  // on exit.
  void Register(X3Node* node) {
    registered_nodes_.push_back(node);
  }
 private:
  X3Viewpoint* viewpoint_;

  float time_; // current time of the scene. this is being set externally.

  std::vector<link_t> links_; // All the valid animation links.
  std::vector<X3Node*> registered_nodes_;
};

class X3Group: public X3GroupingNode {
 public:
  virtual ~X3Group() {
  }
  virtual const char* Name() const {
    return "Group";
  }
  virtual void Print(std::ostream& ost, int offset) const {
    ost << std::string(offset, ' ') << Name() << std::endl;
    X3GroupingNode::Print(ost, offset);
  }
};

class X3Transform: public X3GroupingNode {
 public:
  X3Transform(const char** atts = 0);
  virtual ~X3Transform() {
  }
  virtual const char* Name() const {
    return "Transform";
  }
  virtual void Print(std::ostream& ost, int offset) const {
    ost << std::string(offset, ' ') << Name() << std::endl;
    X3GroupingNode::Print(ost, offset);
  }
  virtual void Render() const;
  virtual void SetupLights(int* light_count) const;

  // See this function for example of setting up animated fields.
  virtual void* GetFieldPointer(const std::string& field_name, 
                                X3ValueType value_type_id);
 private:
  XVec3f translation_;
  rotation_t rotation_;
  XVec3f scale_;
  XVec3f center_;
};

class X3Shape: public X3Node {
 public:
 X3Shape() : geometry_(NULL), appearance_(NULL) {
  }
  virtual ~X3Shape() {
  }
  virtual const char* Name() const {
    return "Shape";
  }
  virtual void Print(std::ostream& ost, int offset) const;
  virtual void Add(X3NodeType type, X3Node* node);
  virtual void Render() const;
 private:
  X3GeometryNode* geometry_;
  X3Appearance* appearance_;
};

class X3Box: public X3GeometryNode {
 public:
  X3Box(const char **atts = 0);
  virtual ~X3Box() {
  }
  virtual const char* Name() const {
    return "Box";
  }
  virtual void Print(std::ostream& ost, int offset) const {
    ost << std::string(offset, ' ') << Name();
    ost << ": size=( " << size_ << ")" << std::endl;
  }
  virtual void Render() const;
 private:
  XVec3f size_;
};

class X3Cylinder: public X3GeometryNode {
 public:
  X3Cylinder(const char** atts = 0);
  virtual ~X3Cylinder() {
  }
  virtual const char* Name() const {
    return "Cylinder";
  }
  virtual void Render() const;
 private:
  bool top_, bottom_, side_;
  float height_, radius_;
};

class X3Cone: public X3GeometryNode {
 public:
  X3Cone(const char** atts = 0);
  virtual ~X3Cone() {
  }
  virtual const char* Name() const {
    return "Cone";
  }
  virtual void Render() const;
  virtual void* GetFieldPointer(const std::string& field_name, 
                                X3ValueType value_type_id);
 private:
  bool bottom_, side_;
  float height_, bottom_radius_;
};

class X3Coordinate: public X3Node {
 public:
  X3Coordinate(const char** atts = 0);
  virtual ~X3Coordinate() {
  }
  virtual const char* Name() const {
    return "Coordinate";
  }
  const XVec3f& point(int i) const {
    assert(i < (int)point_.size());
    return point_[i];
  }
  size_t size() const {
    return point_.size();
  }
 private:
  std::vector<XVec3f> point_;
};

class X3TextureCoordinate: public X3Node {
 public:
  X3TextureCoordinate(const char** atts = 0);
  virtual ~X3TextureCoordinate() {
  }
  virtual const char* Name() const {
    return "TextureCoordinate";
  }
  const XVec2f& point(int i) const {
    assert(i < (int)point_.size());
    return point_[i];
  }
  size_t size() const {
    return point_.size();
  }
 private:
  std::vector<XVec2f> point_;
};

class X3IndexedFaceSet: public X3GeometryNode {
 public:
  X3IndexedFaceSet(const char** atts = 0);
  virtual ~X3IndexedFaceSet() {
  }
  virtual const char* Name() const {
    return "IndexedFaceSet";
  }
  virtual void Render() const;
  virtual void Add(X3NodeType type, X3Node* node);
 private:
  X3Coordinate* coordinate_;
  X3TextureCoordinate* texture_coordinate_; // texture coords read in from a 
                                            // file.
  
  std::vector<XVec3i> triangles_;
  std::vector<XVec4i> quads_;
  std::vector<XVec3f> normals_; // We always compute normals ourselves
                                // when coords are added.

  // The following two fields store indices of texture coordinates
  // If these are empty you need to use the indices of vertices to look into
  // the texture_coordinate_ values.
  std::vector<XVec3i> tex_triangles_;
  std::vector<XVec4i> tex_quads_;
};

// Light nodes.

class X3LightNode: public X3Node {
 public:
  X3LightNode(const char** atts = 0);
  virtual ~X3LightNode() {
  }
  virtual void SetupLights(int* light_count) const;
  float ambient_intensity() const {
    return ambient_intensity_;
  }
  const XVec4f& color() const {
    return color_;
  }
  float intensity() const {
    return intensity_;
  }
 private:
  float ambient_intensity_;
  XVec4f color_;
  float intensity_;
};

// Point light source
class X3PointLight: public X3LightNode {
 public:
  X3PointLight(const char** atts = 0);
  virtual ~X3PointLight() {
  }
  virtual const char* Name() const {
    return "PointLight";
  }
  virtual void SetupLights(int* light_count) const;
 private:
  XVec3f attenuation_;
  XVec3f location_;
  // The radius is infinity.
};

class X3ScalarInterpolator: public X3InterpolatorNode {
 public:
  X3ScalarInterpolator(const char** atts = 0);
  virtual ~X3ScalarInterpolator() {
  }
  virtual const char* Name() const {
    return "ScalarInterpolator";
  }
  virtual X3ValueType ValueTypeId() const {
    return X3VALUE_FLOAT;
  }
  virtual void Evaluate(float time, void* dest); // evaluates the function and 
                                                 // puts the result into dest.
 private:
  float LinearInterpolation(float time);
  float SmoothInterpolation(float time);
 private:
  std::vector<float> keyValue_;
};

class X3PositionInterpolator: public X3InterpolatorNode {
 public:
  X3PositionInterpolator(const char** atts = 0);
  virtual ~X3PositionInterpolator() {
  }
  virtual const char* Name() const {
    return "PositionInterpolator";
  }
  virtual X3ValueType ValueTypeId() const {
    return X3VALUE_XVEC3F;
  }
  virtual void Evaluate(float time, void* dest); // evaluates the function and 
                                                 // puts the result into dest.
 private:
  XVec3f LinearInterpolation(float time);
  XVec3f SmoothInterpolation(float time);
 private:
  std::vector<XVec3f> keyValue_;
};

class X3OrientationInterpolator: public X3InterpolatorNode {
 public:
  X3OrientationInterpolator(const char** atts = 0);
  virtual ~X3OrientationInterpolator() {
  }
  virtual const char* Name() const {
    return "OrientationInterpolator";
  }
  virtual X3ValueType ValueTypeId() const {
    return X3VALUE_ROTATION;
  }
  virtual void Evaluate(float time, void* dest); // evaluates the function and 
                                                 // puts the result into dest.
 private:
  rotation_t LinearInterpolation(float time);
 private:
  std::vector<rotation_t> keyValue_;
};

// This class will help you visualize and debug your position interpolation.
// Each curve has to have a position interpolation node as a child node,
// then it will render a curve corresponding to sampling of that interpolator
// using steps_ steps.
class X3Curve : public X3GeometryNode {
 public:
  X3Curve(const char** atts = 0);
  virtual ~X3Curve() {
  }
  virtual const char* Name() const {
    return "Curve";
  }
  virtual void Add(X3NodeType type, X3Node* node);
  virtual void Render() const;
 private:
  X3PositionInterpolator* position_interpolator_;
  int steps_;
};


#endif  // __SCENE_H__
