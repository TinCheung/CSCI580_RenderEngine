//
//  PenInkApplication.h
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 15/4/14.
//  Copyright (c) 2015年 tantianx. All rights reserved.
//

#ifndef __CSCI580_RenderEngine__PenInkApplication__
#define __CSCI580_RenderEngine__PenInkApplication__

#include <stdio.h>
#include "disp.h"
#include "rend.h"
#include "gz.h"

class PenInkApplication {
public:
    int run();
    int setTriangles(int triangleNum, GzTriangle *triangles);
private:
    GzCamera camera;
    GzDisplay* display; // the display
    GzRender* render;   // the renderer
    GzTriangle *triangles;
};

#endif /* defined(__CSCI580_RenderEngine__PenInkApplication__) */
