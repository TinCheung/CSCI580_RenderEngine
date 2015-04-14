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
#include "test.h"
#include <time.h>

using namespace std;

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
    
    //printf("t-area: %f, ab: %f, ac: %f, bc: %f\n", totalArea, PABArea, PACArea, PBCArea);
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
    
    //printVector(vector1);
    //printVector(vector2);

    float degree = dotProduct(vector1, vector2) < -1 || dotProduct(vector1, vector2) > 1 ? 0 : acos(dotProduct(vector1, vector2));
    float result = (length1 * sin(degree) * length2 / 2);
    
    return result;
}

float vectorLenght(GzVector vector)
{
    return sqrtf(powf(vector[0], 2) + powf(vector[1], 2) + powf(vector[2], 2));
}

float squareArea(GzPoint a, GzPoint b)
{
    float xDiff = a[0] - b[0];
    float yDiff = a[1] - b[1];
    
    xDiff = xDiff < 0 ? xDiff * -1 : xDiff;
    yDiff = yDiff < 0 ? yDiff * -1 : yDiff;

    return xDiff * yDiff;
}

void bilinearInterpolationInSquare(GzPoint p, GzPoint a, GzPoint b, GzPoint c, GzPoint d, GzPoint result)
{
    float totalArea = squareArea(a, c);
    float apArea = squareArea(a, p);
    float bpArea = squareArea(b, p);
    float cpArea = squareArea(c, p);
    float dpArea = squareArea(d, p);
    
    result[0] = cpArea / totalArea;
    result[1] = dpArea / totalArea;
    result[2] = apArea / totalArea;
    result[3] = bpArea / totalArea;
}

complex complexMultiply(complex a, complex b)
{
    complex result;
    result.re = a.re * b.re - a.im * b.im;
    result.im = a.re * b.im + a.im * b.re;
    return result;
}

complex complexPlus(complex a, complex b)
{
    complex result;
    result.re = a.re + b.re;
    result.im = a.im + b.im;
    return result;
}

void hsv2rgb(float hue, float sat, float val, GzColor color, float maxBrightness) {
    unsigned int H_accent = hue/60;
    unsigned int bottom = (int)((255 - sat) * val)>>8;
    unsigned int top = val;
    unsigned char rising  = ((top-bottom)  *((int)hue%60   )  )  /  60  +  bottom;
    unsigned char falling = ((top-bottom)  *(60-(int)hue%60)  )  /  60  +  bottom;
    
    switch(H_accent) {
        case 0:
            color[RED] = top;
            color[GREEN] = rising;
            color[BLUE] = bottom;
            break;
            
        case 1:
            color[RED] = falling;
            color[GREEN] = top;
            color[BLUE] = bottom;
            break;
            
        case 2:
            color[RED] = bottom;
            color[GREEN] = top;
            color[BLUE] = rising;
            break;
            
        case 3:
            color[RED] = bottom;
            color[GREEN] = falling;
            color[BLUE] = top;
            break;
            
        case 4:
            color[RED] = rising;
            color[GREEN] = bottom;
            color[BLUE] = top;
            break;
            
        case 5:
            color[RED] = top;
            color[GREEN] = bottom;
            color[BLUE] = falling;
            break;
    }
    // Scale values to maxBrightness
    color[RED] = color[RED] * maxBrightness/255;
    color[GREEN] = color[GREEN] * maxBrightness/255;
    color[BLUE] = color[BLUE] * maxBrightness/255;
}


void juliaSet(float u, float v, GzColor color)
{
    static bool init = false;
    static GzColor set[512][512];
    
    int i, j, k, l;
    complex z, c;
    c.re = -0.70176, c.im = -0.3842;
    if (!init) {
        for (i = 0; i < 512; i++) {
            for (j = 0; j < 512; j++) {
                z.re = -1.6 + 3.2 * (i / 512.0);
                z.im = -1.2 + 2.4 * (j / 512.0);
                for (k = 0; k < 180; k++) {
                    if (z.re * z.re + z.im * z.im > 2.0) break;
                    z = complexMultiply(z, z);
                    z = complexPlus(z, c);
                }
                if (k >= 180) {
                    for (l = 0; l < 3; l++) set[i][j][l] = 0;
                }
                else {
                    float h, s, v;
                    h = (k) % 360;
                    s = 255;
                    v = 255;
                    
                    hsv2rgb(h, s, v, set[i][j], 1);
                }
                //printVector(set[i][j]);
            }
        }
        init = true;
    }
    
    int x, y;
    x = u * 511;
    y = v * 511;
    
    for (l = 0; l < 3; l++)
        color[l] = set[x][y][l];
}

float noiseGeneration(float floatX, float floatY)
{
    int x = (int)floatX;
    int y = (int)floatY;
    
    x = x + y * 57;
    
    float value = ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
    
    static float noiseMatrix[1024][1024];
    static bool initialized = false;
    if (!initialized) {
        int i, j;
    
        for (i = 0; i < 1024; i++) {
            for (j = 0; j < 1024; j++) {
                noiseMatrix[i][j] = (float)rand() / (float)RAND_MAX;
            }
        }
        initialized = true;
    }
    
    floatX = (int)((floatX + 1) / 2 * 1024) % 1024;
    floatY = (int)((floatY + 1) / 2 * 1024) % 1024;
    
    int x0 = (int)floatX;
    int y0 = (int)floatY;
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    
    int fx = floatX - x0;
    int fy = floatY - y0;
    
    float vx1, vx2;
    vx1 = fx * noiseMatrix[x0][y0] + (1-fx) * noiseMatrix[x1][y0];
    vx2 = fx * noiseMatrix[x0][y1] + (1-fx) * noiseMatrix[x1][y1];
    value = vx1 * fy + (1-fy) * vx2;
    
    return value;
}

float smoothNoise(float x, float y)
{
    //printf("x: %f y: %f\n", x, y);
    float corners = (noiseGeneration(x-1, y-1) + noiseGeneration(x+1, y-1)
                     + noiseGeneration(x-1, y+1) + noiseGeneration(x+1, y+1) ) / 16;
    float sides   = (noiseGeneration(x-1, y) +noiseGeneration(x+1, y) +noiseGeneration(x, y-1) +noiseGeneration(x, y+1) ) / 8;
    float center  =  noiseGeneration(x, y) / 4;
    
    return corners + sides + center;
}

float interpolatedNoise(float x, float y)
{
    int ix = (int) x;
    int iy = (int) y;
    
    float fx = x - ix;
    float fy = y - iy;
    
    fx = fx > 0 ? fx : -1 * fx;
    fy = fy > 0 ? fy : -1 * fy;
    
    float n[4];
    int dx[4] = {0, 1, 0, 1};
    int dy[4] = {0, 0, 1, 1};
    
    int i;
    for (i = 0; i < 4; i++)
        n[i] = smoothNoise(ix + dx[i], iy + dy[i]);
    
    float value1, value2;
    value1 = n[0] * fx + n[1] * (1 - fx);
    value2 = n[2] * fx + n[3] * (1 - fx);
    
    return (value1 * fy) + (value2 * (1 - fy));
}


float perlinNoise(float x, float y)
{
    float value = 0;
    for (int i = 0; i < 32; i++)
    {
        value += pow(2, -1 * i) * absf(interpolatedNoise(x * pow(2, i), y * pow(2, i)));
    }
    //value *= 3;
    value = value > 1 ? 1 : value;
    //printf("x: %f, y:%f, value: %f\n", x, y, value);
    
    return value;
}

void setPoint(GzPoint point, float coordinates[])
{
    for (int i = 0; i < 4; i++)
        point[i] = coordinates[i];
}

float absf(float x)
{
    return (x > 0 ? x : x * -1);
}