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

// Vector operations.
float dotProduct(GzVector, GzVector);
void crossProduct(GzVector, GzVector, GzVector);
void normalization(GzVector);

// Matrix operations.
void matrixMultiply(GzMatrix , GzMatrix, GzMatrix);
void matrixMultiplyVector(GzMatrix, GzPoint, GzPoint);

#endif
