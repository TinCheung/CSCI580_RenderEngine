#include	"gz.h"
#include	"rend.h"
#include    "disp.h"
#include	<stdio.h>
#include	<math.h>

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay *display)
{
/* 
- malloc a renderer struct
- keep closed until BeginRender inits are done
- span interpolator needs pointer to display for pixel writes
- check for legal class GZ_Z_BUFFER_RENDER
*/
    *render = new GzRender;
    
    if (*render == NULL) return GZ_FAILURE;
    
    (*render)->renderClass = renderClass;
    (*render)->display = display;

	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
    //GzFreeDisplay(render->display);
    delete render;

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender	*render)
{
/* 
- set up for start of each frame - init frame buffer
*/
    GzInitDisplay(render->display);
    
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
    int i, j;
    for (i = 0; i < numAttributes; i++) {
        if (nameList[i] == GZ_RGB_COLOR) {
            for (j = 0; j < 3; j++)
                render->flatcolor[j] = ((float *)valueList[0])[j];
        }
    }

	return GZ_SUCCESS;
}


int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList) 
/* numParts - how many names and values */
{
/* 
- pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
- Invoke the scan converter and return an error code
*/
    int i;
    
    for (i = 0; i < numParts; i++) {
        if (nameList[i] == GZ_POSITION) {
            
            float maxX, maxY, minX, minY;
            maxX = maxY = -1;
            minX = MAXXRES + 1;
            minY = MAXYRES + 1;
            
            int j, k, m;
            float vertexes[3][3];
            
            // Get the vertexes data.
            for (j = 0; j < 3; j++) {
                for (k = 0; k < 3; k++) {
                    vertexes[j][k] = ((float *)valueList[0])[3 * j + k];
                }
                // Determine the rectangle we gonna to draw the triangle.
                maxX = vertexes[j][0] > maxX ? vertexes[j][0] : maxX;
                maxY = vertexes[j][1] > maxY ? vertexes[j][1] : maxY;
                minX = vertexes[j][0] < minX ? vertexes[j][0] : minX;
                minY = vertexes[j][1] < minY ? vertexes[j][1] : minY;
            }
            
            // We will check the points' sign in the line format
            // dy * (x - x0) + dx * (y0 - y)
            float dy[3], dx[3];
            float x0[3], y0[3];
            float result;
            int sub1, sub2;
            bool prevSign = true; // true for positive, false for negative
            bool draw;
            
            for (j = 0; j < 3; j++) {
                sub1 = j;
                sub2 = j + 1 > 2 ? 0 : j + 1;
                dx[j] = vertexes[sub1][0] - vertexes[sub2][0];
                dy[j] = vertexes[sub1][1] - vertexes[sub2][1];
                x0[j] = vertexes[sub1][0];
                y0[j] = vertexes[sub1][1];
            }
            
            // Derive the normal vector for the surface which the triangle locates.
            double normal[3];
            double vector1[3], vector2[3]; // the two vectors in the surface.
            float zValue;
            
            for (m = 0; m < 3; m++) {
                vector1[m] = vertexes[1][m] - vertexes[0][m];
                vector2[m] = vertexes[1][m] - vertexes[2][m];
            }
            
            normal[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1];
            normal[1] = vector1[2] * vector2[0] - vector1[0] * vector2[2];
            normal[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0];
            
            // Check the points and draw the triangle.
            for (j = minY; j <= maxY; j++) {
                for (k = minX; k <= maxX; k++) {
                    draw = true;
                    
                    for (m = 0; m < 3; m++) {
                        result = dy[m] * (k - x0[m]) + dx[m] * (y0[m] - j);
                        if (m == 0) {
                            prevSign = result > 0 ? true : false;
                        }
                        else {
                            if (prevSign && result < 0) {
                                draw = false;
                                break;
                            }
                            else if (!prevSign && result > 0) {
                                draw = false;
                                break;
                            }
                        }
                    }
                    
                    if (draw) {
                        // Get the z value.
                        float D;
                        D = -1 * (normal[0] * vertexes[0][0] + normal[1] * vertexes[0][1] + normal[2] * vertexes[0][2]);
                        zValue = (normal[0] * k + normal[1] * j + D) / (-1 * normal[2]);
                        
                        // draw the point
                        GzPutDisplay(render->display, k, j, render->flatcolor[0] * 4095, render->flatcolor[1] * 4095, render->flatcolor[2] * 4095, 0, zValue);
                    }
                }
            }
        }
        else if (nameList[i] == GZ_NULL_TOKEN) {
            // do nothing.
        }
    }

	return GZ_SUCCESS;
}

int GzRotXMat(float degree, GzMatrix mat)
{
    // Create rotate matrix : rotate along x axis
    // Pass back the matrix using mat value
    
    return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
    // Create rotate matrix : rotate along y axis
    // Pass back the matrix using mat value
    
    return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
    // Create rotate matrix : rotate along z axis
    // Pass back the matrix using mat value
    
    return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
    // Create translation matrix
    // Pass back the matrix using mat value
    
    return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
    // Create scaling matrix
    // Pass back the matrix using mat value
    
    return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions
int GzPutCamera(GzRender *render, GzCamera *camera)
{
    /*
     - overwrite renderer camera structure with new camera definition
     */
    return GZ_SUCCESS;
}

int GzPushMatrix(GzRender *render, GzMatrix matrix)
{
    /*
     - push a matrix onto the Ximage stack
     - check for stack overflow
     */
    return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
    /*
     - pop a matrix off the Ximage stack
     - check for stack underflow
     */
    return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
    return(short)((int)(color * ((1 << 12) - 1)));
}



