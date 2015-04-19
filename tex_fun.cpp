/* Texture functions for cs580 GzLib	*/
#include	<stdlib.h>
#include	<stdio.h>
#include	<math.h>
#include    <stdlib.h>
#include	"gz.h"
#include    "mathLib.h"
#include    "stroke.h"
#include    "time.h"

GzColor	*image;
float *shadow1, *shadow2, *shadow3;
int xs, ys;
int reset = 1;

int getSub(int x, int y)
{
    return y * xs + x;
}

void generateShadow(int density, float img[], bool horizonShadow = true)
{
    int x, length, sub;
    int *wave, *thick;
    
    length = ys;
    x = density;
    do {
        wave = new int[length];
        thick = new int[length];
        srand(time(0));
        getWaveAndThickness(length, 4, thick, wave, 10);
        for (int i = 0; i < ys; i++) {
            for (int j = 0; j <= thick[i]; j++) {
                sub = getSub(x + wave[i] + j, i);
                img[sub] = 0;
            }
        }
        
        delete [] wave;
        delete [] thick;
        
        x += density + rand() % 5 - 2;
    } while (x < xs);
    
    if (horizonShadow) {
        length = xs;
        int y = density;
        do {
            wave = new int[length];
            thick = new int[length];
            srand(time(0));
            getWaveAndThickness(length, 4, thick, wave, 10);
            for (int i = 0; i < xs; i++) {
                printf("wave: %d\n", wave[i]);
                for (int j = 0; j < thick[i]; j++) {
                    sub = getSub(i, y + wave[i] + j);
                    img[sub] = 0;
                }
            }
            
            delete [] wave;
            delete [] thick;
            
            y += density + rand() % 5 - 2;
        } while(y < ys);
    }
}

/* Shadow texture function */
int shadow_fun(float u, float v, GzColor color)
{
    if (reset) {
        xs = 500;
        ys = 500;
        
        int i;
        // Generate the shadows.
        shadow1 = new float[xs * ys];
        shadow2 = new float[xs * ys];
        shadow3 = new float[xs * ys];
        
        for (i = 0; i < xs * ys; i++) {
            shadow1[i] = 1;
            shadow2[i] = 1;
            shadow3[i] = 1;
        }
        
        generateShadow(30, shadow1, true);
        generateShadow(20, shadow2, true);
        generateShadow(10, shadow3, true);
        
        reset = 0;
    }
    
    float tone = color[0];
    float *shadow;
    if (tone < 0.3) shadow = shadow3;
    else if (tone < 0.5) shadow = shadow2;
    else {
        color[0] = color[1] = color[2] = 1;
        return GZ_SUCCESS;
    }
    
    /* bounds-test u,v to make sure nothing will overflow image array bounds */
    if (u < 0 || u > 1 || v < 0 || v > 1) {
        return GZ_FAILURE;
    }
    
    int x = u * (xs - 1);
    int y = v * (ys - 1);
    /*
    for (int i = 0; i < 3; i++)
        color[i] = shadow1[y * xs + x];
    */
    GzPoint P, A, B, C, D, combination;
    P[0] = u * (xs - 1);
    P[1] = v * (ys - 1);
    
    A[0] = D[0] = int(P[0]);
    B[0] = C[0] = int(P[0]) + 1;
    C[1] = D[1] = int(P[1]);
    A[1] = B[1] = int(P[1]) + 1;
    
    bilinearInterpolationInSquare(P, A, B, C, D, combination);
    
    int subscriptsX[4] = {(int)A[0], (int)B[0], (int)C[0], (int)D[0]};
    int subscriptsY[4] = {(int)A[1], (int)B[1], (int)C[1], (int)D[1]};
    int subscript;
    
    for (int i = 0; i < 3; i++) {
        color[i] = 0;
    }
    
    for (int i = 0; i < 4; i++) {
        subscript = subscriptsX[i] + subscriptsY[i] * xs;
        
        color[RED] += shadow[subscript] * combination[i];
        color[GREEN] += shadow[subscript] * combination[i];
        color[BLUE] += shadow[subscript] * combination[i];
    }
    
    return GZ_SUCCESS;
}

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
	unsigned char	pixel[3];
	unsigned char	dummy;
	char		foo[8];
	int		i;
	FILE		*fd;

	if (reset) {          /* open and load texture file */
		fd = fopen ("texture", "rb");
		if (fd == NULL) {
			fprintf (stderr, "texture file not found\n");
			return GZ_FAILURE;
		}
		fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
		if (image == NULL) {
			fprintf (stderr, "malloc for texture image failed\n");
			return GZ_FAILURE;
		}

		for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
			fread(pixel, sizeof(pixel), 1, fd);
			image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		reset = 0;          /* init is done */
		fclose(fd);
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds */
    if (u < 0 || u > 1 || v < 0 || v > 1) {
        return GZ_FAILURE;
    }
    
    
    
	/*
    GzPoint P, A, B, C, D, combination;
    P[0] = u * (xs - 1);
    P[1] = v * (ys - 1);
    
    A[0] = D[0] = int(P[0]);
    B[0] = C[0] = int(P[0]) + 1;
    C[1] = D[1] = int(P[1]);
    A[1] = B[1] = int(P[1]) + 1;
    
    bilinearInterpolationInSquare(P, A, B, C, D, combination);
    
    int subscriptsX[4] = {(int)A[0], (int)B[0], (int)C[0], (int)D[0]};
    int subscriptsY[4] = {(int)A[1], (int)B[1], (int)C[1], (int)D[1]};
    int subscript;
    
    for (i = 0; i < 3; i++) {
        color[i] = 0;
    }
    
    for (i = 0; i < 4; i++) {
        subscript = subscriptsX[i] + subscriptsY[i] * xs;
        
        color[RED] += image[subscript][RED] * combination[i];
        color[GREEN] += image[subscript][GREEN] * combination[i];
        color[BLUE] += image[subscript][BLUE] * combination[i];
    }*/
    
    return GZ_SUCCESS;
}

/* White */
int white_tex_fun(float u, float v, GzColor color)
{
    float value = 1;
    
    if (u < 0.5 && v < 0.5) value = 0;
    if (u > 0.5 && v > 0.5) value = 0;
    for (int i = 0; i < 3; i++)
        color[i] = value;
    return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
    // Implement a 8 x 8 chess board
    int i;
    int x = u * 8;
    int y = v * 8;
    
    float value;
    if ((x+y)%2 == 0) value = 1;
    else value = 0;
    
    for (i = 0; i < 3; i++)
        color[i] = value;
    
    // Julia set
    juliaSet(u, v, color);
    
    return GZ_SUCCESS;
}

