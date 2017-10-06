/*
 * Copyright (c) 2011, 2012 University of Michigan, Ann Arbor.
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
 * Author: Sugih Jamin
*/
#ifndef __VIEWING__
#define __VIEWING__

#include "xvec.h"

class Camera {
 public:
  XVec4f e; // eye position
  XVec4f g; // gaze direction
  XVec4f t; // top direction

  float zNear;  // z-coordinate of the near plane, negative number
  float zFar;   // z-coordinate of the far plane, negative number
  float fovy;   // y-field of view in degrees

  // basis vectors for camera's coordinate system
  XVec4f u; 
  XVec4f v;
  XVec4f w;

  Camera(){};
  Camera(XVec4f &eye_pos, XVec4f &gaze_dir, XVec4f &top_dir,
         float near_plane, float far_plane, float deg);
  virtual ~Camera() {};
  
  void orient();
  
  /* tracking motion:
   * translation by factor delta from current eye position . . .
  */
  void dolly(float delta); // . . . along the gaze direction
  void boom(float delta);  // . . . up or down
  void crab(float delta);  // . . . left or right

  /* pivot motion */
  void pan(float theta);   // theta in radian
  void tilt(float theta);  // theta in radian
  void roll(float theta);  // theta in radian
};

/*
 * The Basic Camera Moves
 * by Kyle Cassidy
 * August 2009
 * http://www.videomaker.com/article/14221/
 *
 * Dolly: Motion towards or motion from. The name comes from the old "dolly tracks" that
 *      used to be laid down for the heavy camera to move along - very much like railroad
 *      tracks - in the days before Steadicams got so popular. The phrase dolly-in means
 *      step towards the subject with the camera, while dolly-out means to step backwards
 *      with the camera, keeping the zoom the same. Zooming the camera changes the focal
 *      length of the lens, which can introduce wide-angle distortion or changes in the
 *      apparent depth of field. For this reason, it's sometimes preferable to dolly than zoom.
 * Boom: Moving the camera up or down without changing its vertical or horizontal axis;
 *      boom up means "move the camera up;" boom down means "move the camera down."
 *      You are not tilting the lens up, rather you are moving the entire camera up.
 *      Imagine your camera is on a tripod and you're raising or lowering the tripod head
 *      (this is exactly where the term comes from).
 * Crab: Crabbing is like dollying, but it involves motion left or right. Crab left
 *      means "move the camera physically to the left while maintaining its perpendicular
 *      relationship." This is not to be confused with a pan, where the camera remains
 *      firmly on its axis while the lens turns to one direction or the other. You might
 *      crab left to stay with a pedestrian as she walks down a street.
 * Pan: Moving the camera lens to one side or another. Look to your left, then look to
 *      your right - that's panning.
 * Tilt: Moving the cameras lens up or down while keeping its horizontal axis constant.
 *      Nod your head up and down - this is tilting.
 *
 * (Roll is not a normal camera movement, but more a flight movement.
 * Imagine an airplane rolling in place, from right-side up to upside down.)
*/

extern Camera cam;
extern void movecam(unsigned char key, int x, int y);
extern void setup_view();

#endif //__VIEWING__
