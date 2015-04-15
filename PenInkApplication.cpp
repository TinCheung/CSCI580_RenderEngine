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

#define OUTFILE2 "./PenInk.ppm"

void setCube(GzTriangle triangles[]);

int PenInkApplication::run()
{
    GzTriangle triangles[12];
    setCube(triangles);
    
    int width = 1024;    // frame buffer and display width
    int height = 1024;   // frame buffer and display height
    int	xRes, yRes, dispClass;	/* display parameters */
    int status;
    
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
    
    camera.FOV = 53.7;              /* degrees */
    
    status |= GzPutCamera(render, &camera);
#endif
    
    for (int i = 0; i < 3; i++)
        render->flatcolor[i] = 0;
    
    if (status) exit(GZ_FAILURE); 
    GzPenInkRender(render, 12, triangles);
    
    FILE *outfile;
    if( (outfile  = fopen( OUTFILE2 , "wb" )) == NULL )
    {
        printf("Could not open output file for writing %s \n", OUTFILE2);
        return GZ_FAILURE;
    }
    
    GzFlushDisplay2File(outfile, display);
    
    if( fclose( outfile ) )
        printf( "The output file was not closed\n" );
    
    status |= GzFreeRender(render);
    status |= GzFreeDisplay(display);
    
    if (status)
        return(GZ_FAILURE);
    else 
        return(GZ_SUCCESS);
    
    return GZ_SUCCESS;
}

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
    
    setPoint(triangles[0].vertexes[0], v1);
    setPoint(triangles[0].vertexes[1], v2);
    setPoint(triangles[0].vertexes[2], v3);
    
    setPoint(triangles[1].vertexes[0], v2);
    setPoint(triangles[1].vertexes[1], v3);
    setPoint(triangles[1].vertexes[2], v4);
    
    setPoint(triangles[2].vertexes[0], v1);
    setPoint(triangles[2].vertexes[1], v3);
    setPoint(triangles[2].vertexes[2], v5);
    
    setPoint(triangles[3].vertexes[0], v3);
    setPoint(triangles[3].vertexes[1], v5);
    setPoint(triangles[3].vertexes[2], v7);
    
    setPoint(triangles[4].vertexes[0], v3);
    setPoint(triangles[4].vertexes[1], v4);
    setPoint(triangles[4].vertexes[2], v7);
    
    setPoint(triangles[5].vertexes[0], v4);
    setPoint(triangles[5].vertexes[1], v7);
    setPoint(triangles[5].vertexes[2], v8);
    
    setPoint(triangles[6].vertexes[0], v4);
    setPoint(triangles[6].vertexes[1], v2);
    setPoint(triangles[6].vertexes[2], v8);
    
    setPoint(triangles[7].vertexes[0], v2);
    setPoint(triangles[7].vertexes[1], v8);
    setPoint(triangles[7].vertexes[2], v6);
    
    setPoint(triangles[8].vertexes[0], v5);
    setPoint(triangles[8].vertexes[1], v7);
    setPoint(triangles[8].vertexes[2], v6);
    
    setPoint(triangles[9].vertexes[0], v7);
    setPoint(triangles[9].vertexes[1], v6);
    setPoint(triangles[9].vertexes[2], v8);
    
    setPoint(triangles[10].vertexes[0], v1);
    setPoint(triangles[10].vertexes[1], v2);
    setPoint(triangles[10].vertexes[2], v5);
    
    setPoint(triangles[11].vertexes[0], v2);
    setPoint(triangles[11].vertexes[1], v5);
    setPoint(triangles[11].vertexes[2], v6);
}