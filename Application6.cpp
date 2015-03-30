//
//  Application6.cpp
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 15/3/26.
//  Copyright (c) 2015年 tantianx. All rights reserved.
//

#define INFILE  "ppot.asc"
#define OUTFILE "output.ppm"

#include "Application6.h"
#include "gz.h"
#include "disp.h"
#include "rend.h"
#include <stdio.h>
#include <stdlib.h>

extern int tex_fun(float u, float v, GzColor color); /* image texture function */
extern int ptex_fun(float u, float v, GzColor color); /* procedural texture function */

int Application6::runRenderer()
{
    GzCamera    camera;
    GzToken     nameListTriangle[3]; 	/* vertex attribute names */
    GzPointer   valueListTriangle[3]; 	/* vertex attribute pointers */
    GzToken     nameListShader[9];      /* shader attribute names */
    GzPointer   valueListShader[9];     /* shader attribute pointers */
    GzToken     nameListLights[10];     /* light info */
    GzPointer   valueListLights[10];
    GzCoord		vertexList[3];		/* vertex position coordinates */
    GzCoord		normalList[3];		/* vertex normals */
    GzTextureIndex	uvList[3];		/* vertex texture map indices */
    char		dummy[256];
    int         status;
    int         xRes, yRes, dispClass;	/* display parameters */
    int         interpStyle;
    float       specpower;
    int         i;
    float offsetX, offsetY;
    
    FILE *outfile;
    if( (outfile  = fopen( OUTFILE , "wb" )) == NULL )
    {
        printf("Could not open output file for writing %s \n", OUTFILE);
        return GZ_FAILURE;
    }
    
    
    /* Transforms */
    GzMatrix	translateAndScale =
    {
        {3.25,	0.0,	0.0,	0.0},
        {0.0,	3.25,	0.0,	-3.25},
        {0.0,	0.0,	3.25,	3.5},
        {0.0,	0.0,	0.0,	1.0}
    };
    
    GzMatrix	rotateX =
    {
        {1.0,	0.0,	0.0,	0.0},
        {0.0,	.7071,	.7071,	0.0},
        {0.0,	-.7071,	.7071,	0.0},
        {0.0,	0.0,	0.0,	1.0}
    };
    
    GzMatrix	rotateY =
    {
        {.866,	0.0,	-0.5,	0.0},
        {0.0,	1.0,	0.0,	0.0},
        {0.5,	0.0,	.866,	0.0},
        {0.0,	0.0,	0.0,	1.0}
    };
    
    /* Light */
    GzLight	light1 = { {-0.7071, 0.7071, 0}, {0.5, 0.5, 0.9} };
    GzLight	light2 = { {0, -0.7071, -0.7071}, {0.9, 0.2, 0.3} };
    GzLight	light3 = { {0.7071, 0.0, -0.7071}, {0.2, 0.7, 0.3} };
    GzLight	ambientlight = { {0, 0, 0}, {0.3, 0.3, 0.3} };
    
    /* Material property */
    GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
    GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
    GzColor diffuseCoefficient = {0.7, 0.7, 0.7};
    
    /*
     * initialize the display and the renderer
     */
    
    int width = 256;    // frame buffer and display width
    int height = 256;   // frame buffer and display height
    
    for (i = 0; i < AAKERNEL_SIZE; i++) {
        status = 0;
        
        // Initialize the display.
        status |= GzNewDisplay(&display[i], GZ_RGBAZ_DISPLAY, width, height);
        status |= GzGetDisplayParams(display[i], &xRes, &yRes, &dispClass);
        status |= GzInitDisplay(display[i]);
        status |= GzNewRender(&(render[i]), GZ_Z_BUFFER_RENDER, display[i]);
        
#if 1 	/* set up app-defined camera if desired, else use camera defaults */
        camera.position[X] = -3;
        camera.position[Y] = -25;
        camera.position[Z] = -4;
        
        camera.lookat[X] = 7.8;
        camera.lookat[Y] = 0.7;
        camera.lookat[Z] = 6.5;
        
        camera.worldup[X] = -0.2;
        camera.worldup[Y] = 1.0;
        camera.worldup[Z] = 0.0;
        
        camera.FOV = 63.7;              /* degrees */
        
        status |= GzPutCamera(render[i], &camera);
        status |= GzBeginRender(render[i]);
#endif
        /*
         renderer is ready for frame --- define lights and shader at start of frame
         */
        
        /*
         * Tokens associated with light parameters
         */
        nameListLights[0] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[0] = (GzPointer)&light1;
        nameListLights[1] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[1] = (GzPointer)&light2;
        nameListLights[2] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[2] = (GzPointer)&light3;
        status |= GzPutAttribute(render[i], 3, nameListLights, valueListLights);
        
        nameListLights[0] = GZ_AMBIENT_LIGHT;
        valueListLights[0] = (GzPointer)&ambientlight;
        status |= GzPutAttribute(render[i], 1, nameListLights, valueListLights);
        
        /*
         * Tokens associated with shading
         */
        nameListShader[0]  = GZ_DIFFUSE_COEFFICIENT;
        valueListShader[0] = (GzPointer)diffuseCoefficient;
        
        /*
         * Select either GZ_COLOR or GZ_NORMALS as interpolation mode
         */
        nameListShader[1]  = GZ_INTERPOLATE;
#if 0
        interpStyle = GZ_COLOR;         /* Gouraud shading */
#else
        interpStyle = GZ_NORMALS;       /* Phong shading */
#endif
        
        valueListShader[1] = (GzPointer)&interpStyle;
        nameListShader[2]  = GZ_AMBIENT_COEFFICIENT;
        valueListShader[2] = (GzPointer)ambientCoefficient;
        nameListShader[3]  = GZ_SPECULAR_COEFFICIENT;
        valueListShader[3] = (GzPointer)specularCoefficient;
        nameListShader[4]  = GZ_DISTRIBUTION_COEFFICIENT;
        specpower = 32;
        valueListShader[4] = (GzPointer)&specpower;
        nameListShader[5] = GZ_TEXTURE_MAP;
#if 1
        valueListShader[5] = (GzPointer)tex_fun;  /* or use ptex_fun */
#else
        valueListShader[5] = (GzPointer)ptex_fun;
#endif
        // Add anti-aliasing offset
        offsetX = AAFilter[i][0];
        offsetY = AAFilter[i][1];
        
        valueListShader[6] = (GzPointer)&offsetX;
        nameListShader[6] = GZ_AASHIFTX;
        
        valueListShader[7] = (GzPointer)&offsetY;
        nameListShader[7] = GZ_AASHIFTY;
        
        status |= GzPutAttribute(render[i], 8, nameListShader, valueListShader);
        
        status |= GzPushMatrix(render[i], translateAndScale);
        status |= GzPushMatrix(render[i], rotateY);
        status |= GzPushMatrix(render[i], rotateX);
        
        if (status) return(GZ_FAILURE);
        
        /*
         * Tokens associated with triangle vertex values
         */
        nameListTriangle[0] = GZ_POSITION;
        nameListTriangle[1] = GZ_NORMAL;
        nameListTriangle[2] = GZ_TEXTURE_INDEX;
        
        // I/O File open
        FILE *infile;
        if( (infile  = fopen( INFILE , "r" )) == NULL )
        {
            printf("Could not open input from %s \n", INFILE);
            return GZ_FAILURE;
        }
        
        /*
         * Walk through the list of triangles, set color
         * and render each triangle
         */
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
                   &(uvList[2][0]), &(uvList[2][1]) );
            
            /*
             * Set the value pointers to the first vertex of the
             * triangle, then feed it to the renderer
             * NOTE: this sequence matches the nameList token sequence
             */
            valueListTriangle[0] = (GzPointer)vertexList;
            valueListTriangle[1] = (GzPointer)normalList;
            valueListTriangle[2] = (GzPointer)uvList;
            GzPutTriangle(render[i], 3, nameListTriangle, valueListTriangle);
        }
        
        // GzFlushDisplay2File(outfile, display[i]); 	/* write out or update display to file*/
        
        /*
         * Clean up and exit
         */
        
        if( fclose( infile ) )
            printf( "The input file was not closed\n" );
        
        // status |= GzFreeRender(render[i]);
        // status |= GzFreeDisplay(display[i]);
        
        if (status != GZ_SUCCESS) {
            return GZ_FAILURE;
        }
    }
    
    // Combine all the images from the displays.
    int j, k, sub, sum;
    for (j = 0; j < height; j++) {
        for (k = 0; k < width; k++) {
            sub = j * width + k;
            
            sum = 0;
            for (i = 0; i < AAKERNEL_SIZE; i++) {
                sum += display[i]->fbuf[sub].red * AAFilter[i][2];
            }
            display[0]->fbuf[sub].red = sum;
            
            sum = 0;
            for (i = 0; i < AAKERNEL_SIZE; i++) {
                sum += display[i]->fbuf[sub].green * AAFilter[i][2];
            }
            display[0]->fbuf[sub].green = sum;
            
            sum = 0;
            for (i = 0; i < AAKERNEL_SIZE; i++) {
                sum += display[i]->fbuf[sub].blue * AAFilter[i][2];
            }
            display[0]->fbuf[sub].blue = sum;
        }
    }
    
    GzFlushDisplay2File(outfile, display[0]);
    
    if( fclose( outfile ) )
        printf( "The output file was not closed\n" );
    
    return GZ_SUCCESS;
}