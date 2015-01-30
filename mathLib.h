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

int scale(short v, int max, int tMax);

// Vector operations.
float dotProduct(float vector1[], float vector2[]);
float crossProduct(float vector1[], float vector2[]);
float normalization(float *x, float *y, float *z);

#endif
