//
//  mathLib.h
//  CSCI580_RenderEngine
//
//  In this file, we define some basic math operations.
//
//  Created by Tan Tian Xiang on 1/22/15.
//  Copyright (c) 2015 tantianx. All rights reserved.
//

#ifndef CSCI580_RenderEngine_mathLib_h
#define CSCI580_RenderEngine_mathLib_h
#define PI 3.14159265

#include "rend.h"
#include "gz.h"

int scale(short v, int max, int tMax);
float absf(float x);
int round(float x);
int max(int x, int y);
int min(int x, int y);

void bilinearInterpolationInTriangle(GzPoint, GzPoint, GzPoint, GzPoint, GzVector);
void bilinearInterpolationInSquare(GzPoint, GzPoint, GzPoint, GzPoint, GzPoint, GzPoint);
float triangleArea(GzPoint, GzPoint, GzPoint);
float squareArea(GzPoint, GzPoint);
void setPoint(GzPoint point, float coordinates[]);


// Vector operations.
float dotProduct(GzVector, GzVector);
void crossProduct(GzVector, GzVector, GzVector);
void normalization(GzVector);
float vectorLenght(GzVector);

// Matrix operations.
void matrixMultiply(GzMatrix , GzMatrix, GzMatrix);
void matrixMultiplyVector(GzMatrix, GzPoint, GzPoint);
void invertMatrix3(GzMatrix, GzMatrix);
void TMatrix(GzMatrix, GzMatrix);
float det2(GzMatrix, int, int);
float det3(GzMatrix);
bool isSameMatrix(GzMatrix, GzMatrix);
bool isScaleMatrix(GzMatrix);
bool isTranslateMatrix(GzMatrix);
void matrixNormalization(GzMatrix);

// Light operations.
void addColorInLight(GzColor, GzLight, GzColor, GzVector, GzVector, float power=1);
void getReflectLightDirection(GzVector, GzVector, GzVector);

// Noise function.
float noiseGeneration(float x, float y);
float smoothNoise(float x, float y);
float interpolatedNoise(float x, float y);
float perlinNoise(float x, float y);

// Julia set
// Complex Number
struct Complex {
    float re;
    float im;
};

typedef struct Complex complex;
complex complexMultiply(complex a, complex b);
complex complexPlus(complex a, complex b);
void juliaSet(float u, float v, GzColor color);
void hsvToRGB(float h, float s, float v, GzColor rgb);

#endif
