//
//  Application6.h
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 15/3/26.
//  Copyright (c) 2015年 tantianx. All rights reserved.
//

#ifndef CSCI580_RenderEngine_Application6_h
#define CSCI580_RenderEngine_Application6_h

#include "disp.h"
#include "rend.h"
#include "gz.h"

class Application6
{
public:
    int runRenderer();
protected:
    GzDisplay* display[AAKERNEL_SIZE]; // the display
    GzRender* render[AAKERNEL_SIZE];   // the renderer
};


#endif
