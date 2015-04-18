/*   CS580 HW   */
#include	"gz.h"
#include	"disp.h"

#include	"mathLib.h"
#include	<memory.h>
#include    <limits.h>
#include    "test.h"

int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
    /* create a framebuffer:
     -- allocate memory for framebuffer : (sizeof)GzPixel x width x height
     -- pass back pointer
     */
    *framebuffer = new char[sizeof(GzPixel) * width * height];
    
    if (*framebuffer == NULL)
        return GZ_FAILURE;
    
    return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, GzDisplayClass dispClass, int xRes, int yRes)
{
    
    /* create a display:
     -- allocate memory for indicated class and resolution
     -- pass back pointer to GzDisplay object in display
     */
    *display = new GzDisplay;
    
    if (*display == NULL)
        return GZ_FAILURE;
    else {
        (*display)->xres = xRes;
        (*display)->yres = yRes;
        (*display)->dispClass = dispClass;
        (*display)->fbuf = new GzPixel[yRes * xRes];
        
        if ((*display)->fbuf == NULL)
            return GZ_FAILURE;
    }
    
    return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
    /* clean up, free memory */
    delete [] display->fbuf;
    delete display;
    
    return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
    /* pass back values for an open display */
    *xRes = display->xres;
    *yRes = display->yres;
    *dispClass = display->dispClass;
    
    return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
    /* set everything to some default values - start a new frame */
    if (display->fbuf != NULL)
        delete [] display->fbuf;
    
    display->fbuf = new GzPixel[display->xres * display->yres];
    if (display->fbuf == NULL)
        return GZ_FAILURE;
    
    // Initialize the background color.
    // memset(display->fbuf, 0, sizeof(GzPixel) * display->xres * display->yres);
    int i;
    for (i = 0; i < display->yres * display->xres; i++) {
        /*
        display->fbuf[i].red = scale(128, 255, 4095);
        display->fbuf[i].green = scale(113, 255, 4095);
        display->fbuf[i].blue = scale(96, 255, 4095);
        */
        display->fbuf[i].red = scale(255, 255, 4095);
        display->fbuf[i].green = scale(255, 255, 4095);
        display->fbuf[i].blue = scale(255, 255, 4095);
        
        display->fbuf[i].z = INT_MAX;
        display->fbuf[i].frontTriangleId = -1; // -1 inidicates the background.
        display->fbuf[i].type = ZBUFFER_BACKGROUND;
    }
    
    return GZ_SUCCESS;
}

int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
    /* write pixel values into the display */
    if (0 > j || j >= display->yres || 0 > i || i >= display->xres)
        return GZ_FAILURE;
    
    int sub = j * display->xres + i;
    
    if (display->fbuf[sub].z >= z && z > 0) {
        display->fbuf[sub].red = r;
        display->fbuf[sub].green = g;
        display->fbuf[sub].blue = b;
        display->fbuf[sub].alpha = a;
        display->fbuf[sub].z = z;
    }
    
    return GZ_SUCCESS;
}

int GzPutDisplayExt(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z, int triangleId, int type)
{
    if (0 > j || j >= display->yres || 0 > i || i >= display->xres)
        return GZ_FAILURE;
    
    int sub = j * display->xres + i;
    
    if (display->fbuf[sub].z >= z && z > 0) {
        display->fbuf[sub].red = r;
        display->fbuf[sub].green = g;
        display->fbuf[sub].blue = b;
        display->fbuf[sub].alpha = a;
        display->fbuf[sub].z = z;
        display->fbuf[sub].type = type;
        display->fbuf[sub].frontTriangleId = triangleId;
    }
    
    return GZ_SUCCESS;
}

int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
    /* pass back pixel value in the display */
    /* check display class to see what vars are valid */
    if (0 > j || j >= display->yres || 0 > i || i >= display->xres)
        return GZ_FAILURE;
    
    int sub = j * display->xres + i;
    
    *r = display->fbuf[sub].red;
    *g = display->fbuf[sub].green;
    *b = display->fbuf[sub].blue;
    *z = display->fbuf[sub].z;
    
    return GZ_SUCCESS;
}

int GzGetFrontTriangleId(GzDisplay *display, int i, int j)
{
    int sub = j * display->xres + i;
    return display->fbuf[sub].frontTriangleId;
}

void GzGetVisibleTriangleIds(GzDisplay *display, bool visible[])
{
    int i, j;
    
    for (j = 0; j < display->yres; j++) {
        for (i = 0; i < display->xres; i++) {
            int id = GzGetFrontTriangleId(display, i, j);
            visible[id] = true;
        }
    }
}

int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{
    
    /* write pixels to ppm file based on display class -- "P6 %d %d 255\r" */
    fprintf(outfile, "P3 %hd %hd 4095\n", display->xres, display->yres);
    int sub, i, j;
    for (j = 0; j < display->yres; j++) {
        for (i = 0; i < display->xres; i++) {
            sub = j * display->xres + i;
            fprintf(outfile, "%d %d %d ",
                    display->fbuf[sub].red > 4095 ? 4095 : display->fbuf[sub].red,
                    display->fbuf[sub].green > 4095 ? 4095 : display->fbuf[sub].green,
                    display->fbuf[sub].blue > 4095 ? 4095 : display->fbuf[sub].blue);
            
        }
        fprintf(outfile, "\n");
    }
    
    return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{
    
    /* write pixels to framebuffer:
     - Put the pixels into the frame buffer
     - Caution: store the pixel to the frame buffer as the order of blue, green, and red 
     - Not red, green, and blue !!!
     */
    int i;
    for (i = 0; i < display->yres * display->xres;) {
        framebuffer[i] = scale(display->fbuf[i].blue, 5333, 255);
        i++;
        framebuffer[i] = scale(display->fbuf[i].green, 5333, 255);
        i++;
        framebuffer[i] = scale(display->fbuf[i].red, 5333, 255);
        i++;
    }
    
    return GZ_SUCCESS;
}