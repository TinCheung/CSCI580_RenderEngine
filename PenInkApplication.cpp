//
//  PenInkApplication.cpp
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 15/4/14.
//  Copyright (c) 2015年 tantianx. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "PenInkApplication.h"
#include "mathLib.h"
#include <vector>

#define OUTFILE2 "./PenInk.ppm"
#define INFILE3  "pot4.asc"

extern int white_tex_fun(float u, float v, GzColor color);
extern int shadow_fun(float u, float v, GzColor color);
extern int brick_fun(float u, float v, GzColor color);
void setCube(GzTriangle triangles[]);

using namespace std;

int PenInkApplication::run()
{
    vector<GzTriangle> tempTriangles;
    GzTriangle *triangles;
    
    
    int width = 1024;    // frame buffer and display width
    int height = 1024;   // frame buffer and display height
    int	xRes, yRes, dispClass;	/* display parameters */
    int status;
    
    GzMatrix	translateAndScale =
    {
        3.25,	0.0,	0.0,	0.0,
        0.0,	3.25,	0.0,	-3.25,
        0.0,	0.0,	3.25,	3.5,
        0.0,	0.0,	0.0,	1.0
    };
    
    GzMatrix	rotateX =
    {
        1.0,	0.0,	0.0,	0.0,
        0.0,	.7071,	.7071,	0.0,
        0.0,	-.7071,	.7071,	0.0,
        0.0,	0.0,	0.0,	1.0
    };
    
    GzMatrix	rotateY =
    {
        .866,	0.0,	-0.5,	0.0,
        0.0,	1.0,	0.0,	0.0, 
        0.5,	0.0,	.866,	0.0, 
        0.0,	0.0,	0.0,	1.0 
    };
    
    status = 0;
    
    status |= GzNewDisplay(&display, GZ_RGBAZ_DISPLAY, width, height);
    status |= GzGetDisplayParams(display, &xRes, &yRes, &dispClass);
    status |= GzInitDisplay(display);
    status |= GzNewRender(&render, GZ_Z_BUFFER_RENDER, display);
    
#if 1 	/* set up app-defined camera if desired, else use camera defaults */
    
    camera.position[X] = 2;
    camera.position[Y] = 2;
    camera.position[Z] = 2;
    
    camera.lookat[X] = 0;
    camera.lookat[Y] = 0;
    camera.lookat[Z] = 0;
    
    camera.worldup[X] = 1;
    camera.worldup[Y] = 0;
    camera.worldup[Z] = 0;
    /*
    camera.position[X] = 13.2;
    camera.position[Y] = -8.7;
    camera.position[Z] = -14.8;
    
    camera.lookat[X] = 0.8;
    camera.lookat[Y] = 0.7;
    camera.lookat[Z] = 4.5;
    
    camera.worldup[X] = -0.2;
    camera.worldup[Y] = 1.0;
    camera.worldup[Z] = 0.0;
    */
    camera.FOV = 53.7;              /* degrees */
    
    status |= GzPutCamera(render, &camera);
#endif
    /*
    status |= GzPushMatrix(render, translateAndScale);
    status |= GzPushMatrix(render, rotateY);
    status |= GzPushMatrix(render, rotateX);
    */
    for (int i = 0; i < 3; i++)
        render->flatcolor[i] = 0;
    
    // render->tex_fun = white_tex_fun;
    // render->tex_fun = shadow_fun;
    render->tex_fun = brick_fun;
    
    // set up the lights.
    GzLight	light1 = { {-1, -1, -1}, {0.9, 0.9, 0.9} };
    render->lights[0] = light1;
    render->numlights = 1;
    
    /* Material property */
    GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
    GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
    GzColor diffuseCoefficient = {0.7, 0.7, 0.7};
    
    FILE *infile;
    if( (infile  = fopen( INFILE3 , "r" )) == NULL )
    {
        printf("Could not open input from %s \n", INFILE3);
        return GZ_FAILURE;
    }
    
    for (int i = 0; i < 3; i++) {
        render->Kd[i] = diffuseCoefficient[i];
        render->Ka[i] = ambientCoefficient[i];
        render->Ks[i] = specularCoefficient[i];
    }
    
    
    FILE *outfile;
    if( (outfile  = fopen( OUTFILE2 , "wb" )) == NULL )
    {
        printf("Could not open output file for writing %s \n", OUTFILE2);
        return GZ_FAILURE;
    }
    
    GzCoord		vertexList[3];		/* vertex position coordinates */
    GzCoord		normalList[3];		/* vertex normals */
    GzTextureIndex	uvList[3];		/* vertex texture map indices */
    char		dummy[256];
    Triangle t;
    
    while( fscanf(infile, "%s", dummy) == 1) { 	/* read in tri word */
        fscanf(infile, "%f %f %f %f %f %f %f %f",
               &(vertexList[0][0]), &(vertexList[0][1]),
               &(vertexList[0][2]),
               &(normalList[0][0]), &(normalList[0][1]),
               &(normalList[0][2]),
               &(uvList[0][0]), &(uvList[0][1]) );
        fscanf(infile, "%f %f %f %f %f %f %f %f",
               &(vertexList[1][0]), &(vertexList[1][1]),
               &(vertexList[1][2]),
               &(normalList[1][0]), &(normalList[1][1]),
               &(normalList[1][2]),
               &(uvList[1][0]), &(uvList[1][1]) );
        fscanf(infile, "%f %f %f %f %f %f %f %f",
               &(vertexList[2][0]), &(vertexList[2][1]),
               &(vertexList[2][2]),
               &(normalList[2][0]), &(normalList[2][1]),
               &(normalList[2][2]),
               &(uvList[2][0]), &(uvList[2][1]));

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                t.vertexes[i][j] = vertexList[i][j];
                t.vertexesNormals[i][j] = normalList[i][j];
                if (j < 2)
                    t.uv[i][j] = uvList[i][j];
            }
            t.vertexes[i][3] = 1;
        }
        tempTriangles.push_back(t);
    }
    
    triangles = new Triangle[tempTriangles.size()];
    for (int p = 0; p < tempTriangles.size(); p++) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                triangles[p].vertexes[i][j] = tempTriangles[p].vertexes[i][j];
                triangles[p].vertexesNormals[i][j] = tempTriangles[p].vertexesNormals[i][j];
                if (j < 2)
                    triangles[p].uv[i][j] = tempTriangles[p].uv[i][j];
            }
            t.vertexes[i][3] = 1;
        }
        triangles[p].triangleId = p;
    }
    
    render->flatcolor[0] = render->flatcolor[1] = render->flatcolor[2] = 0.5;
    
    if (status) exit(GZ_FAILURE);
    //GzPenInkRender(render, (int)tempTriangles.size(), triangles);
    
    setCube(triangles);
    GzPenInkRender(render, 12, triangles);
    
    GzFlushDisplay2File(outfile, display);
    
    if( fclose( outfile ) )
        printf( "The output file was not closed\n" );
    
    status |= GzFreeRender(render);
    status |= GzFreeDisplay(display);
    
    printf("Complete.\n");
    
    if (status)
        return(GZ_FAILURE);
    else 
        return(GZ_SUCCESS);
    
    return GZ_SUCCESS;
}

#define GROUND 5
void setCube(GzTriangle triangles[])
{
    // Draw a square.
    float v1[4] = {1, 0, 0, 1};
    float v2[4] = {0, 0, 0, 1};
    float v3[4] = {1, 1, 0, 1};
    float v4[4] = {0, 1, 0, 1};
    float v5[4] = {1, 0, 1, 1};
    float v6[4] = {0, 0, 1, 1};
    float v7[4] = {1, 1, 1, 1};
    float v8[4] = {0, 1, 1, 1};
    float v9[4] = {0, GROUND, GROUND, 1};
    float v10[4] = {0, GROUND, -0, 1};
    float v11[4] = {0, -GROUND, GROUND, 1};
    float v12[4] = {0, -0, -0, 1};
    
    setPoint(triangles[0].vertexes[0], v1);
    setPoint(triangles[0].vertexes[1], v2);
    setPoint(triangles[0].vertexes[2], v3);
    
    triangles[0].uv[0][0] = 0; triangles[0].uv[0][1] = 0;
    triangles[0].uv[1][0] = 0; triangles[0].uv[1][1] = 1;
    triangles[0].uv[2][0] = 1; triangles[0].uv[2][1] = 0;
    
    triangles[0].vertexesNormals[0][0] = 0.5;
    triangles[0].vertexesNormals[0][1] = -0.5;
    triangles[0].vertexesNormals[0][2] = -0.5;
    
    triangles[0].vertexesNormals[1][0] = -0.5;
    triangles[0].vertexesNormals[1][1] = -0.5;
    triangles[0].vertexesNormals[1][2] = -0.5;
    
    triangles[0].vertexesNormals[2][0] = 0.5;
    triangles[0].vertexesNormals[2][1] = 0.5;
    triangles[0].vertexesNormals[2][2] = -0.5;
    
    setPoint(triangles[1].vertexes[0], v2);
    setPoint(triangles[1].vertexes[1], v3);
    setPoint(triangles[1].vertexes[2], v4);
    
    triangles[1].uv[0][0] = 0; triangles[1].uv[0][1] = 1;
    triangles[1].uv[1][0] = 1; triangles[1].uv[1][1] = 0;
    triangles[1].uv[2][0] = 1; triangles[1].uv[2][1] = 1;
    
    triangles[1].vertexesNormals[0][0] = -0.5;
    triangles[1].vertexesNormals[0][1] = -0.5;
    triangles[1].vertexesNormals[0][2] = -0.5;
    
    triangles[1].vertexesNormals[1][0] = 0.5;
    triangles[1].vertexesNormals[1][1] = 0.5;
    triangles[1].vertexesNormals[1][2] = -0.5;
    
    triangles[1].vertexesNormals[2][0] = -0.5;
    triangles[1].vertexesNormals[2][1] = 0.5;
    triangles[1].vertexesNormals[2][2] = -0.5;
    
    setPoint(triangles[2].vertexes[0], v1);
    setPoint(triangles[2].vertexes[1], v3);
    setPoint(triangles[2].vertexes[2], v5);
    
    triangles[2].vertexesNormals[0][0] = 0.5;
    triangles[2].vertexesNormals[0][1] = -0.5;
    triangles[2].vertexesNormals[0][2] = -0.5;
    
    triangles[2].vertexesNormals[1][0] = 0.5;
    triangles[2].vertexesNormals[1][1] = 0.5;
    triangles[2].vertexesNormals[1][2] = -0.5;
    
    triangles[2].vertexesNormals[2][0] = 0.5;
    triangles[2].vertexesNormals[2][1] = -0.5;
    triangles[2].vertexesNormals[2][2] = 0.5;
    
    triangles[2].uv[0][0] = 0; triangles[2].uv[0][1] = 0;
    triangles[2].uv[1][0] = 1; triangles[2].uv[1][1] = 0;
    triangles[2].uv[2][0] = 0; triangles[2].uv[2][1] = 1;
    
    setPoint(triangles[3].vertexes[0], v3);
    setPoint(triangles[3].vertexes[1], v5);
    setPoint(triangles[3].vertexes[2], v7);
    
    triangles[3].uv[0][0] = 1; triangles[3].uv[0][1] = 0;
    triangles[3].uv[1][0] = 0; triangles[3].uv[1][1] = 1;
    triangles[3].uv[2][0] = 1; triangles[3].uv[2][1] = 1;
    
    triangles[3].vertexesNormals[0][0] = 0.5;
    triangles[3].vertexesNormals[0][1] = 0.5;
    triangles[3].vertexesNormals[0][2] = -0.5;
    
    triangles[3].vertexesNormals[1][0] = 0.5;
    triangles[3].vertexesNormals[1][1] = -0.5;
    triangles[3].vertexesNormals[1][2] = 0.5;
    
    triangles[3].vertexesNormals[2][0] = 0.5;
    triangles[3].vertexesNormals[2][1] = 0.5;
    triangles[3].vertexesNormals[2][2] = 0.5;
    
    setPoint(triangles[4].vertexes[0], v3);
    setPoint(triangles[4].vertexes[1], v4);
    setPoint(triangles[4].vertexes[2], v7);

    triangles[4].vertexesNormals[0][0] = 0.5;
    triangles[4].vertexesNormals[0][1] = 0.5;
    triangles[4].vertexesNormals[0][2] = -0.5;
    
    triangles[4].vertexesNormals[1][0] = -0.5;
    triangles[4].vertexesNormals[1][1] = 0.5;
    triangles[4].vertexesNormals[1][2] = -0.5;
    
    triangles[4].vertexesNormals[2][0] = 0.5;
    triangles[4].vertexesNormals[2][1] = 0.5;
    triangles[4].vertexesNormals[2][2] = 0.5;
    
    triangles[4].uv[0][0] = 0; triangles[4].uv[0][1] = 0;
    triangles[4].uv[1][0] = 1; triangles[4].uv[1][1] = 0;
    triangles[4].uv[2][0] = 0; triangles[4].uv[2][1] = 1;
    
    setPoint(triangles[5].vertexes[0], v4);
    setPoint(triangles[5].vertexes[1], v7);
    setPoint(triangles[5].vertexes[2], v8);

    triangles[5].vertexesNormals[0][0] = -0.5;
    triangles[5].vertexesNormals[0][1] = 0.5;
    triangles[5].vertexesNormals[0][2] = -0.5;
    
    triangles[5].vertexesNormals[1][0] = 0.5;
    triangles[5].vertexesNormals[1][1] = 0.5;
    triangles[5].vertexesNormals[1][2] = 0.5;
    
    triangles[5].vertexesNormals[2][0] = -0.5;
    triangles[5].vertexesNormals[2][1] = 0.5;
    triangles[5].vertexesNormals[2][2] = 0.5;
    
    triangles[5].uv[0][0] = 1; triangles[5].uv[0][1] = 0;
    triangles[5].uv[1][0] = 0; triangles[5].uv[1][1] = 1;
    triangles[5].uv[2][0] = 1; triangles[5].uv[2][1] = 1;
    
    setPoint(triangles[6].vertexes[0], v4);
    setPoint(triangles[6].vertexes[1], v2);
    setPoint(triangles[6].vertexes[2], v8);

    triangles[6].vertexesNormals[0][0] = -0.5;
    triangles[6].vertexesNormals[0][1] = 0.5;
    triangles[6].vertexesNormals[0][2] = -0.5;
    
    triangles[6].vertexesNormals[1][0] = -0.5;
    triangles[6].vertexesNormals[1][1] = -0.5;
    triangles[6].vertexesNormals[1][2] = -0.5;
    
    triangles[6].vertexesNormals[2][0] = -0.5;
    triangles[6].vertexesNormals[2][1] = 0.5;
    triangles[6].vertexesNormals[2][2] = 0.5;
    
    triangles[6].uv[0][0] = 0; triangles[6].uv[0][1] = 0;
    triangles[6].uv[1][0] = 1; triangles[6].uv[1][1] = 0;
    triangles[6].uv[2][0] = 0; triangles[6].uv[2][1] = 1;
    
    setPoint(triangles[7].vertexes[0], v2);
    setPoint(triangles[7].vertexes[1], v8);
    setPoint(triangles[7].vertexes[2], v6);
    
    triangles[7].vertexesNormals[0][0] = -0.5;
    triangles[7].vertexesNormals[0][1] = -0.5;
    triangles[7].vertexesNormals[0][2] = -0.5;
    
    triangles[7].vertexesNormals[1][0] = -0.5;
    triangles[7].vertexesNormals[1][1] = 0.5;
    triangles[7].vertexesNormals[1][2] = 0.5;
    
    triangles[7].vertexesNormals[2][0] = -0.5;
    triangles[7].vertexesNormals[2][1] = -0.5;
    triangles[7].vertexesNormals[2][2] = 0.5;

    triangles[7].uv[0][0] = 1; triangles[7].uv[0][1] = 0;
    triangles[7].uv[1][0] = 0; triangles[7].uv[1][1] = 1;
    triangles[7].uv[2][0] = 1; triangles[7].uv[2][1] = 1;
    
    setPoint(triangles[8].vertexes[0], v5);
    setPoint(triangles[8].vertexes[1], v7);
    setPoint(triangles[8].vertexes[2], v6);

    triangles[8].vertexesNormals[0][0] = 0.5;
    triangles[8].vertexesNormals[0][1] = -0.5;
    triangles[8].vertexesNormals[0][2] = 0.5;
    
    triangles[8].vertexesNormals[1][0] = 0.5;
    triangles[8].vertexesNormals[1][1] = 0.5;
    triangles[8].vertexesNormals[1][2] = 0.5;
    
    triangles[8].vertexesNormals[2][0] = -0.5;
    triangles[8].vertexesNormals[2][1] = -0.5;
    triangles[8].vertexesNormals[2][2] = 0.5;
    
    triangles[8].uv[0][0] = 0; triangles[8].uv[0][1] = 0;
    triangles[8].uv[1][0] = 0; triangles[8].uv[1][1] = 1;
    triangles[8].uv[2][0] = 1; triangles[8].uv[2][1] = 0;
    
    setPoint(triangles[9].vertexes[0], v7);
    setPoint(triangles[9].vertexes[1], v6);
    setPoint(triangles[9].vertexes[2], v8);

    triangles[9].vertexesNormals[0][0] = 0.5;
    triangles[9].vertexesNormals[0][1] = 0.5;
    triangles[9].vertexesNormals[0][2] = 0.5;
    
    triangles[9].vertexesNormals[1][0] = -0.5;
    triangles[9].vertexesNormals[1][1] = -0.5;
    triangles[9].vertexesNormals[1][2] = 0.5;
    
    triangles[9].vertexesNormals[2][0] = -0.5;
    triangles[9].vertexesNormals[2][1] = 0.5;
    triangles[9].vertexesNormals[2][2] = 0.5;
    
    triangles[9].uv[0][0] = 0; triangles[9].uv[0][1] = 1;
    triangles[9].uv[1][0] = 1; triangles[9].uv[1][1] = 0;
    triangles[9].uv[2][0] = 1; triangles[9].uv[2][1] = 1;
    
    setPoint(triangles[10].vertexes[0], v1);
    setPoint(triangles[10].vertexes[1], v2);
    setPoint(triangles[10].vertexes[2], v5);

    triangles[10].vertexesNormals[0][0] = 0.5;
    triangles[10].vertexesNormals[0][1] = -0.5;
    triangles[10].vertexesNormals[0][2] = -0.5;
    
    triangles[10].vertexesNormals[1][0] = -0.5;
    triangles[10].vertexesNormals[1][1] = -0.5;
    triangles[10].vertexesNormals[1][2] = -0.5;
    
    triangles[10].vertexesNormals[2][0] = 0.5;
    triangles[10].vertexesNormals[2][1] = -0.5;
    triangles[10].vertexesNormals[2][2] = 0.5;
    
    triangles[10].uv[0][0] = 0; triangles[10].uv[0][1] = 0;
    triangles[10].uv[1][0] = 0; triangles[10].uv[1][1] = 1;
    triangles[10].uv[2][0] = 1; triangles[10].uv[2][1] = 0;
    
    setPoint(triangles[11].vertexes[0], v2);
    setPoint(triangles[11].vertexes[1], v5);
    setPoint(triangles[11].vertexes[2], v6);

    triangles[11].vertexesNormals[0][0] = -0.5;
    triangles[11].vertexesNormals[0][1] = -0.5;
    triangles[11].vertexesNormals[0][2] = -0.5;
    
    triangles[11].vertexesNormals[1][0] = 0.5;
    triangles[11].vertexesNormals[1][1] = -0.5;
    triangles[11].vertexesNormals[1][2] = 0.5;
    
    triangles[11].vertexesNormals[2][0] = -0.5;
    triangles[11].vertexesNormals[2][1] = -0.5;
    triangles[11].vertexesNormals[2][2] = 0.5;
    
    triangles[11].uv[0][0] = 0; triangles[11].uv[0][1] = 1;
    triangles[11].uv[1][0] = 1; triangles[11].uv[1][1] = 0;
    triangles[11].uv[2][0] = 1; triangles[11].uv[2][1] = 1;
    
    setPoint(triangles[12].vertexes[0], v9);
    setPoint(triangles[12].vertexes[1], v10);
    setPoint(triangles[12].vertexes[2], v12);
    
    triangles[12].uv[0][0] = 0; triangles[10].uv[0][1] = 0;
    triangles[12].uv[1][0] = 0; triangles[10].uv[1][1] = 1;
    triangles[12].uv[2][0] = 1; triangles[10].uv[2][1] = 1;
    
    setPoint(triangles[13].vertexes[0], v9);
    setPoint(triangles[13].vertexes[1], v11);
    setPoint(triangles[13].vertexes[2], v12);
    
    triangles[13].uv[0][0] = 0; triangles[11].uv[0][1] = 0;
    triangles[13].uv[1][0] = 1; triangles[11].uv[1][1] = 0;
    triangles[13].uv[2][0] = 1; triangles[11].uv[2][1] = 1;
    
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 3; j++)normalization(triangles[i].vertexesNormals[j]);
        triangles[i].triangleId = i;
    }
}