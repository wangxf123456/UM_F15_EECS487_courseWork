/*
 * Copyright (c) 2010 University of Michigan, Ann Arbor.
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
 * Authors:
 *        Ari Grant, grantaa@umich.edu
 *        Sugih Jamin, jamin@eecs.umich.edu
 */

#include <vector>
using namespace std;

#include "xvec.h"

typedef enum view {
	SATURATION_VALUE_VIEW,
	HUE_VIEW,
	OPACITY_VIEW,
	OTHER_VIEW
} ViewArea;

class ColorPicker {
public:
	ColorPicker();
	
	void draw();
	void drawNoSelection();
	void resize(int w, int h, int x, int y);
	
	void mousePressedAt(int x, int y);
	void mouseReleasedAt(int x, int y);

	XVec4f& currentColor();
	
	void setPreviousColors(XVec4f &a, XVec4f &b, XVec4f &c);
	
private:
	/* the current color */
	XVec4f color;
	
	/* the previous colors */
	vector<XVec4f> historyColors;
	
	/* the location of the color picker in the window */
	int width, height;
	int originX, originY;
	
	/* which part of the picker a mouse click begins */
	ViewArea viewClicked;
};
