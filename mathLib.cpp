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

/*
 a1·b2·c3+b1·c2·a3+c1·a2·b3-a3·b2·c1-b3·c2·a1-c3·a2·b1
   0   1   2
0  a1  b1  c1
1  a2  b2  c2
2  a3  b3  c3
 */
float det3(GzMatrix mat)
{
    return (  mat[0][0] * mat[1][1] * mat[2][2]
            + mat[0][1] * mat[1][2] * mat[2][0]
            + mat[0][2] * mat[1][0] * mat[2][1]
            - mat[2][0] * mat[1][1] * mat[0][2]
            - mat[2][1] * mat[1][2] * mat[0][0]
            - mat[2][2] * mat[1][0] * mat[0][1] );
}

float det2(GzMatrix mat, int i, int j)
{
    float temp[4];
    int sub = 0;
    for (int v = 0; v < 3; v++) {
        for (int u = 0; u < 3; u++) {
            if (v == i || u == j) continue;
            temp[sub] = mat[v][u];
            sub++;
        }
    }
    return pow(-1, i + j) * (temp[0] * temp[3] - temp[1] * temp[2]);
}

void invertMatrix3(GzMatrix A, GzMatrix C)
{
    GzMatrix *result = (GzMatrix *)malloc(sizeof(GzMatrix));
    int i, j;
    float detA = det3(A);
    //printf("detA: %f\n", detA);
    for (i = 0; i < 3; i ++) {
        for (j = 0; j < 3; j++) {
            //printf("det %d %d: %f\n", i, j, det2(A,i,j));
            (*result)[i][j] = det2(A, i, j) / detA;
        }
    }
    TMatrix(*result, *result);
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            C[i][j] = (*result)[i][j];
        }
    }
}

void TMatrix(GzMatrix A, GzMatrix B)
{
    GzMatrix *result = (GzMatrix *)malloc(sizeof(GzMatrix));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            (*result)[i][j] = A[j][i];
        }
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            B[i][j] = (*result)[i][j];
        }
    }
    free(result);
}

bool isSameMatrix(GzMatrix A, GzMatrix B)
{
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (A[i][j] != B[i][j]) {
                return false;
            }
        }
    }
    return true;
}

bool isScaleMatrix(GzMatrix mat)
{
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (i != j && mat[i][j] != 0) {
                return false;
            }
        }
    }
    return true;
}

bool isTranslateMatrix(GzMatrix mat)
{
    int i;
    for (i = 0; i < 3; i++) {
        if (mat[i][3] != 0) return true;
    }
    return false;
}

void matrixNormalization(GzMatrix mat)
{
    int i, j;
    float sum;
    for (j = 0; j < 4; j++) {
        sum = 0;
        for (i = 0; i < 4; i++) {
            sum += pow(mat[i][0] ,2);
        }
        sum = sqrtf(sum);
        for (i = 0; i < 4; i++) {
            mat[i][j] = mat[i][j] / sum;
        }
    }
}

void addColorInLight(GzColor result, GzLight light, GzColor coefficient, GzVector vector1, GzVector vector2, float power)
{
    int i;
    float product, color, temp;
    product = dotProduct(vector1, vector2);
    product = product > 1 ? 1 : product;
    product = product < 0 ? 0 : product;
    for (i = 0; i < 3; i++) {
        color = light.color[i];
        temp = coefficient[i] * color * pow(product, power);
        result[i] = temp;
    }
}

void getReflectLightDirection(GzVector in, GzVector normal, GzVector out)
{
    normalization(in);
    normalization(normal);
    
    float normalLength = 2 * dotProduct(in, normal);
    for (int m = 0; m < 3; m++) {
        out[m] = normalLength * normal[m] - in[m];
    }
}

void bilinearInterpolationInTriangle(GzPoint P, GzPoint A, GzPoint B, GzPoint C, GzVector result)
{
    float totalArea = triangleArea(A, B, C);
    float PABArea = triangleArea(P, A, B);
    float PACArea = triangleArea(P, A, C);
    float PBCArea = triangleArea(P, B, C);
    
    result[0] = (PBCArea / totalArea);
    result[1] = (PACArea / totalArea);
    result[2] = (PABArea / totalArea);
}

float triangleArea(GzPoint P0, GzPoint P1, GzPoint P2)
{
    GzVector vector1, vector2;
    float length1, length2;
    int i;
    
    for (i = 0; i < 3; i++) {
        vector1[i] = P1[i] - P0[i];
        vector2[i] = P2[i] - P0[i];
    }
    length1 = vectorLenght(vector1);
    length2 = vectorLenght(vector2);
    
    normalization(vector1);
    normalization(vector2);
    
    float degree = acos(dotProduct(vector1, vector2));
    return (length1 * sinf(degree) * length2 / 2);
}

float vectorLenght(GzVector vector)
{
    return sqrtf(powf(vector[0], 2) + powf(vector[1], 2) + powf(vector[2], 2));
}