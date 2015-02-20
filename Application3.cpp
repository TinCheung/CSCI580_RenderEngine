// Application3.cpp: implementation of the Application3 class.
//
//////////////////////////////////////////////////////////////////////

/*
 * application test code for homework assignment #3
*/

#define INFILE3  "pot4.asc"
#define OUTFILE3 "output.ppm"

#include "Application3.h"
#include "gz.h"
#include "disp.h"
#include "rend.h"
#include "mathLib.h"
#include "test.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <queue>
#include <string>

using namespace std;

void shade(GzCoord norm, GzCoord color);

int Application3::runRenderer()
{
	GzCamera	camera;
	GzToken		nameListTriangle[3]; 	/* vertex attribute names */
	GzPointer	valueListTriangle[3]; 	/* vertex attribute pointers */
	GzToken     nameListColor[3];       /* color type names */
	GzPointer   valueListColor[3];      /* color type rgb pointers */
	GzColor		color; 
	GzCoord		vertexList[3];		/* vertex position coordinates */ 
	GzCoord		normalList[3];		/* vertex normals */ 
	GzTextureIndex	uvList[3];		/* vertex texture map indices */ 
	char		dummy[256]; 
	int		i; 
	int		xRes, yRes, dispClass;	/* display parameters */ 
	int		status; 
 
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

	/* 
	 * initialize the display and the renderer 
	 */ 

	int width = 256;    // frame buffer and display width
	int height = 256;   // frame buffer and display height

	status = 0;

    status |= GzNewDisplay(&display, GZ_RGBAZ_DISPLAY, width, height);
	status |= GzGetDisplayParams(display, &xRes, &yRes, &dispClass);
	status |= GzInitDisplay(display);
	status |= GzNewRender(&render, GZ_Z_BUFFER_RENDER, display); 

#if 1 	/* set up app-defined camera if desired, else use camera defaults */
	camera.position[X] = 13.2;      
  	camera.position[Y] = -8.7;
  	camera.position[Z] = -14.8;

  	camera.lookat[X] = 0.8;
  	camera.lookat[Y] = 0.7;
  	camera.lookat[Z] = 4.5;

  	camera.worldup[X] = -0.2;
  	camera.worldup[Y] = 1.0;
  	camera.worldup[Z] = 0.0;

	camera.FOV = 53.7;              /* degrees */

	status |= GzPutCamera(render, &camera); 
#endif 

	status |= GzBeginRender(render);

	status |= GzPushMatrix(render, translateAndScale);  
	status |= GzPushMatrix(render, rotateY);
	status |= GzPushMatrix(render, rotateX);
    
	if (status) return(GZ_FAILURE); 

	 /* 
	 * Tokens associated with triangle vertex values 
	 */ 
	 nameListTriangle[0] = GZ_POSITION; 
	 
	// I/O File open
	FILE *infile;
	if( (infile  = fopen( INFILE3 , "r" )) == NULL )
	{
		printf("Could not open input from %s \n", INFILE3);
		return GZ_FAILURE;
	}

	FILE *outfile;
	if( (outfile  = fopen( OUTFILE3 , "wb" )) == NULL )
	{
		printf("Could not open output file for writing %s \n", OUTFILE3);
		return GZ_FAILURE;
	}

	/* 
	* Walk through the list of triangles, set color 
	* and render each triangle 
	*/ 
	i = 0; 
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
	    * Set up shading attributes for each triangle 
	    */ 
	    shade(normalList[0], color);/* shade based on the norm of vert0 */
	    valueListColor[0] = (GzPointer)color; 
	    nameListColor[0] = GZ_RGB_COLOR; 
	    GzPutAttribute(render, 1, nameListColor, valueListColor); 
 
	     /* 
	     * Set the value pointers to the first vertex of the 	
	     * triangle, then feed it to the renderer 
	     */ 
	     valueListTriangle[0] = (GzPointer)vertexList;
 
	     GzPutTriangle(render, 1, nameListTriangle, valueListTriangle);
	} 

	GzFlushDisplay2File(outfile, display); 	/* write out or update display to file*/

	/* 
	 * Clean up and exit
	 */ 

	if( fclose( infile ) )
		printf( "The input file was not closed\n" );
  
	if( fclose( outfile ) )
		printf( "The output file was not closed\n" );

	status |= GzFreeRender(render); 
	status |= GzFreeDisplay(display); 
 
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

/* 
This doesn't really belong in the application program, but for this 
simplified case of a renderer that doesn't do any shading itself, this 
is the easiest place to put it.
*/

void shade(GzCoord norm, GzCoord color)
{
  GzCoord	light;
  float		coef;

  light[0] = 0.707f;
  light[1] = 0.5f;
  light[2] = 0.5f;

  coef = light[0]*norm[0] + light[1]*norm[1] + light[2]*norm[2];
  if (coef < 0) 	coef *= -1;

  if (coef > 1.0)	coef = 1.0;
  color[0] = coef*0.95f;
  color[1] = coef*0.65f;
  color[2] = coef*0.88f;
}

/* The animate function */
int Application3::runAnimate(float *translation, float *rotation, int step)
{
    int status = 0;
    
    float translationInc[3];
    float rotationInc[3];
    
    GzToken		nameListTriangle[3]; 	/* vertex attribute names */
    GzPointer	valueListTriangle[3]; 	/* vertex attribute pointers */
    GzToken     nameListColor[3];       /* color type names */
    GzPointer   valueListColor[3];      /* color type rgb pointers */
    GzColor		color;
    GzCoord		vertexList[3];		/* vertex position coordinates */
    GzCoord		normalList[3];		/* vertex normals */
    GzTextureIndex	uvList[3];		/* vertex texture map indices */
    char		dummy[256];
    stringstream outputFileNameStream;
    string outputFileName;
    FILE *infile, *outFile;
    
    GzCamera	camera;
    int		xRes, yRes, dispClass;
    int width = 256;    // frame buffer and display width
    int height = 256;   // frame buffer and display height
    
    camera.position[X] = 13.2;
    camera.position[Y] = -8.7;
    camera.position[Z] = -14.8;
    
    camera.lookat[X] = 0.8;
    camera.lookat[Y] = 0.7;
    camera.lookat[Z] = 4.5;
    
    camera.worldup[X] = -0.2;
    camera.worldup[Y] = 1.0;
    camera.worldup[Z] = 0.0;
    
    camera.FOV = 53.7;              /* degrees */
    
    for (int i = 0; i < 3; i++) {
        translationInc[i] = translation[i] / step;
        rotationInc[i] = rotation[i] / step;
    }
    
    GzMatrix translateMatrix = {{1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}};
    
    GzMatrix scaleMatrix = {{3.25, 0, 0, 0},
        {0, 3.25, 0, 0},
        {0, 0, 3.25, 0},
        {0, 0, 0, 1}};
    
    GzMatrix	rotateX;
    GzMatrix	rotateY;
    GzMatrix	rotateZ;
    
    for (int i = 1; i <= step; i++) {
        status |= GzNewDisplay(&display, GZ_RGBAZ_DISPLAY, width, height);
        status |= GzGetDisplayParams(display, &xRes, &yRes, &dispClass);
        status |= GzInitDisplay(display);
        status |= GzNewRender(&render, GZ_Z_BUFFER_RENDER, display);
        status |= GzPutCamera(render, &camera);
        
        outputFileNameStream.clear();
        outputFileName.clear();
        outputFileNameStream << "./Images/" << i << ".ppm";
        outputFileNameStream >> outputFileName;
        
        infile = fopen(INFILE3, "r");
        if (infile == NULL) {
            printf("Could not open output file for writing %s \n", OUTFILE3);
            return GZ_FAILURE;
        }
        
        outFile = fopen(outputFileName.c_str(), "wb");
        if(outFile == NULL)
        {
            printf("Could not open output file for writing %s \n", OUTFILE3);
            return GZ_FAILURE;
        }
        
        nameListTriangle[0] = GZ_POSITION;
        
        /* Change the matrix. */
        while (this->render->matlevel > 3) {
            GzPopMatrix(this->render);
        }
        
        translateMatrix[0][3] = translationInc[0] * i;
        translateMatrix[1][3] = translationInc[1] * i;
        translateMatrix[2][3] = translationInc[2] * i;
        /*
        rotateX[1][1] = rotateX[2][2] = (float)cos(rotationInc[0] * i);
        rotateX[2][1] = (float)(sin(rotationInc[0] * i));
        rotateX[1][2] = (float)(-1 * sin(rotationInc[0] * i));
        
        rotateY[0][0] = rotateY[2][2] = (float)cos(rotationInc[1] * i);
        rotateY[0][2] = (float)(sin(rotationInc[1] * i));
        rotateY[2][0] = (float)(-1 * sin(rotationInc[1] * i));
        
        rotateZ[0][0] = rotateZ[1][1] = (float)cos(rotationInc[2] * i);
        rotateZ[1][0] = (float)(sin(rotationInc[2] * i));
        rotateZ[0][1] = (float)(-1 * sin(rotationInc[2] * i));
        */
        GzRotXMat(rotationInc[0] * i, rotateX);
        GzRotYMat(rotationInc[1] * i, rotateY);
        GzRotZMat(rotationInc[2] * i, rotateZ);
        
        //printMatrix(rotateY);
        
        status |= GzPushMatrix(this->render, translateMatrix);
        status |= GzPushMatrix(this->render, scaleMatrix);
        status |= GzPushMatrix(this->render, rotateX);
        status |= GzPushMatrix(this->render, rotateY);
        status |= GzPushMatrix(this->render, rotateZ);
        
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
             * Set up shading attributes for each triangle
             */
            shade(normalList[0], color);/* shade based on the norm of vert0 */
            valueListColor[0] = (GzPointer)color;
            nameListColor[0] = GZ_RGB_COLOR;
            GzPutAttribute(render, 1, nameListColor, valueListColor);
            
            /*
             * Set the value pointers to the first vertex of the
             * triangle, then feed it to the renderer
             */
            valueListTriangle[0] = (GzPointer)vertexList;
            GzPutTriangle(render, 1, nameListTriangle, valueListTriangle);
        }
        GzFlushDisplay2File(outFile, display); 	/* write out or update display to file*/
        printf("Finish %d\n", i);
        fclose(infile);
        fclose(outFile);
        status |= GzFreeRender(render);
        status |= GzFreeDisplay(display);
    }

    return status;
}