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

void drawLine(GzDisplay *display, int fromX, int fromY, int toX, int toY, int thickness)
{
    int i, length, count;
    float dx, dy, k, b;
    dx = absf(fromX - toX);
    dy = absf(fromY - toY);
    
    int *wave, *thick;
    
    if (dx != 0)
    {
        k = (float)(fromY - toY) / (float)(fromX - toX);
        b = fromY - k * fromX;
        if (dx > dy) {
            length = abs(fromX - toX) + 1;
            wave = new int[length];
            thick = new int[length];
            
            getWaveAndThickness(length, thickness, thick, wave);
            count = 0;
            
            for (i = min(fromX, toX); i <= max(fromX, toX); i++) {
                //GzPutDisplay(display, i, k * i + b, 0, 0, 0, 0, 10);
                //drawPoint(display, i, k * i + b, 0, 0, STOKE_DIRECTION_X);
                drawPoint(display, i, k * i + b, wave[count], thick[count], STOKE_DIRECTION_X);
                count++;
            }
        }
        else {
            length = abs(fromY - toY) + 1;
            wave = new int[length];
            thick = new int[length];
            
            getWaveAndThickness(length, thickness, thick, wave);
            count = 0;
            
            for (i = min(fromY, toY); i <= max(fromY, toY); i++) {
                //GzPutDisplay(display, (i - b) / k, i, 0, 0, 0, 0, 10);
                //drawPoint(display, (i - b) / k, i, 0, 0, STOKE_DIRECTION_Y);
                drawPoint(display, (i - b) / k, i, wave[count], thick[count], STOKE_DIRECTION_Y);
                count++;
            }
        }
    }
    else {
        length = abs(fromY - toY) + 1;
        wave = new int[length];
        thick = new int[length];
        
        getWaveAndThickness(length, thickness, thick, wave);
        count = 0;
        
        for (i = min(fromY, toY); i <= max(fromY, toY); i++) {
            //GzPutDisplay(display, fromX, i, 0, 0, 0, 0, 10);
            //drawPoint(display, fromX, i, 0, 0, STOKE_DIRECTION_Y);
            drawPoint(display, fromX, i, wave[count], thick[count], STOKE_DIRECTION_Y);
            count++;
        }
    }
}

void drawPoint(GzDisplay *display, int x, int y, int wave, int thickness, int direction)
{
    int i;
    
    if (direction == STOKE_DIRECTION_X) {
        
        for (i = 0; i <= thickness; i++)
        {
            GzPutDisplay(display, x, y + wave + i, 0, 0, 0, 0, 10);
        }
    }
    else {
        //printf("x: %d thick: %d\n", x + wave, thickness);
        for (i = 0; i <= thickness; i++)
        {
            GzPutDisplay(display, x + wave + i, y, 0, 0, 0, 0, 10);
        }
    }
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
            wave[i] = wave[i] < -1 * STROKE_WAVE_LIMIT ? -1 * STROKE_WAVE_LIMIT : wave[i];
        }
        else {
            wave[i] = wave[i - 1] + 1;
            wave[i] = wave[i] > STROKE_WAVE_LIMIT ? STROKE_WAVE_LIMIT : wave[i];
        }
        printf("wave: %d\n", wave[i]);
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
        //printf("thickness: %d\n", thickness[i]);
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
            printf("wave: %d\n", wave[i]);
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
            printf("wave: %d\n", wave[i]);
        }
        else {
            wave[i] = wave[i - 1];
            thickness[i] = thickness[i - 1];
        }
    }
}
