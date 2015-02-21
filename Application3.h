// Application3.h: interface for the Application3 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "gz.h"
#include "rend.h"
#include <queue>

using namespace std;

class Application3
{
public:
	int runRenderer();
    int runAnimate(float [], float [], float [], int);
protected:
	GzDisplay* display; // the display
	GzRender* render;   // the renderer
};
