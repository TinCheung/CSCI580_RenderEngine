#include	"gz.h"
#include	"rend.h"
#include    "disp.h"
#include    "mathLib.h"
#include    "test.h"
#include	<stdio.h>
#include    <stdlib.h>
#include	<math.h>
#include    <climits>
#include    "stroke.h"
#include    <vector>
#include    "planarMap.h"

using namespace std;

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
    (*render)->numlights = 0;
    (*render)->tex_fun = NULL;

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
        switch (nameList[i]) {
            case GZ_RGB_COLOR:
                for (j = 0; j < 3; j++)
                    render->flatcolor[j] = ((float *)valueList[i])[j];
                break;
            case GZ_DIRECTIONAL_LIGHT:
                if (render->numlights == MAX_LIGHTS) {
                    return GZ_FAILURE;
                }
                else {
                    int lightSub = render->numlights;
                    for (int j = 0; j < 3; j++) {
                        render->lights[lightSub].direction[j] = ((GzLight *)valueList[i])->direction[j];
                    }
                    normalization(render->lights[lightSub].direction);
                    for (int j = 0; j < 3; j++) {
                        render->lights[lightSub].color[j] = ((GzLight *)valueList[i])->color[j];
                    }
                    render->numlights++;
                }
                break;
            case GZ_AMBIENT_LIGHT:
                for (int j = 0; j < 3; j++) {
                    render->ambientlight.direction[j] = ((GzLight *)valueList[i])->direction[j];
                    normalization(render->ambientlight.direction);
                }
                for (int j = 0; j < 3; j++) {
                    render->ambientlight.color[j] = ((GzLight *)valueList[i])->color[j];
                }
                break;
            case GZ_DIFFUSE_COEFFICIENT:
                for (int j = 0; j < 3; j++) {
                    render->Kd[j] = (*((GzCoord *)valueList[i]))[j];
                }
                break;
            case GZ_AMBIENT_COEFFICIENT:
                for (int j = 0; j < 3; j++) {
                    render->Ka[j] = (*((GzCoord *)valueList[i]))[j];
                }
                break;
            case GZ_SPECULAR_COEFFICIENT:
                for (int j = 0; j < 3; j++) {
                    render->Ks[j] = (*((GzCoord *)valueList[i]))[j];
                }
                break;
            case GZ_INTERPOLATE:
                render->interp_mode = *((int *)valueList[i]);
                break;
            case GZ_DISTRIBUTION_COEFFICIENT:
                render->spec = *((float *)valueList[i]);
                break;
            case GZ_TEXTURE_MAP:
                render->tex_fun = ((int(*)(float, float, float *))(valueList[i]));
                break;
            case GZ_AASHIFTX:
                render->antiAliasingOffsetX = *((float *)valueList[i]);
                break;
            case GZ_AASHIFTY:
                render->antiAliasingOffsetY = *((float *)valueList[i]);
                break;
            default:
                printf("unexpected attribute name.\n");
                return GZ_FAILURE;
                break;
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
            GzPoint vertexes[3], vertexNormal[3];
            GzColor vertexesColor[3];
            GzVector eyeVector = {0, 0, -1};
            float vertexUV[3][2];
            
            for (j = 0; j < 3; j++) {
                // Get the vertexes data.
                for (k = 0; k < 3; k++) {
                    vertexes[j][k] = ((float *)valueList[0])[3 * j + k];
                }
                vertexes[j][3] = 1;
                
                matrixMultiplyVector(render->Ximage[render->matlevel - 1], vertexes[j], vertexes[j]);
                //printVector(vertexes[j]);
                
                // Get the normal data.
                for (k = 0; k < 3; k++) {
                    vertexNormal[j][k] = ((float *)valueList[1])[3 * j + k];
                }
                vertexNormal[j][3] = 1;
                
                // Get the texture uv data.
                for (k = 0; k < 2; k++) {
                    vertexUV[j][k] = ((float *)valueList[2])[2 * j + k];
                }
                
                // Do the viewing transformation for the vertexes and the normals.
                matrixMultiplyVector(render->Xnorm[render->matlevel - 1], vertexNormal[j], vertexNormal[j]);
            }
            
            GzColor specularColor, diffuseColor, ambientColor;
            
            // Calculate the surface normal in the camera space.
            GzVector surfaceNormal;
            GzVector vector01, vector02;
            
            for (k = 0; k < 3; k++) {
                vector01[k] = vertexes[1][k] - vertexes[0][k];
                vector02[k] = vertexes[2][k] - vertexes[0][k];
            }
            crossProduct(vector01, vector02, surfaceNormal);
            
            // Convert the x,y,z to the screen space.
            for (j = 0; j < 3; j++) {
                for (k = 0; k < 3; k++) {
                    vertexes[j][k] = vertexes[j][k] / vertexes[j][3];
                }
                //printVector(vertexes[0]);
                
                // Add anti-aliasing offset
                //vertexes[j][0] += (float)(render->antiAliasingOffsetX); // (float)2 * render->display->xres;
                //vertexes[j][1] += render->antiAliasingOffsetY; // 2 * render->display->yres;
                
                // Determine the rectangle we gonna to draw the triangle.
                maxX = vertexes[j][0] > maxX ? vertexes[j][0] : maxX;
                maxY = vertexes[j][1] > maxY ? vertexes[j][1] : maxY;
                minX = vertexes[j][0] < minX ? vertexes[j][0] : minX;
                minY = vertexes[j][1] < minY ? vertexes[j][1] : minY;
            }
            
            // We will check the points' sign in the line format
            // dy * (x - x0) + dx * (y0 - y)
            float dy[3], dx[3], x0[3], y0[3], result;
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
            
            float zValue;
            
            // Warp the uv of vertexes
            for (int l = 0; l < 3; l++) {
                float Vz = vertexes[l][2] / (ZMAX - vertexes[l][2]);
                for (int m = 0; m < 2; m++) {
                    vertexUV[l][m] = vertexUV[l][m] / (Vz + 1);
                }
            }
            
            // For Ground Model, calculate the three vertexes' color.
            if (render->interp_mode == GZ_COLOR) {
                for (j = 0; j < 3; j++) {
                    GzColor textureColor;
                    render->tex_fun(vertexUV[j][0], vertexUV[j][1], textureColor);
                    
                    normalization(vertexNormal[j]);
                    
                    for (k = 0; k < 3; k++) {
                        render->Ka[k] = render->Kd[k] = render->Ks[k] = 1;
                        specularColor[k] = diffuseColor[k] = ambientColor[k] = vertexesColor[j][k] = 0;
                    }
                    
                    GzShadePoint(vertexesColor[j], vertexNormal[j], eyeVector, render);
                }
            }
            
            float normal[3];
            float vector1[3], vector2[3]; // the two vectors in the surface.
            
            for (m = 0; m < 3; m++) {
                vector1[m] = vertexes[1][m] - vertexes[0][m];
                vector2[m] = vertexes[1][m] - vertexes[2][m];
            }
            
            crossProduct(vector1, vector2, normal);
            
            float D;
            D = -1 * dotProduct(normal, vertexes[0]);
            
            //normalization(normal);
            
            printf("D: %f\n", D);
            printVector(normal);
            
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
                        GzPoint curPoint = {(float)k, (float)j, 0, 0};
                        GzPoint tempPoints[3];
                        GzVector comb;
                        GzColor textureColor;
                        
                        zValue = (normal[0] * k + normal[1] * j + D) / (-1 * normal[2]);
                        
                        for (int l = 0; l < 3; l++) {
                            tempPoints[l][0] = vertexes[l][0];
                            tempPoints[l][1] = vertexes[l][1];
                            tempPoints[l][2] = tempPoints[l][3] = 0;
                        }
                        
                        bilinearInterpolationInTriangle(curPoint, tempPoints[0], tempPoints[1], tempPoints[2], comb);
                        
                        // zValue = comb[0] * vertexes[0][2] + comb[1] * vertexes[1][2] + comb[2] * vertexes[2][2];
                        
                        // Texturing
                        float VzI = zValue/(ZMAX - zValue);
                        float curUV[2];
                        
                        // Interpolate and unwarp the current point UV
                        for (int l = 0; l < 2; l++) {
                            curUV[l] = comb[0] * vertexUV[0][l] + comb[1] * vertexUV[1][l] + comb[2] * vertexUV[2][l];
                            curUV[l] = curUV[l] * (VzI + 1);
                        }
                        
                        // Get the texture color.
                        //(*(render->tex_fun))(curUV[0], curUV[1], textureColor);
                        
                        // Shading.
                        GzColor pointColor;
                        // Ground Model shading.
                        if (render->interp_mode == GZ_COLOR) {
                            // Calculate the combination of the current point.
                            for (int l = 0; l < 3; l++) {
                                pointColor[l] = comb[0] * vertexesColor[0][l]
                                              + comb[1] * vertexesColor[1][l]
                                              + comb[2] * vertexesColor[2][l];
                                pointColor[l] = pointColor[l] * textureColor[l];
                            }
                            
                        }
                        // Phong Model shading.
                        else if (render->interp_mode == GZ_NORMALS) {
                            GzVector pointNormal;
                            
                            for (int l = 0; l < 3; l++)
                            {
                                render->Ka[l] = render->Kd[l] = textureColor[l];
                            }
                            
                            for (int l = 0; l < 3; l++) {
                                pointNormal[l] = comb[0] * vertexNormal[0][l]
                                               + comb[1] * vertexNormal[1][l]
                                               + comb[2] * vertexNormal[2][l];
                            }
                            GzShadePoint(pointColor, pointNormal, eyeVector, render);
                        }
                        
                        printf("x: %d, y: %d, z: %f\n", k, j, zValue);
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
    GzMatrix b = {{1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, D, 1}};
    
    GzMatrix temp2 = {
        {(float)render->display->xres/(float)2, 0, 0, (float)render->display->xres/(float)2},
        {0, -1 * (float)render->display->yres/(float)2, 0, (float)render->display->yres/(float)2},
        {0, 0, ZMAX * D, 0},
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
    
    printMatrix(render->Ximage[render->matlevel - 1]);
    
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
    //printf("Add: "); printMatrix(matrix);
    int i, j;
    bool pushNormalMatrix = true;
    bool toNormalized = false;
    if (isSameMatrix(matrix, render->Xsp)) {
        pushNormalMatrix = false;
    }
    else if (isSameMatrix(matrix, render->camera.Xpi)) {
        pushNormalMatrix = false;
    }
    else if (isSameMatrix(matrix, render->camera.Xiw)) {
        pushNormalMatrix = true;
    }
    else if (isScaleMatrix(matrix)) {
        pushNormalMatrix = false;
    }
    else if (isTranslateMatrix(matrix)) {
        pushNormalMatrix = false;
    }
    else {
        pushNormalMatrix = true;
        toNormalized = true;
    }
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++) {
            render->Ximage[render->matlevel][i][j] = matrix[i][j];
            if (pushNormalMatrix && i < 3 && j < 3)
                render->Xnorm[render->matlevel][i][j] = matrix[i][j];
            else
                render->Xnorm[render->matlevel][i][j] = i == j ? 1 : 0;
        }
    
    if (render->matlevel > 0) {
        matrixMultiply(render->Ximage[render->matlevel - 1], render->Ximage[render->matlevel],
                       render->Ximage[render->matlevel]);
        matrixMultiply(render->Xnorm[render->matlevel - 1], render->Xnorm[render->matlevel],
                       render->Xnorm[render->matlevel]);
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

void GzShadePoint(GzColor pointColor, GzVector normal, GzVector eyeVector, GzRender *render)
{
    GzColor specularColor, diffuseColor, ambientColor;
    normalization(normal);
    float dotProductNL, dotProductNE;
    
    int k;
    for (k = 0; k < 3; k++) {
        specularColor[k] = diffuseColor[k] = ambientColor[k] = pointColor[k] = 0;
    }
    
    for (k = 0; k < render->numlights; k++) {
        GzLight light = render->lights[k];
        GzLight reflectLight;
        
        dotProductNL = dotProduct(light.direction, normal);
        
        if (dotProductNL < 0) {
            for (int m = 0; m < 3; m++) normal[m] *= -1;
            dotProductNL = dotProduct(light.direction, normal);
        }
        
        dotProductNE = dotProduct(normal, eyeVector);
        
        if (dotProductNE < 0) {
            for (int m = 0; m < 3; m++) normal[m] *= -1;
            dotProductNE = dotProduct(normal, eyeVector);
        }
        
        if (dotProductNL * dotProductNE < 0) {
            continue;
        }
        if (dotProductNL < 0 && dotProductNE < 0)
            for (int m = 0; m < 3; m++) normal[m] *= -1;
        
        normalization(normal);
        normalization(light.direction);
        normalization(eyeVector);
        
        getReflectLightDirection(light.direction, normal, reflectLight.direction);
        normalization(reflectLight.direction);
        addColorInLight(specularColor, light, render->Ks, reflectLight.direction,
                        eyeVector, render->spec);
        addColorInLight(diffuseColor, light, render->Kd, normal, light.direction, 1);
        
        for (int l = 0; l < 3; l++) {
            pointColor[l] += (diffuseColor[l] * 1 + specularColor[l] * 1);
        }
    }
    
    for (k = 0; k < 3; k++)
        pointColor[k] += (render->ambientlight.color[k] * render->Ka[k] * 1);
}

int GzPenInkRender(GzRender *render, int triangleNum, GzTriangle triangles[])
{
    int t;
    
    // Record the front triangle for each pixel.
    for (t = 0; t < triangleNum; t++)
    {
        printf("tri: %d\n", t);
        float maxX, maxY, minX, minY, vertexUV[3][2];
        maxX = maxY = -1;
        minX = MAXXRES + 1;
        minY = MAXYRES + 1;
        
        int j, k, m;
        GzPoint vertexes[3], vertexNormal[3];;
        
        for (j = 0; j < 3; j++) {
            // Get the vertexes data.
            for (k = 0; k < 3; k++) {
                vertexes[j][k] = triangles[t].vertexes[j][k];
            }
            vertexes[j][3] = 1;
            
            matrixMultiplyVector(render->Ximage[render->matlevel - 1], vertexes[j], vertexes[j]);
            //printVector(vertexes[j]);
            
            // Get the normal data.
            for (k = 0; k < 3; k++) {
                vertexNormal[j][k] = triangles[t].vertexesNormals[j][k];
            }
            vertexNormal[j][3] = 1;
            
            // Get the texture uv data.
            for (k = 0; k < 2; k++) {
                vertexUV[j][k] = triangles[t].uv[j][k];
            }
            matrixMultiplyVector(render->Xnorm[render->matlevel - 1], vertexNormal[j], vertexNormal[j]);
        }
        
        for(j = 0; j < 3; j++) {
            for (k = 0; k < 3; k++) {
                vertexes[j][k] = vertexes[j][k] / vertexes[j][3];
                triangles[t].vertexesInScreen[j][k] = vertexes[j][k];
            }
            
            // Determine the triangle region.
            maxX = vertexes[j][0] > maxX ? vertexes[j][0] : maxX;
            maxY = vertexes[j][1] > maxY ? vertexes[j][1] : maxY;
            minX = vertexes[j][0] < minX ? vertexes[j][0] : minX;
            minY = vertexes[j][1] < minY ? vertexes[j][1] : minY;
            //printVector(vertexes[j]);
        }
        
        float vector1[3], vector2[3]; // the two vectors in the surface.
        
        for (m = 0; m < 3; m++) {
            vector1[m] = vertexes[1][m] - vertexes[0][m];
            vector2[m] = vertexes[1][m] - vertexes[2][m];
        }
        
        crossProduct(vector1, vector2, triangles[t].normal);
        // normalization(triangles[t].normal);
        triangles[t].D = -1 * dotProduct(triangles[t].normal, vertexes[0]);
        
        // Calculate the tone of the triangle.
        GzVector normal;
        GzVector eyeVector = {0, 0, -1};
        GzColor tone = {1, 1, 1};
        
        for (j = 0; j < 3; j++)
            normal[j] = triangles[t].normal[j];
        
        GzShadePoint(tone, normal, eyeVector, render);
        triangles[t].tone = tone[0];
        // printf("tone: %f\n", triangles[t].tone);
        
        // Draw the textures.
        float dy[3], dx[3], x0[3], y0[3], result;
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
        
        float zValue;
        
        // Warp the uv of vertexes
        for (int l = 0; l < 3; l++) {
            float Vz = vertexes[l][2] / (ZMAX - vertexes[l][2]);
            for (int m = 0; m < 2; m++) {
                vertexUV[l][m] = vertexUV[l][m] / (Vz + 1);
            }
        }
        
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
                    GzPoint curPoint = {(float)k, (float)j, 0, 0};
                    GzPoint tempPoints[3];
                    GzVector comb;
                    GzColor textureColor;
        
                    zValue = getZValue(triangles[t].normal, triangles[t].D, k, j);
                    
                    for (int l = 0; l < 3; l++) {
                        tempPoints[l][0] = vertexes[l][0];
                        tempPoints[l][1] = vertexes[l][1];
                        tempPoints[l][2] = tempPoints[l][3] = 0;
                    }
                    
                    bilinearInterpolationInTriangle(curPoint, tempPoints[0], tempPoints[1], tempPoints[2], comb);
                    
                    // Texturing
                    float VzI = zValue/(ZMAX - zValue);
                    float curUV[2];
                    
                    // Interpolate and unwarp the current point UV
                    for (int l = 0; l < 2; l++) {
                        curUV[l] = comb[0] * vertexUV[0][l] + comb[1] * vertexUV[1][l] + comb[2] * vertexUV[2][l];
                        curUV[l] = curUV[l] * (VzI + 1);
                    }
                    
                    // Get the texture color.
                    GzVector pointNormal;
                    
                    for (int l = 0; l < 3; l++) {
                        pointNormal[l] = comb[0] * vertexNormal[0][l]
                        + comb[1] * vertexNormal[1][l]
                        + comb[2] * vertexNormal[2][l];
                    }
                    GzShadePoint(tone, pointNormal, eyeVector, render);
                    
                    textureColor[0] = tone[0];
                    if (isnan(curUV[0]) || isnan(curUV[1]))
                    {
                        textureColor[0] = textureColor[1] = textureColor[2] = 1;
                    }
                    //printf ("x: %d, y: %d, u: %f, v: %f, Vzi: %f\n", k, j, curUV[0], curUV[1], VzI);
                    else {
                        (*(render->tex_fun))(curUV[0], curUV[1], textureColor);
                    }
                    
                    // for (int q = 0; q < 3; q++) textureColor[q] = triangles[t].tone;
                    
                    //printf("x: %d, y: %d, z: %f\n", k, j, zValue);
                    // draw the point
                    GzPutDisplayExt(render->display, k, j, textureColor[0] * 4095, textureColor[0] * 4095, textureColor[0] * 4095, 0, zValue, triangles[t].triangleId, ZBUFFER_TEX);
                    //GzPutDisplay(render->display, k, j, textureColor[0] * 4095, textureColor[0] * 4095, textureColor[0] * 4095, 0, zValue);
                }
            }
        }
    }
    
    // Get the triangles visibility.
    bool *visibity = new bool[triangleNum];
    
    GzGetTrianglesVisibility(render->display, triangleNum, visibity);
    
    vector<Edge> edges;
    getEdgesFromTriangles(triangles, triangleNum, &edges);
    
    for (int i = 0; i < edges.size(); i++) {
        //if (checkEdgeTonesDiff(triangles, triangleNum, edges[i])) drawEdge(render->display, edges[i], 2);
        if (!isAllVisible(triangles, triangleNum, visibity, edges[i]))
            drawEdge(render->display, edges[i], 2);
    }
    
    // GzIndication(render->display);
    
    return GZ_SUCCESS;
}
