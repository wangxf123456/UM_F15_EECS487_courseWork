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

#ifndef CANVAS_H
#define CANVAS_H

#include "rasterizer.h"

#include <vector>
#include <string>
using namespace std;

#define GRID_SIZE 8

#define LEFT    100
#define UP      101
#define RIGHT   102
#define DOWN    103

class Canvas {
public:
        Canvas();
        
        void resize(int x, int y);
        void mousePressedAt(int x, int y);
        bool mouseReleasedAt(int x, int y);
        void mouseMovedTo(int x, int y);
        
        void draw();
        void drawInRect(XVec4f &rect, bool inColor);

        void setGridOn(bool isOn);
        void setSnapOn(bool isOn);

        void setSelected(int index);
        void toggleSelectedAntialiased();
        void setSelectedVertex(char v);
        void deleteSelected();

        void nudge(int direction);
        
        void newScene();
        bool openScene(string location);
        void saveScene(string location);
        
        void bringForward();
        void bringToFront();
        void sendBackward();
        void sendToBack();
        
        void cancelDrawing();
        void setHardwareRender(bool isOn);
        
        void startDrawingClipView();
        void toggleIsDrawingClipped();
        
        XVec4f clippingView();
        XVec2i offset();

        XVec4f canvasRect(); 
        
        XVec4f currentColor();
        void setCurrentColor(XVec4f &v);
        Line* selected();

private:
        /* if the grid is visble, if snapping is on,
         if it is clipping, if hardware rendering is on */
        bool gridOn, snapOn, isDrawingClipped, isHardwareRender;
        
        /* width and height of the window */
        int width, height;
        
        /* width and height of the canvas,
         multiples of GRID_SIZE */
        int canvasWidth, canvasHeight;
        
        /* locations of mouse clicks or movements */
        XVec2f firstMouse, secondMouse,
                tempSecondMouse, tempThirdMouse;
        
        /* the colors of the triangle being drawn */
        XVec4f color0, color1, color2;
        
        /* all the triangles in the scene */
        vector<Line *> shapes;
        
        /* which triangle is selected */
        int indexOfSelected;
        
        /* which vertex of the triangle is selected */
        char selectedVertex;
        
        /* the clipping area */
        XVec4f clipView;
        
        /* if the user is drawing a clip area */
        bool isDrawingClipArea;
};

#endif // CANVAS_H
