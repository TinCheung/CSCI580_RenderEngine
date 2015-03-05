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

void bilinearInterpolationInTriangle(GzPoint, GzPoint, GzPoint, GzPoint, GzVector);
float triangleArea(GzPoint, GzPoint, GzPoint);

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

#endif
