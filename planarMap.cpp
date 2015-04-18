//
//  planarMap.cpp
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 15/4/15.
//  Copyright (c) 2015年 tantianx. All rights reserved.
//
#include "planarMap.h"

void constructPlanarMap(GzPlanarMap planarMap, Triangle triangles[], int triangleNum, bool visible[])
{
    int i, j;
    
    planarMap.size = 0;
    planarMap.edges = new Edge[triangleNum * 3];
    
    for (i = 0; i < triangleNum; i++)
    {
        if (!visible[i]) continue;
        
        for (j = i + 1; j < triangleNum; j++)
        {
            addEdge(planarMap, triangles[i], triangles[j]);
        }
    }
    
    
}

void addEdge(GzPlanarMap planarMap, Triangle t1, Triangle t2)
{
    int i, j;
    GzPoint end1[2];
    GzPoint end2[2];
    
    for (i = 0; i < 3; i++) {
        copyPoint(t1.vertexes[i], end1[0]);
        copyPoint(t1.vertexes[i + 1 == 3 ? 0 : i + 1], end1[1]);
        for (j = 0; j < 3; j++) {
            copyPoint(t2.vertexes[j], end2[0]);
            copyPoint(t2.vertexes[j + 1 == 3 ? 0 : j + 1], end2[1]);
            
            if (isTheSameEdge(end1, end2)) {
                int sub = planarMap.size;
                
                planarMap.edges[sub].edgeId = sub;
                planarMap.edges[sub].fromX = end1[0][0];
                planarMap.edges[sub].fromY = end1[0][1];
                planarMap.edges[sub].toX = end1[1][0];
                planarMap.edges[sub].toY = end1[1][1];
                planarMap.edges[sub].triangleCount = 2;
                planarMap.edges[sub].triangleIds[0] = t1.triangleId;
                planarMap.edges[sub].triangleIds[1] = t2.triangleId;
                
                planarMap.size++;
            }
        }
    }
}

void copyPoint(GzPoint from, GzPoint to)
{
    int i;
    for (i = 0; i < 4; i++)
        to[i] = from[i];
}


bool isTheSameEdge(GzPoint points1[], GzPoint points2[])
{
    if (isTheSamePoint(points1[0], points2[0]) && isTheSamePoint(points1[1], points2[1]))
        return true;
    if (isTheSamePoint(points1[1], points2[0]) && isTheSamePoint(points1[0], points2[1]))
        return true;
    
    return false;
}

bool isTheSamePoint(GzPoint p1, GzPoint p2)
{
    int i;
    for (i = 0; i < 3; i++)
        if (p1[i] != p2[i])
            return false;
    return true;
}