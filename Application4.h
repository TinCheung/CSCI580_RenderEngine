// Application4.h: interface for the Application4 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "gz.h"
#include "disp.h"
#include "rend.h"

class Application4
{
public:	
	int runRenderer();
protected:
	GzDisplay* display; // the display
	GzRender* render;   // the renderer
};
