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
 * Authors: Manoj Rajagopalan, Ari Grant, Sugih Jamin
 *
 */
#ifndef __SCENE_H__
#define __SCENE_H__

#include "xvec.h"

/*! Represents a ray passing through point p with direction v */
struct Ray {
  XVec3f e;
  XVec3f d;

  Ray(XVec3f const& e0, XVec3f const& d0) : e(e0), d(d0) {}
  XVec3f pt(double const& t) const { 
    XVec3f const tempd = t*d;
    return e+tempd; 
  }
};

typedef XVec3f Colorf;

/* Represents a material, like in OpenGL */

struct Material {

  Colorf amb, diff, spec;
  double s; // shininess

  Material(Colorf const& a,
           Colorf const& d,
           Colorf const& sp,
           double s0=0) :
           amb(a), diff(d), spec(sp), s(s0) {}
};

/* Represents a positional, non-directional, point light-source */

struct Light {

  XVec3f o; // position
  Colorf amb, diff, spec; // light components
  double T0, T1, T2; // attenuation parameters
  
  Light(XVec3f const& o0,
        Colorf const& a,
        Colorf const& d,
        Colorf const& s,
        double const& t0=1,
        double const& t1=0,
        double const& t2=0) :
        o(o0), amb(a), diff(d), spec(s), T0(t0), T1(t1), T2(t2) {}
  
  // Color at point p due to this light, given normal and material
  // At present, performs diffuse calculations only.
  Colorf pt(XVec3f const& p, XVec3f const& n, Material const& mat) const;
};

struct Plane {
        
  XVec3f n; // normal vector, not necessarily normalized
  XVec3f q;  // plane passes through  
  Material mat; 
        
  Plane(XVec3f const& normal, XVec3f const& q0, Material const& m) :
        n(normal), q(q0), mat(m) {}
        
  bool contains(XVec3f const& p) const;
        
  // If the ray lies in the plane or interesects it at a unique
  // point then they are considered to intersect.
  bool is_intersecting(Ray const& ray) const;
        
  // The point of intersection with given ray, in terms of 't'
  // parameter for ray. If ray is parallel and not in the plane
  // then intersection is theoretically at infinity. For this case,
  // HUGE_VAL (largest representable double precision number) is
  // returned.
  double intersect(Ray const& ray) const;
};

struct Sphere {

  XVec3f c; // center
  double r; // radius

  Sphere(double const& x0,
         double const& y0,
         double const& z0,
         double const& r0) :
         c(x0, y0, z0), r(r0) {}

  // Does the sphere contain the given point?
  bool contains(XVec3f const& p) const;
  
  // Does the given ray intersect the sphere?
  bool is_intersecting(Ray const& ray) const;
  
  // point of intersection of ray and sphere
  // in terms of the 't' parameter of the ray.
  // Negative value indicates that point of intersection
  // is in the direction opposite to that of the ray.
  // If they don't intersect, NaN (Not-a-Number)
  // is returned. This will signal floating point errors
  // if used subsequently so use the is_intersecting()
  // method above as a test before calling this.
  double intersect(Ray const& ray) const;

  // unit normal to sphere through the given point
  XVec3f unit_normal(XVec3f const& p) const {
    XVec3f n(p-c);
    n.normalize();
    return(n);
  }
};

#endif // __SCENE_H__
