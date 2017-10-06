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
#ifndef __SCENEGRAPH_H__
#define __SCENEGRAPH_H__

// A simplified X3D scene graph for EECS 487.
// See the specs at http://www.web3d.org/x3d/specifications/ISO-IEC-19775-X3DAbstractSpecification/
// for more information.

#include <math.h>
#include <vector>
#include <string>
#include <iostream>

#include "xvec.h"

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
  X3NODE_POINTLIGHT
};

// The base node class for all the nodes. Implements default behaviors.
class X3Node {
 public:
  virtual ~X3Node() {
  }
  // By default no children are allowed.
  virtual void Add(X3NodeType type, X3Node* node) {
    std::cerr << "WARNING: cannot add " << node->Name() << " to " << Name() << std::endl;
  }
  virtual const char* Name() const {
    return "Node";
  }
  virtual void Print(std::ostream& ost, int offset) const {
    ost << std::string(offset, ' ') << Name() << std::endl;
  }
  // Nothing is rendered.
  virtual void Render() const {
  }
  // Nothing is set up.
  virtual void SetupLights(int* light_count) const {
  }
};

// Base class for geometry nodes. 
class X3GeometryNode : public X3Node {
 public:
  virtual ~X3GeometryNode() {
  }
};

// Material node.
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
 private:
  float ambient_intensity_;
  XVec4f diffuse_color_;
  XVec4f emissive_color_;
  float shininess_;
  XVec4f specular_color_;
};

// Appearance node holds the material.
class X3Appearance : public X3Node {
 public:
 X3Appearance() : material_(NULL) {
  }
  virtual ~X3Appearance() {
  }
  virtual const char* Name() const {
    return "Appearance";
  }
  virtual void Add(X3NodeType type, X3Node* node);
  virtual void Print(std::ostream& ost, int offset) const;
  virtual void Render() const {
    if(material_)
      material_->Render();
  }
 private:
  X3Material* material_;
};

// Structure to store rotation. NOTE: the order of parameters
// is different in glRotate and X3D rotation.
struct rotation_t {
rotation_t(float ax, float ay, float az, float phi) 
  : axis(ax, ay, az), angle_rad(phi) { }
  XVec3f axis;
  float angle_rad;
};

class X3Viewpoint: public X3Node {
 public:
  X3Viewpoint(const char** atts = 0);
  virtual ~X3Viewpoint() { }
  virtual const char* Name() const {
    return "Viewpoint";
  }
  virtual void Print(std::ostream& ost, int offset) const;
  const XVec3f& position() const {
    return position_;
  }
  virtual void Render() const;
  virtual void SetupLights(int* light_count) const {
    Render();
  }

  void track_latlong(float dx, float dy);
  void dolly(float dz);

  void reset(void) { position_ = position0_; };

 private:
  XVec3f position_, position0_;
  XVec3f up_, gaze_, u_;
  float radius;
};


// Grouping node, passes rendering and other action to its children.
// Scene, transform, and group nodes are derived from this node.
// Scene also includes the viewpoint, while Transform takes care of transformations
// before recursing to children. See below.
class X3GroupingNode: public X3Node {
 public:
  virtual ~X3GroupingNode() { }
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

// Scene node is the root of the scene graph. It contains the viewpoint,
// which governs the positioning of the camera. Note that in our implementation
// there are no Camera nodes (so all the viewing transformation need to be put
// inside the X3Viewpoint class).
// There is always a default viewpoint which may be overridden based on what is 
// in the file.
class X3Scene: public X3GroupingNode {
 public:
  X3Scene() {
    viewpoint_ = new X3Viewpoint();
  }
  virtual ~X3Scene() {
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
 private:
  X3Viewpoint* viewpoint_;
};

// Simple grouping node just derives from GroupingNode.
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

// Transform class is responsible for hierarchy of modeling transformations.
// See Section 10.4.4 of X3D specs for the description of its behavior.
// NOTE: we do not handle scaleOrientation transforms here.
// So a point (P) is transformed to its parent coordinates (P') as follows:
// P' = T * C * R * S * - C * P
// See explanation on notation in Section 10.4.4 of the X3D specs
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
 private:
  XVec3f translation_;
  rotation_t rotation_;
  XVec3f scale_;
  XVec3f center_;
};

// Class for shapes: holds the appearance and the geometry part.
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

// Geometry nodes.

// A simple box geometry class.
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

// A cylinder geometry class: you will need to implement its Render() method.
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

// A cone geometry class
class X3Cone: public X3GeometryNode {
 public:
  X3Cone(const char** atts = 0);
  virtual ~X3Cone() {
  }
  virtual const char* Name() const {
    return "Cone";
  }
  virtual void Render() const;
 private:
  bool bottom_, side_;
  float height_, bottom_radius_;
};

// A list of XYZ points for the indexed face set class below.
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
  size_t size() const {      // number of coordinates/vertices
    return point_.size();
  }
 private:
  std::vector<XVec3f> point_;
};

// An indexed face set (mesh) class
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
  std::vector<int> coord_index_;
  X3Coordinate* coordinate_;
  
  // These two lists hold triangle and quad indices. We discard any other polygons.
  std::vector<XVec3i> triangles_;
  std::vector<XVec4i> quads_;

  std::vector<XVec3f> normals_; // You will compute normals when coords are added.
};

// Light nodes.
// The existing lights are always on.

// Base class for lights.
class X3LightNode: public X3Node {
 public:
  X3LightNode(const char** atts = 0);
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

class X3PointLight: public X3LightNode {
 public:
  X3PointLight(const char** atts = 0);
  virtual void SetupLights(int* light_count) const;
 private:
  XVec3f attenuation_;
  XVec3f location_;
  // The radius is infinity.
};


#endif  // __SCENGRAPH_H__
