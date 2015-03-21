// Application5.h: interface for the Application4 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "disp.h"
#include "rend.h"

class Application5
{
public:	
	int runRenderer();
protected:
	GzDisplay* display; // the display
	GzRender* render;   // the renderer
};
