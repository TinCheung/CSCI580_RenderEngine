/*   CS580 HW   */
#include	"gz.h"
#include	"disp.h"

#include	"mathLib.h"
#include	<memory.h>
#include    <limits.h>
#include    "test.h"
#include    <math.h>
#include    "myRand.h"
#include    <stack>

#define DISTANCE_THRESHOLD 40

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
        (*display)->indicationFactor = new float[yRes * xRes];
        (*display)->texId = new int[yRes * xRes];
        
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
        display->texId[i] = 0;
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
    int ftId = display->fbuf[sub].frontTriangleId; // == -1 ? triangleId : display->fbuf[sub].frontTriangleId;
    
    if ((display->fbuf[sub].z >= z && z > 0)) {
        display->fbuf[sub].red = r;
        display->fbuf[sub].green = g;
        display->fbuf[sub].blue = b;
        display->fbuf[sub].alpha = a;
        display->fbuf[sub].z = z;
        display->fbuf[sub].type = type;
        display->fbuf[sub].frontTriangleId = triangleId;
    }
    else if (ftId == triangleId) {
        //printf("same %d, %d \n", ftId, triangleId);
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

int GzPutDisplayExtForTex(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z, int triangleId, int type, int texID)
{
    if (0 > j || j >= display->yres || 0 > i || i >= display->xres)
        return GZ_FAILURE;
    
    int sub = j * display->xres + i;
    int ftId = display->fbuf[sub].frontTriangleId; // == -1 ? triangleId : display->fbuf[sub].frontTriangleId;
    
    if ((display->fbuf[sub].z >= z && z > 0)) {
        display->fbuf[sub].red = r;
        display->fbuf[sub].green = g;
        display->fbuf[sub].blue = b;
        display->fbuf[sub].alpha = a;
        display->fbuf[sub].z = z;
        display->fbuf[sub].type = type;
        display->fbuf[sub].frontTriangleId = triangleId;
        display->texId[sub] = texID;
    }
    else if (ftId == triangleId) {
        //printf("same %d, %d \n", ftId, triangleId);
        display->fbuf[sub].red = r;
        display->fbuf[sub].green = g;
        display->fbuf[sub].blue = b;
        display->fbuf[sub].alpha = a;
        display->fbuf[sub].z = z;
        display->fbuf[sub].type = type;
        display->fbuf[sub].frontTriangleId = triangleId;
        display->texId[sub] = texID;
    }
    return GZ_SUCCESS;
}

int GzPutDisplayExtForEdge(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z, int triangleIds[], int idCount, int type)
{
    if (0 > j || j >= display->yres || 0 > i || i >= display->xres)
        return GZ_FAILURE;
    
    int sub = j * display->xres + i;
    
    bool draw;
    
    if (i == 463 && j == 561)
        i = 463;
    
    draw = display->fbuf[sub].z >= z && z > 0;
    int pixelTriId = display->fbuf[sub].frontTriangleId;
    
    for (int i = 0; i < idCount; i++)
        draw = draw | (pixelTriId == triangleIds[i]);
    
    if (draw) {
        display->fbuf[sub].red = r;
        display->fbuf[sub].green = g;
        display->fbuf[sub].blue = b;
        display->fbuf[sub].alpha = a;
        display->fbuf[sub].z = z;
        display->fbuf[sub].type = type;
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

void GzGetTrianglesVisibility(GzDisplay *display, int num, bool visibility[])
{
    int i, j, sub, fid;
    
    for (i = 0; i < num; i++) visibility[i] = false;
    for (i = 0; i < display->xres; i++) {
        for (j = 0; j < display->yres; j++) {
            sub = j * display->xres + i;
            fid = display->fbuf[sub].frontTriangleId;
            if (fid == -1) continue;
            visibility[fid] = true;
        }
    }
}

float findDistanceToNearestEdge(GzDisplay *display, int x, int y, int *minX, int *minY)
{
    float distance = 10000000;
    float tempDistance;
    
    int i, j, sub, pixelType;
    
    for (i = 0; i < display->xres; i++) {
        for (j = 0; j < display->yres; j++) {
            sub = j * display->xres + i;
            pixelType = display->fbuf[sub].type;
            if (pixelType == ZBUFFER_EDGE) {
                tempDistance = absf(i-x);
                distance = tempDistance > distance ? distance : tempDistance;
                *minY = y;
                *minX = i;
            }
        }
    }
    /*
    for (j = 0; j < display->yres; j++) {
        sub = j * display->xres + x;
        pixelType = display->fbuf[sub].type;
        if (pixelType == ZBUFFER_EDGE) {
            tempDistance = sqrtf(powf(j - y, 2));
            distance = tempDistance > distance ? distance : tempDistance;
        }
    }
    */
    return distance;
}

void GzInitIndicationFactor(GzDisplay *display)
{
    int i, j, sub, temp;
    
    temp = 5;
    int sign = 1;
    for (i = 0; i < display->xres; i++) {
        for (j = 0; j < display->yres; j++) {
            float random = rand2() % 2;
            sub = j * display->xres + i;
            if (temp > 15 || temp < 5) {
                sign *= -1;
                temp = temp > 15 ? 15 : 5;
            }
            else {
                temp += random * sign;
            }
            
            float noise = temp / 10.0;
            //printf("noise: %f\n", noise);
            display->indicationFactor[sub] = absf(noise) * DISTANCE_THRESHOLD;
        }
    }
}
/*
bool deletedTextureInNeighbourhood(GzDisplay *display, int x, int y)
{
    stack<int> nX;
    stack<int> nY;
    
    nX.push(x);
    nY.push(y);
    
    int i, j;
    int texId = display->texId[y * display->yres + x];
    
    bool visited[display->xres * display->yres];
    
    for (i = 0; i < display->xres * display->yres; i++)visited[i] = false;
    
    while (!nX.empty()) {
        int tempx = nX.top();
        int tempy = nY.top();
        
        nX.pop();
        nY.pop();
        
        if (display->fbuf[tempy * display->xres + tempx].red == 4905)
            return true;
        
        for (i = tempx - 1; i <= tempx + 1; i++) {
            for (j = tempy - 1; j <= tempy + 1; j++) {
                if (i == tempx && j == tempy) continue;
                int sub = j * display->xres + i;
                if (display->fbuf[sub].type == ZBUFFER_TEX && display->texId[sub] == texId && !visited[j * display->xres + i])
                {
                    nX.push(i);
                    nY.push(j);
                    visited[j * display->xres + i] = true;
                }
            }
        }
    }
    
    return false;
}*/

bool deletedTextureInNeighbourhood(GzDisplay *display, int x, int y)
{
    int i, j;
    
    for (i = x - 100; i <= x + 100; i++) {
        for (j = y - 100; j <= y + 100; j++) {
            int sub = j * display->xres + i;
            if (display->fbuf[sub].type == ZBUFFER_TEX && display->texId[sub] == display->texId[y * display->xres + x]&& display->fbuf[sub].red == 4090)
            {
                return true;
            }
        }
    }
    
    return false;
}

void GzIndication(GzDisplay *display)
{
    int i, j, sub, pixelType, minX, minY;
    float distance, b, c;
    
    b = 0.4;
    c = 0.8;
    GzInitIndicationFactor(display);
    for (i = 0; i < display->xres; i++) {
        printf("i: %d\n", i);
        for (j = 0; j < display->yres; j++) {
            sub = j * display->xres + i;
            pixelType = display->fbuf[sub].type;
            if (pixelType == ZBUFFER_TEX) {
                distance = findDistanceToNearestEdge(display, i, j, &minX, &minY);
                //distance = pow(b * distance, -1 * c);
                //int random = rand2() % 100 + 50;
                //printf("at: %d, %d, near: %d, %d, d: %f, f: %f, texId: %d\n", i, j, minX, minY, distance, display->indicationFactor[minY * display->xres + minX], display->texId[minY * display->xres + minX]);
                //distance = distance * display->indicationFactor[minY * display->xres + minX];
                //printf("%f\n", distance);
                //display->texId[sub] != 200
                //distance > display->indicationFactor[minY * display->xres + minX]
                if (distance > 4*display->indicationFactor[minY * display->xres + minX]) {
                    display->fbuf[sub].red = display->fbuf[sub].green =
                    display->fbuf[sub].blue = 4090;
                    //display->deletedTexId.push_back(display->texId[sub]);
                }
            }
        }
    }
    
    for (i = 0; i < display->xres; i++) {
        printf("i2: %d\n", i);
        for (j = 0; j < display->yres; j++) {
            sub = j * display->xres + i;
            pixelType = display->fbuf[sub].type;
            if (pixelType == ZBUFFER_TEX && display->fbuf[sub].red != 4090) {
                if (deletedTextureInNeighbourhood(display, i, j)) {
                    display->fbuf[sub].red = display->fbuf[sub].green =
                    display->fbuf[sub].blue = 4090;
                }
            }
        }
    }
}

void GzPrintTexId(GzDisplay *display)
{
    
    int i, j, sub, pixelType;
    
    for (i = 0; i < display->xres; i++) {
        for (j = 0; j < display->yres; j++) {
            sub = j * display->xres + i;
            pixelType = display->fbuf[sub].type;
            if (pixelType == ZBUFFER_TEX) {
                printf("at %d, %d. tex: %d\n", i, j, display->texId[sub]);
            }
        }
    }
}

void GzContourCompletion(GzDisplay *display)
{
    int i, j, k, sub, pixelType, pixelType2, sub2;
    bool draw;
    
    int dx[4] = {0, 0, 1, -1};
    int dy[4] = {1, -1, 0, 0};
    
    for (i = 1; i < display->xres - 1; i++) {
        for (j = 1; j < display->yres - 1; j++) {
            sub = j * display->xres + i;
            pixelType = display->fbuf[sub].type;
            if (pixelType == ZBUFFER_TEX) {
                draw = false;
                for (k = 0; k < 4; k++) {
                    sub2 = (j + dy[k]) * display->xres + i + dx[k];
                    pixelType2 = display->fbuf[sub2].type;
                    if (pixelType2 == ZBUFFER_BACKGROUND) {
                        draw = true;
                        break;
                    }
                }
                if (draw) {
                    display->fbuf[sub].red = display->fbuf[sub].green =
                    display->fbuf[sub].blue = 0;
                    display->fbuf[sub].type = ZBUFFER_EDGE;
                }
            }
        }
    }
}