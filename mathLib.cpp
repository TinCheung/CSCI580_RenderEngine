//
//  mathLib.cpp
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 1/22/15.
//  Copyright (c) 2015 tantianx. All rights reserved.
//

#include "mathLib.h"

int scale(short v, int max, int tMax)
{
    double k = (double)tMax / (double)max;
    return (short)((double)v * k);
}