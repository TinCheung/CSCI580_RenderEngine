// Application2.h: interface for the Application2 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "disp.h"
// class GzDisplay;
class GzRender;

class Application2
{
public:
	int runRenderer();

protected:
	GzDisplay* display; // the display
	GzRender* render;   // the renderer
};
