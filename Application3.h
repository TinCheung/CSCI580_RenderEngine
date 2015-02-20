// Application3.h: interface for the Application3 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "gz.h"
#include "rend.h"

class Application3
{
public:
	int runRenderer();
    int runAnimate(float [], float [], int);
protected:
	GzDisplay* display; // the display
	GzRender* render;   // the renderer
};
