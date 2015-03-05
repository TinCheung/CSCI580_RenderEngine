//
//  test.cpp
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 2/13/15.
//  Copyright (c) 2015 tantianx. All rights reserved.
//

#include "test.h"
#include  <stdio.h>
#include "gz.h"
#include "mathLib.h"
#include <stdlib.h>

void testMatrixMultiply()
{
    GzMatrix A, B;
    GzMatrix *result = (GzMatrix *)malloc(sizeof(GzMatrix));
    
    int i, j, k;
    for (i = 0, k = 1; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            B[i][j] = A[i][j] = 1;
            printf("%d ", k);
            k++;
        }
        printf("\n");
    }
    
    matrixMultiply(A, B, A);
    printMatrix(A);
    
    free(result);
}

void testMatrixMultiplyVector()
{
    GzMatrix A;
    GzPoint Vec;
    GzPoint *result = (GzPoint *)malloc(sizeof(GzPoint));
    
    int i, j, k;
    for (i = 0, k = 1; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            A[i][j] = k;
            printf("%d ", k);
            k++;
        }
        Vec[i] = i + 1;
        printf(" %d\n", (int)Vec[i]);
    }
    
    matrixMultiplyVector(A, Vec, Vec);
    printPoint(Vec);
    
    free(result);
}

void printMatrix(GzMatrix m)
{
    int i, j;
    
    printf("Matrix Printed: \n");
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            printf("%f ", m[i][j]);
        }
        printf("\n");
    }
}
void printPoint(GzPoint v)
{
    int i;
    printf("Point Printed: \n");
    for (i = 0; i < 4; i++)
        printf("%f ", v[i]);
    printf("\n");
}

void printVector(GzVector v)
{
    int i;
    printf("Vector Printed: \n");
    for (i = 0; i < 3; i++)
        printf("%f ", v[i]);
    printf("\n");
}

void rememberPoints(GzPoint point)
{
    GzPoint tested[3] = {{2.400000f, 2.250000f, 1.000000f, 0.0f},
                         {-1.291500f,1.250000f,	0.549500f, 0.0f},
                         {0.273482f, -0.323828f,2.541834f, 0.0f}};
    
    int i, j;
    float distance, total;
    for (i = 0; i < 3; i++) {
        total = 0;
        for (j = 0; j < 3; j++) {
            distance = point[j] - tested[i][j];
            total += distance > 0 ? distance : -1*distance;
            //printf("distance%d: %f\n", j, distance);
        }
        //printf("total: %f\n", total);
        //printf("\n");
    }
}

void testInvert()
{
    GzMatrix A = {{1,2,3,0},
                  {2,2,1,0},
                  {3,4,3,0},
                  {0,0,0,1}};
    GzMatrix *result = (GzMatrix *)malloc(sizeof(GzMatrix));
    
    invertMatrix3(A, A);
    printMatrix(A);
    
    free(result);
}

void testMatrixNormalization()
{
    GzMatrix A;
    int i, j;
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            A[i][j] = i + j;
        }
    }
    printMatrix(A);
    matrixNormalization(A);
    printMatrix(A);
}

void testTriangleArea()
{
    GzPoint A = {0, 0, 0, 0};
    GzPoint B = {1, 1, 0, 0};
    GzPoint C = {1, -1, 0, 0};
    
    printf("%f\n", triangleArea(A, B, C));
}