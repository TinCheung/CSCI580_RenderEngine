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

using namespace std;

struct PlanarMap {
    Edge *edges;    // Inner edges.
    Edge *contour;  
    int size;
};

typedef struct PlanarMap GzPlanarMap;
bool isTheSameEdge(GzPoint points1[], GzPoint points2[]);
void copyPoint(GzPoint from, GzPoint to);
bool isTheSamePoint(GzPoint p1, GzPoint p2);

// Search operations.
void findEdgeTriangleIds(Triangle triangles[], int start, int num, GzPoint p1, GzPoint p2, int ids[], int *idCount);
void getEdgesFromTriangles(Triangle triangles[], int num, vector<Edge> *edges);

#endif /* defined(__CSCI580_RenderEngine__planarMap__) */
