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
#include <cmath>
#include <cassert>

#include "xvec.h"
#include "scene.h"
#include <iostream>
using namespace std;

static inline double 
max(double const& a, double const& b) 
{
  return (a>b) ? a : b;
}

Colorf 
Light::pt(XVec3f const& p, XVec3f const& n, Material const& mat) const
{
  double const d = p.dist(o);
  double const att = 1.0/(T0+T1*d+T2*d*d);
  XVec3f const L = o-p;
  return (att*max(L.dot(n),0.0)) * (diff * mat.diff);
}

bool 
Plane::contains(XVec3f const& p) const 
{
  XVec3f v(p-q);
  v.normalize();
  return (fabs(n.dot(v)) < 1.0e-8);
}

// If the ray lies in the plane or interesects it at a unique
// point then they are considered to intersect.
bool 
Plane::is_intersecting(Ray const& ray) const 
{
  return (contains(ray.e) ?
          true :
          // is the dot product zero within some numerical tolerance?
          (fabs(n.dot(ray.d)) > 1.0e-8));
}

// The point of intersection with given ray, in terms of 't'
// parameter for ray. If ray is parallel and not in the plane
// then intersection is theoretically at infinity. For this case,
// HUGE_VAL (largest representable double precision number) is
// returned.
double 
Plane::intersect(Ray const& ray) const 
{
  /* YOUR CODE HERE */
  if (!is_intersecting(ray)) {
    return HUGE_VAL;
  } else {
    return (q - ray.e).dot(n) / ray.d.dot(n);
  }
}

bool 
Sphere::contains(XVec3f const& p) const
{
  return (p-c).norm() <= r;
}

bool 
Sphere::is_intersecting(Ray const& ray) const
{
  XVec3f l(c-ray.e);
  float l2 = l.dot(l);
  float r2 = r*r;
  XVec3f dhat(ray.d);
  dhat.normalize();
  float tca = l.dot(dhat);
  if (tca < 0.0 && (l2 > r2)) {
    return false;
  }
  
  float d2 = l2 - (tca*tca);
  if (d2 > r2) {
    return false;
  }
  
  return true;
}

double 
Sphere::intersect(Ray const& ray) const
{
  /* YOUR CODE HERE */
  XVec3f l = c-ray.e;
  float l2 = l.dot(l);
  float r2 = r*r;
  XVec3f dhat(ray.d);
  dhat.normalize();
  float tca = l.dot(dhat);
  
  float d2 = l2 - (tca*tca);
  float thc = sqrt(r2 - d2);
  if (l2 > r2) {
    return tca - thc;
  } else {
    return tca + thc;
  }
}
