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