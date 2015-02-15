//
//  mathLib.cpp
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 1/22/15.
//  Copyright (c) 2015 tantianx. All rights reserved.
//
#include <stdlib.h>
#include <math.h>
#include "mathLib.h"

int scale(short v, int max, int tMax)
{
    double k = (double)tMax / (double)max;
    return (short)((double)v * k);
}

void normalization(GzVector v)
{
    float len;
    int i;
    
    for (i = 0, len = 0; i < 3; i++) {
        len += pow(v[i], 2);
    }
    len = sqrtf(len);
    for (i = 0; i < 3; i++) {
        v[i] = v[i] / len;
    }
}

// Matrix A, B. Return the result AB to C
void matrixMultiply(GzMatrix A, GzMatrix B, GzMatrix C)
{
    int i, j, k;
    GzMatrix *result = (GzMatrix *)malloc(sizeof(GzMatrix));
    float sum;
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            sum = 0;
            for (k = 0; k < 4; k++) {
                sum += A[i][k] * B[k][j];
            }
            (*result)[i][j] = sum;
        }
    }
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            C[i][j] = (*result)[i][j];
        }
    }
    
    free(result);
}

void matrixMultiplyVector(GzMatrix m, GzPoint v, GzPoint C)
{
    int i, j;
    GzPoint *result = (GzPoint *)malloc(sizeof(GzPoint));
    float sum;
    
    for (i = 0; i < 4; i++) {
        sum = 0;
        for (j = 0; j < 4; j++) {
            sum += m[i][j] * v[j];
        }
        (*result)[i] = sum;
    }
    
    for (i = 0; i < 4; i++) {
        C[i] = (*result)[i];
    }
    
    free(result);
}

float dotProduct(GzVector a, GzVector b)
{
    int i;
    float result;
    for (i = 0, result = 0; i < 3; i++) {
        result += (a[i] * b[i]);
    }
    
    return result;
}

void crossProduct(GzVector a, GzVector b, GzVector c)
{
    int i;
    GzVector *result = (GzVector *)malloc(sizeof(GzVector));
    
    (*result)[0] = a[1] * b[2] - a[2] * b[1];
    (*result)[1] = a[2] * b[0] - a[0] * b[2];
    (*result)[2] = a[0] * b[1] - a[1] * b[0];
    
    for (i = 0; i < 3; i++)
        c[i] = (*result)[i];
    
    free(result);
}