//
//  stroke.cpp
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 15/4/14.
//  Copyright (c) 2015年 tantianx. All rights reserved.
//

#include <stdlib.h>
#include <time.h>
#include "stroke.h"
#include "mathLib.h"
#include "planarMap.h"

void drawLineWith3DPoint(GzDisplay *display, int triangleId, GzPoint from, GzPoint to, GzVector normal, float D, int thickness)
{
    int i, length, count;
    float dx, dy, k, b, depth;
    dx = absf(from[0] - to[0]);
    dy = absf(from[1] - to[1]);
    
    int *wave, *thick;
    if (dx == 0 && dy == 0) return;
    
    if (dx != 0)
    {
        k = (float)(from[1] - to[1]) / (float)(from[0] - to[0]);
        b = from[1] - k * from[0];
        if (dx > dy) {
            length = (int)absf(from[0] - to[0]) + 1;
            wave = new int[length];
            thick = new int[length];
            
            getWaveAndThickness(length, thickness, thick, wave);
            count = 0;
            
            for (i = min(from[0], to[0]); i <= max(from[0], to[0]); i++) {
                depth = getZValue(normal, D, i, k * i + b);
                drawPoint(display, triangleId, i, k * i + b, wave[count], thick[count], STOKE_DIRECTION_X, depth);
                count++;
            }
        }
        else {
            length = (int)absf(from[1] - to[1]) + 1;
            wave = new int[length];
            thick = new int[length];
            
            getWaveAndThickness(length, thickness, thick, wave);
            count = 0;
            
            for (i = min(from[1], to[1]); i <= max(from[1], to[1]); i++) {
                depth = getZValue(normal, D, (i - b) / k, i);
                drawPoint(display, triangleId, (i - b) / k, i, wave[count], thick[count], STOKE_DIRECTION_Y, depth);
                count++;
            }
        }
    }
    else {
        length = (int)absf(from[1] - to[1]) + 1;
        wave = new int[length];
        thick = new int[length];
        
        getWaveAndThickness(length, thickness, thick, wave);
        count = 0;
        
        for (i = min(from[1], to[1]); i <= max(from[1], to[1]); i++) {
            depth = getZValue(normal, D, from[0], i);
            drawPoint(display, triangleId, from[0], i, wave[count], thick[count], STOKE_DIRECTION_Y, depth);
            count++;
        }
    }
}

void drawEdge(GzDisplay *display, Edge edge, int thickness)
{
    int i, j, length, count;
    float dx, dy, k, b, depth;
    
    float from[2], to[2];
    
    from[0] = edge.endPoint1[0];
    from[1] = edge.endPoint1[1];
    to[0] = edge.endPoint2[0];
    to[1] = edge.endPoint2[1];
    
    GzPoint normal;
    copyPoint(edge.normal, normal);
    float D = edge.D;
    
    dx = absf(from[0] - to[0]);
    dy = absf(from[1] - to[1]);
    
    int *wave, *thick;
    if (dx == 0 && dy == 0) return;
    
    if (dx != 0)
    {
        k = (float)(from[1] - to[1]) / (float)(from[0] - to[0]);
        b = from[1] - k * from[0];
        if (dx > dy) {
            length = (int)absf(from[0] - to[0]) + 10;
            wave = new int[length];
            thick = new int[length];
            
            getWaveAndThickness(length, thickness, thick, wave);
            count = 0;
            
            for (i = min(from[0], to[0]); i <= max(from[0], to[0]); i++) {
                depth = getZValue(normal, D, i, k * i + b);
                for (j = 0; j < edge.triangleCount; j++) {
                    drawPoint(display, edge.triangleIds[j], i, k * i + b, wave[count], thick[count], STOKE_DIRECTION_X, depth);
                }
                count++;
            }
        }
        else {
            length = (int)absf(from[1] - to[1]) + 10;
            wave = new int[length];
            thick = new int[length];
            
            getWaveAndThickness(length, thickness, thick, wave);
            count = 0;
            
            for (i = min(from[1], to[1]); i <= max(from[1], to[1]); i++) {
                depth = getZValue(normal, D, (i - b) / k, i);
                for (j = 0; j < edge.triangleCount; j++) {
                    drawPoint(display, edge.triangleIds[j], (i - b) / k, i, wave[count], thick[count], STOKE_DIRECTION_Y, depth);
                }
                count++;
            }
        }
    }
    else {
        length = (int)absf(from[1] - to[1]) + 10;
        wave = new int[length];
        thick = new int[length];
        
        getWaveAndThickness(length, thickness, thick, wave);
        count = 0;
        
        for (i = min(from[1], to[1]); i <= max(from[1], to[1]); i++) {
            depth = getZValue(normal, D, from[0], i);
            for (j = 0; j < edge.triangleCount; j++) {
                drawPoint(display, edge.triangleIds[j], from[0], i, wave[count], thick[count], STOKE_DIRECTION_Y, depth);
            }
            count++;
        }
    }
    
    depth = getZValue(normal, D, from[0], from[1]);
    drawEndPoint(display, edge.triangleIds[0], (int)from[0], (int)from[1], thickness, depth);
    depth = getZValue(normal, D, to[0], to[1]);
    drawEndPoint(display, edge.triangleIds[0], (int)to[0], (int)to[1], thickness, depth);
    
    if (edge.triangleCount > 1) {
        depth = getZValue(normal, D, from[0], from[1]);
        drawEndPoint(display, edge.triangleIds[1], (int)from[0], (int)from[1], thickness / 2, depth);
        depth = getZValue(normal, D, to[0], to[1]);
        drawEndPoint(display, edge.triangleIds[1], (int)to[0], (int)to[1], thickness, depth);
    }
    
    delete [] wave;
    delete [] thick;
}

void drawEndPoint(GzDisplay *display, int triangleId, int x, int y, int thickness, int depth)
{
    int i, j;
    
    for (i = x - thickness; i <= x + thickness; i++)
        for (j = y - thickness; j <= y + thickness; j++)
        {
            if (abs(i + j - x - y) <= thickness)
            {
                GzPutDisplay(display, i, j, 0, 0, 0, 0, depth);
            }
        }
}

void drawPoint(GzDisplay *display, int triangleId, int x, int y, int wave, int thickness, int direction, int depth)
{
    int i;
    
    if (direction == STOKE_DIRECTION_X)
        for (i = 0; i <= thickness; i++)
            GzPutDisplayExt(display, x, y + wave + i, 0, 0, 0, 0, depth, triangleId, ZBUFFER_EDGE);
    else
        for (i = 0; i <= thickness; i++)
            GzPutDisplayExt(display, x + wave + i, y, 0, 0, 0, 0, depth, triangleId, ZBUFFER_EDGE);
}

void getStrokeWave(int length, int wave[])
{
    int i;
    wave[0] = 0;
    srand((unsigned int)time(0));
    
    for (i = 1; i < length; i++)
    {
        int randomNum = rand() % 20;
        if (randomNum < 17) wave[i] = wave[i - 1];
        else if (randomNum == 18) {
            wave[i] = wave[i - 1] - 1;
            wave[i] = wave[i] < 0 ? 0 : wave[i];
        }
        else {
            wave[i] = wave[i - 1] + 1;
            wave[i] = wave[i] > STROKE_WAVE_LIMIT ? STROKE_WAVE_LIMIT : wave[i];
        }
    }
}

void getStrokeThickness(int length, int thick, int thickness[])
{
    int i, initThick, upperBound;
    thickness[0] = 0;
    srand((unsigned int)time(0));
    
    initThick = thick;
    upperBound = thick + THICKNESS_VARY_LIMIT;
    
    for (i = 1; i < length; i++)
    {
        int randomNum = rand() % 20;
        if (randomNum < 18) {
            thickness[i] = thick;
        }
        else if (randomNum == 18) {
            thick = thick - 1 < initThick ? initThick : thick - 1;
            thickness[i] = thick;
        }
        else {
            thick = thick + 1 > upperBound ? upperBound : thick + 1;
            thickness[i] = thick;
        }
    }
    
}

void getWaveAndThickness(int length, int thick, int thickness[], int wave[])
{
    int i, initThick, upperBound;
    srand((unsigned int)time(0));
    
    initThick = thick;
    upperBound = thick + THICKNESS_VARY_LIMIT;
    
    wave[0] = 0;
    thickness[0] = thick;
    
    for (i = 1; i < length; i++) {
        int randomNum = rand() % 4;
        if (randomNum == 1)
        {
            randomNum = rand() % 20;
            if (randomNum < 17) wave[i] = wave[i - 1];
            else if (randomNum == 18) {
                wave[i] = wave[i - 1] - 1;
                wave[i] = wave[i] < -1 * STROKE_WAVE_LIMIT ? -1 * STROKE_WAVE_LIMIT : wave[i];
            }
            else {
                wave[i] = wave[i - 1] + 1;
                wave[i] = wave[i] > STROKE_WAVE_LIMIT ? STROKE_WAVE_LIMIT : wave[i];
            }
            thickness[i] = thickness[i - 1];
        }
        else if (randomNum == 0){
            randomNum = rand() % 20;
            if (randomNum < 18) {
                thickness[i] = thick;
            }
            else if (randomNum == 18) {
                thick = thick - 1 < initThick ? initThick : thick - 1;
                thickness[i] = thick;
            }
            else {
                thick = thick + 1 > upperBound ? upperBound : thick + 1;
                thickness[i] = thick;
            }
            wave[i] = wave[i - 1];
        }
        else {
            wave[i] = wave[i - 1];
            thickness[i] = thickness[i - 1];
        }
    }
}

void getWaveAndThickness(int length, int thick, int thickness[], int wave[], int waveLimit)
{
    int i, initThick, upperBound;
    srand((unsigned int)time(0));
    
    initThick = thick;
    upperBound = thick + THICKNESS_VARY_LIMIT;
    
    wave[0] = 0;
    thickness[0] = thick;
    
    for (i = 1; i < length; i++) {
        int randomNum = rand() % 2;
        if (randomNum == 1)
        {
            randomNum = rand() % 20;
            if (randomNum < 10) wave[i] = wave[i - 1];
            else if (randomNum < 15) {
                wave[i] = wave[i - 1] - 1;
                wave[i] = wave[i] < -1 * waveLimit ? -1 * waveLimit : wave[i];
            }
            else {
                wave[i] = wave[i - 1] + 1;
                wave[i] = wave[i] > waveLimit ? waveLimit : wave[i];
            }
            thickness[i] = thickness[i - 1];
        }
        else if (randomNum == 0){
            randomNum = rand() % 20;
            if (randomNum < 18) {
                thickness[i] = thick;
            }
            else if (randomNum == 18) {
                thick = thick - 1 < initThick ? initThick : thick - 1;
                thickness[i] = thick;
            }
            else {
                thick = thick + 1 > upperBound ? upperBound : thick + 1;
                thickness[i] = thick;
            }
            wave[i] = wave[i - 1];
        }
        else {
            wave[i] = wave[i - 1];
            thickness[i] = thickness[i - 1];
        }
    }
}
