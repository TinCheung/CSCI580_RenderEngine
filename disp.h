/* 
*  disp.h -- include file for Display
*  USC csci 580 
*/

#ifndef DISP_H
#define DISP_H

#include "gz.h"
#include <stdio.h>
#include <vector>

#define ZBUFFER_BACKGROUND  0
#define ZBUFFER_TEX         1
#define ZBUFFER_EDGE        2
#define ZBUFFER_SHADOW      3

using namespace std;

/* define general display pixel-type */
#ifndef GZ_PIXEL
typedef	struct {
  GzIntensity    red;
  GzIntensity    green;
  GzIntensity    blue;
  GzIntensity    alpha;
  GzDepth	 z;
    int frontTriangleId;
    int type;
} GzPixel;
#define GZ_PIXEL
#endif


/* define a display type */
#ifndef GZ_DISPLAY
typedef struct {
  unsigned short	xres;
  unsigned short	yres;
  GzDisplayClass	dispClass;
  short			open;
  GzPixel		*fbuf;		/* frame buffer array */
    float         *indicationFactor;
    int           *texId;
    vector<int> deletedTexId;
} GzDisplay;
#define GZ_DISPLAY
#endif

#define	MAXXRES	1024	/* put some bounds on size in case of error */
#define	MAXYRES	1024

#define	ARRAY(x,y)	(x+(y*display->xres))	/* simplify fbuf indexing */

// Function declaration
int GzNewFrameBuffer(char** framebuffer, int width, int height);
int GzNewDisplay(GzDisplay **display, GzDisplayClass dispClass, int xRes, int yRes);
int GzFreeDisplay(GzDisplay *display);
int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass);
int GzInitDisplay(GzDisplay *display);
int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z);
int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z);
int GzFlushDisplay2File(FILE* outfile, GzDisplay *display);
int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay* display);

int GzGetFrontTriangleId(GzDisplay *display, int i, int j);

// Enhance the zbuffer.
int GzPutDisplayExt(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z, int triangleId, int type);
int GzPutDisplayExtForTex(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z, int triangleId, int type, int texID);
int GzPutDisplayExtForEdge(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z, int triangleIds[], int idCount, int type);
void GzGetTrianglesVisibility(GzDisplay *display, int num, bool visibility[]);
void GzIndication(GzDisplay *display);
void GzInitIndicationFactor(GzDisplay *display);
void GzPrintTexId(GzDisplay *display);
void GzContourComplete(GzDisplay *display);

#endif