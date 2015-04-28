/* Texture functions for cs580 GzLib	*/
#include    "myRand.h"
#include	<stdio.h>
#include	<math.h>
#include	"gz.h"
#include    "mathLib.h"
#include    "stroke.h"

GzColor	*image;
float *shadow1, *shadow2, *shadow3, *brick, *grass, *shadows[8];
int *brickId, brickCount;
int xs, ys;
int reset = 1;
int prevR = -1;
bool *reservedForWhite;

int getSub(int x, int y)
{
    return y * xs + x < xs * ys ? y * xs + x : xs + ys - 1;
}

void generateShadow(int density, float img[], bool verticalShadow = false, bool horizonShadow = false, bool reservedWhite=true)
{
    int x, length, sub;
    int *wave, *thick;
    
    length = ys;
    x = density;
    
    int thickness = 2;
    
    if (verticalShadow) {
        do {
            wave = new int[length];
            thick = new int[length];
        
            getWaveAndThickness(length, thickness, thick, wave, 5);
            for (int i = 0; i < ys; i++) {
                for (int j = 0; j <= thick[i]; j++) {
                    sub = getSub(x + wave[i] + j, i);
                    if (reservedForWhite[sub] && reservedWhite) continue;
                    img[sub] = 0;
                }
            }
        
            delete [] wave;
            delete [] thick;
        
            x += density + rand2() % 5 - 2;
            int random = rand2() % 100;
            if (random < 20) {
                x += rand2() % 30;
            }
            
        } while (x < xs);
    }
    
    if (horizonShadow) {
        length = xs;
        int y = density;
        do {
            wave = new int[length];
            thick = new int[length];
            getWaveAndThickness(length, thickness, thick, wave, 5);
            for (int i = xs - 1; i >= 0; i--) {
                for (int j = thick[i] - 1; j >= 0; j--) {
                    sub = getSub(i, y + wave[i] + j);
                    if (reservedForWhite[sub] && reservedWhite) continue;
                    img[sub] = 0;
                }
            }
            
            delete [] wave;
            delete [] thick;
            
            y += density + rand2() % 5 - 2;
            
            int random = rand2() % 100;
            if (random < 20) {
                y += rand2() % 30;
            }
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
        
        generateShadow(xs / 30, shadow1, true);
        generateShadow(xs / 20, shadow2, true);
        generateShadow(xs / 10, shadow3, true);
        
        reset = 0;
    }
    
    float tone = color[0];
    float *shadow;
    if (tone < 0.3) shadow = shadow3;
    else if (tone < 0.5) shadow = shadow2;
    else {
        shadow = shadow3;
        // color[0] = color[1] = color[2] = 1;
        // return GZ_SUCCESS;
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

void drawBrick(int minx, int miny, int maxx, int maxy)
{
    int *wave, *thick, length;
    int i, j, k, sub;
    int thickness = 2;
    
    
    for (i = minx; i <= maxx; i++)
        for (j = miny; j <= maxy; j++){
            sub = getSub(i, j);
            brickId[sub] = brickCount;
            //printf("%d\n", brickCount);
        }
    
    for (i = minx; i < maxx; i++) {
        length = maxx - minx + 10;
        wave = new int[length];
        thick = new int[length];

        getWaveAndThickness(length, thickness, thick, wave, 3);
        for (j = minx; j <= maxx; j++) {
            if (j < 0 || j >= xs) continue;
            for (k = 0; k <= thickness; k++) {
                sub = getSub(j, miny + wave[j - minx] + k);
                brick[sub] = 0;
                brickId[sub] = brickCount;
            }
        }
        
        delete [] wave;
        delete [] thick;
    }
    
    for (i = minx; i < maxx; i++) {
        length = maxx - minx + 10;
        wave = new int[length];
        thick = new int[length];
        
        getWaveAndThickness(length, thickness, thick, wave, 3);
        for (j = minx; j <= maxx; j++) {
            if (j < 0 || j >= xs) continue;
            for (k = 0; k <= thickness; k++) {
                sub = getSub(j, maxy + wave[j - minx] + k);
                brick[sub] = 0;
                brickId[sub] = brickCount;
            }
        }
        
        delete [] wave;
        delete [] thick;
    }
    
    for (i = miny; i < maxy; i++) {
        length = maxy - miny + 10;
        wave = new int[length];
        thick = new int[length];
        
        getWaveAndThickness(length, thickness, thick, wave, 3);
        for (j = miny; j <= maxy; j++) {
            if (j < 0 || j >= ys) continue;
            for (k = 0; k <= thickness; k++) {
                sub = getSub(minx + wave[j - miny] + k, j);
                brick[sub] = 0;
                brickId[sub] = brickCount;
            }
        }
        
        delete [] wave;
        delete [] thick;
    }
    
    for (i = miny; i < maxy; i++) {
        length = maxy - miny + 10;
        wave = new int[length];
        thick = new int[length];
        
        getWaveAndThickness(length, thickness, thick, wave, 3);
        for (j = miny; j <= maxy; j++) {
            if (j < 0 || j >= ys) continue;
            for (k = 0; k <= thickness; k++) {
                sub = getSub(maxx + wave[j - miny] + k, j);
                brick[sub] = 0;
                brickId[sub] = brickCount;
            }
        }
        
        delete [] wave;
        delete [] thick;
    }
    
    int white = rand2() % 100;
    while (white == prevR) {
        white = rand2() % 100;
    }
    prevR = white;
    
    //printf("w: %d\n", white);
    if (white > 40) {
        for (int i = minx; i < maxx; i++)
            for (int j = miny; j < maxy; j++) {
                sub = getSub(i, j);
                reservedForWhite[sub] = true;
            }
    }
}

void grass_fun(float grass[], int density, int grassLength) {
    int *wave, *len;
    int i, j, fromx, fromy;
    int baseline = 0;
    int grassThickness = 3;
    int bound;
    
    while (baseline < ys) {
        baseline += grassLength / 2;
        for (i = rand2() % density; i < xs; i+= density + rand2() % 10 - 5) {
            fromx = i;
            fromy = baseline + rand2() % 10;
            bound = grassLength + rand2() % 6;
            for (j = 0; j < bound; j++) {
                if (fromx > 0 && fromx < xs && fromy > 0 && fromy < ys) {
                    int sub = (fromy - 1)* xs + fromx;
                    grass[sub] = 0;
                }
                int r = rand2() % 4;
                if (r < 1) {
                    fromx += 1;
                }
                fromy++;
            }
        }
    }
    
}

/* Brick tex*/
int brick_fun(float u, float v, GzColor color)
{
    if (reset) {
        xs = ys = 1000;
        brick = new float[xs * ys];
        brickId = new int[xs * ys];
        grass = new float[xs * ys];
        reservedForWhite = new bool[xs * ys];
        
        for (int i = 0; i < xs * ys; i++) {
            brick[i] = 1;
            grass[i] = 1;
            reservedForWhite[i] = false;
            brickId[i] = 0;
        }
        
        int brickHeight = 50;
        int brickWidth = 100;
        int brickBlank = 15;
        brickCount = 101;
        
        int start[2] = {50, 0};
        int count = 0;
        int x, y;
        x = start[count];
        y = 10;
        
        while (y < ys) {
            drawBrick(x, y, x + brickWidth, y + brickHeight);
            brickCount++;
            if (x + brickWidth + brickBlank <= xs) {
                x += brickBlank + brickWidth;
            }
            else {
                count = 1 - count;
                x = start[count];
                y += brickHeight + brickBlank;
            }
        }
        
        // Generate the shadows.
        shadow1 = new float[xs * ys];
        shadow2 = new float[xs * ys];
        shadow3 = new float[xs * ys];
        
        for (int i =0; i < 8; i++) {
            shadows[i] = new float[xs * ys];
            if (shadows[i] == NULL) {
                printf("unable to create shadow tex.\n");
            }
        }
        
        for (int i = 0; i < xs * ys; i++) {
            /*
            shadow1[i] = brick[i];
            shadow2[i] = brick[i];
            shadow3[i] = brick[i];
            */
            shadow1[i] = 1;
            shadow2[i] = 1;
            shadow3[i] = 1;
            
            for (int j = 0; j < 8; j++) {
                if (j == 7) shadows[j][i] = brick[i];
                else shadows[j][i] = 1;
            }
        }        
        
        generateShadow(30, shadow1, false, true);
        generateShadow(20, shadow2, true, true);
        generateShadow(10, shadow3, true, true);
        
        for (int i = 0; i < 8; i++) {
            if (i < 4)
                generateShadow(i * 3, shadows[i], true, true, false);
            else if (i < 7)
                generateShadow(i * 3, shadows[i], false, true, false);
            else
                generateShadow(10, shadows[i], true, false);
        }

        
        //grass_fun(grass, 20, 25);
        
        reset = 0;
        /*
        for (int i = 0; i < xs; i++) {
            for (int j = 0; j < ys; j++) {
                if (brickId[getSub(i, j)] == 0)
                    printf("at %d, %d: %d\n", i, j, brickId[getSub(i, j)]);
            }
        }*/
    }
    
    /* bounds-test u,v to make sure nothing will overflow image array bounds */
    if (u < 0 || u > 1 || v < 0 || v > 1) {
        color[0] = color[1] = color[2] = 0;
        return GZ_FAILURE;
    }
    
    int x = u * (xs - 1);
    int y = v * (ys - 1);
    
    float tone = color[0];
    float *shadow;

    tone *= 10;
    if (tone >= 8) {
        color[0] = color[1] = color[2] = 1;
        return GZ_SUCCESS;
    }
    else {
        tone  = tone > 7 ? 7 : tone;
        shadow = shadows[(int)tone];
    }
    
    /*
    if (tone < 0.2) shadow = shadow3;
    else if (tone < 0.4) shadow = shadow2;
    else if (tone < 0.7) {
        shadow = shadow1;
        // color[0] = color[1] = color[2] = 1;
        // return GZ_SUCCESS;
    }
    else {
        color[0] = color[1] = color[2] = 1;
        return GZ_SUCCESS;
    }*/
    /*
    for (int i = 0; i < 3; i++)
        color[i] = brick[y * xs + x];
    */
    // return GZ_SUCCESS;
    
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
        subscript = subscriptsX[i] + (subscriptsY[i] - 1) * (xs);
        
        color[RED] += shadows[7][subscript] * combination[i];//((shadow[subscript] < 1 || shadows[7][subscript] < 1) ? 0 : 1) * combination[i];
        color[GREEN] += shadows[7][subscript] * combination[i];//((shadow[subscript] < 1 || shadows[7][subscript] < 1) ? 0 : 1) * combination[i];
        color[BLUE] += shadows[7][subscript] * combination[i];//((shadow[subscript] < 1 || shadows[7][subscript] < 1) ? 0 : 1) * combination[i];
        /*
        color[RED] += grass[subscript] * combination[i];
        color[GREEN] += grass[subscript] * combination[i];
        color[BLUE] += grass[subscript] * combination[i];
         */
    }
    
    //printf("return: %d, %d\n", x, y);
    return brickId[(int)y * xs + (int)x];
}

/* White */
int white_tex_fun(float u, float v, GzColor color)
{
    float value = 1;
    
    if (u < 0.5 && v < 0.5) value = 1;
    if (u > 0.5 && v > 0.5) value = 1;
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

