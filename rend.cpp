#include	"gz.h"
#include	"rend.h"
#include    "disp.h"
#include    "mathLib.h"
#include    "test.h"
#include	<stdio.h>
#include    <stdlib.h>
#include	<math.h>
#include    <climits>

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
    (*render)->matlevel = 0;

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
            GzPoint vertexes[3];
            
            for (j = 0; j < 3; j++) {
                // Get the vertexes data.
                for (k = 0; k < 3; k++) {
                    vertexes[j][k] = ((float *)valueList[0])[3 * j + k];
                }
                vertexes[j][3] = 1;
                
                matrixMultiplyVector(render->Ximage[render->matlevel - 1], vertexes[j], vertexes[j]);
                // Convert the x,y,z to the screen space.
                for (k = 0; k < 3; k++) {
                    vertexes[j][k] = vertexes[j][k] / vertexes[j][3];
                }
                // printPoint(vertexes[j]);
                
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
            float normal[3];
            float vector1[3], vector2[3]; // the two vectors in the surface.
            float zValue;
            
            for (m = 0; m < 3; m++) {
                vector1[m] = vertexes[1][m] - vertexes[0][m];
                vector2[m] = vertexes[1][m] - vertexes[2][m];
            }

            crossProduct(vector1, vector2, normal);
            
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
                        D = -1 * dotProduct(normal, vertexes[0]);
                        zValue = (normal[0] * k + normal[1] * j + D) / (-1 * normal[2]);
                        // printf("z value: %f\n", zValue);
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
    int i, j;
    float r;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            mat[i][j] = 0;
    
    r = degree * PI / 180;
    mat[0][0] = mat[3][3] = 1;
    mat[1][1] = mat[2][2] = cos(r);
    mat[2][1] = sin(r);
    mat[1][2] = -1 * mat[2][1];
    
    return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
    // Create rotate matrix : rotate along y axis
    // Pass back the matrix using mat value
    int i, j;
    float r;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            mat[i][j] = 0;
    
    r = degree * PI / 180;
    mat[1][1] = mat[3][3] = 1;
    mat[0][0] = mat[2][2] = cos(r);
    mat[0][2] = sin(r);
    mat[2][0] = -1 * mat[0][2];
    
    return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
    // Create rotate matrix : rotate along z axis
    // Pass back the matrix using mat value
    int i, j;
    float r;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            mat[i][j] = 0;
    
    r = degree * PI / 180;
    mat[2][2] = mat[3][3] = 1;
    mat[0][0] = mat[1][1] = cos(r);
    mat[1][0] = sin(r);
    mat[0][1] = -1 * mat[1][0];
    
    return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
    // Create translation matrix
    // Pass back the matrix using mat value
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (j == 3) {
                if (i != 3) {
                    mat[i][j] = translate[i];
                }
                else {
                    mat[i][j] = 1;
                }
            }
            else {
                mat[i][j] = 0;
            }
        }
    }
    
    return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
    // Create scaling matrix
    // Pass back the matrix using mat value
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (i == j) {
                if (i != 3) {
                    mat[i][j] = scale[i];
                }
                else {
                    mat[i][j] = 1;
                }
            }
            else {
                mat[i][j] = 0;
            }
        }
    }
    
    return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
    /*
     - overwrite renderer camera structure with new camera definition
     */
    render->camera = *camera;
    
    // Calculate the world to camera matrix.
    GzVector Cz, Cy, Cx;
    // float cosZY;
    int i;
    
    // Find out the camera Z vector.
    for (i = 0; i < 3; i++) {
        Cz[i] = render->camera.lookat[i] - render->camera.position[i];
        Cy[i] = render->camera.worldup[i];
    }
    
    // Up vector in world space.
    normalization(Cz);
    normalization(Cy);
    float f1 = dotProduct(Cy, Cz);
    
    for (i = 0; i < 3; i++) {
        Cy[i] = Cy[i] - Cz[i] * f1;
    }
    normalization(Cy);
    crossProduct(Cy, Cz, Cx);
    normalization(Cx);
    
    float sum = 0;
    for (i = 0; i < 3; i++) {
        sum += Cy[i] * Cz[i];
    }
    // printf("sum: %f\n", sum);
    
    /*
    float unit;
    for (i = 0, unit = 0; i < 3; i++) {
        unit = Cz[i] / (camera->lookat[i] - camera->position[i]);
        printf("Unit: %f\n", unit);
    }
    */
    
    float temp[3];
    float negPos[3];
    for (i = 0; i < 3; i++) {
        negPos[i] = camera->position[i] * -1;
    }
    
    temp[0] = dotProduct(Cx, camera->position);
    temp[1] = dotProduct(Cy, camera->position);
    temp[2] = dotProduct(Cz, camera->position);
    
    GzMatrix a = {
        {Cx[0], Cx[1], Cx[2], -1 * temp[0]},
        {Cy[0], Cy[1], Cy[2], -1 * temp[1]},
        {Cz[0], Cz[1], Cz[2], -1 * temp[2]},
        {0, 0, 0, 1}};
    
    float D = tanf(render->camera.FOV / 2 * (PI / 180));
    // printf("FOV: %f, d: %f\n", render->camera.FOV, D);
    GzMatrix b = {{1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, D, 1}};
    
    GzMatrix temp2 = {
        {(float)render->display->xres/(float)2, 0, 0, (float)render->display->xres/(float)2},
        {0, -1 * (float)render->display->yres/(float)2, 0, (float)render->display->yres/(float)2},
        {0, 0, INT_MAX * D, 0},
        {0, 0, 0, 1}};
    
    int j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            render->camera.Xiw[i][j] = a[i][j];
            render->camera.Xpi[i][j] = b[i][j];
            render->Xsp[i][j] = temp2[i][j];
        }
    }
    
    GzPushMatrix(render, render->Xsp);
    GzPushMatrix(render, render->camera.Xpi);
    GzPushMatrix(render, render->camera.Xiw);
    
    // printMatrix(render->Xsp);
    
    return GZ_SUCCESS;
}

int GzPushMatrix(GzRender *render, GzMatrix matrix)
{
    /*
     - push a matrix onto the Ximage stack
     - check for stack overflow
     */
    if ((*render).matlevel == MATLEVELS)
        return GZ_FAILURE;
    
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            render->Ximage[render->matlevel][i][j] = matrix[i][j];
    
    if (render->matlevel > 0) {
        matrixMultiply(render->Ximage[render->matlevel - 1], render->Ximage[render->matlevel],
                       render->Ximage[render->matlevel]);
    }
    render->matlevel++;
    
    return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
    /*
     - pop a matrix off the Ximage stack
     - check for stack underflow
     */
    if (render->matlevel == 0) {
        return GZ_FAILURE;
    }
    
    render->matlevel--;
    
    return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
    return(short)((int)(color * ((1 << 12) - 1)));
}



