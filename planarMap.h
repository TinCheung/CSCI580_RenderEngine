//
//  planarMap.h
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 15/4/15.
//  Copyright (c) 2015年 tantianx. All rights reserved.
//

#ifndef __CSCI580_RenderEngine__planarMap__
#define __CSCI580_RenderEngine__planarMap__

#include <stdio.h>
#include <vector>
#include "planarMap.h"
#include "gz.h"

struct PlanarMap {
    Edge *edges;    // Inner edges.
    Edge *contour;  
    int size;
};

typedef struct PlanarMap GzPlanarMap;
void addEdge(GzPlanarMap planarMap, Triangle t1, Triangle t2);
bool isTheSameEdge(GzPoint points1[], GzPoint points2[]);
void copyPoint(GzPoint from, GzPoint to);
void constructPlanarMap(GzPlanarMap planarMap, Triangle triangles[], int triangleNum, bool visible[]);
bool isTheSamePoint(GzPoint p1, GzPoint p2);

#endif /* defined(__CSCI580_RenderEngine__planarMap__) */
