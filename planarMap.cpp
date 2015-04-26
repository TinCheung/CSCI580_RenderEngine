//
//  planarMap.cpp
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 15/4/15.
//  Copyright (c) 2015年 tantianx. All rights reserved.
//
#include "planarMap.h"

void findEdgeTriangleIds(Triangle triangles[], int start, int num, GzPoint p1, GzPoint p2, int ids[], int *idCount)
{
    int i, j, next;
    int count = 0;
    GzPoint edge1[2], edge2[2];
    
    copyPoint(p1, edge1[0]);
    copyPoint(p2, edge1[1]);
    
    for (i = 0; i < num; i++) {
        if (i == start - 1) continue;
        for (j = 0; j < 3; j++) {
            next = j + 1 == 3 ? 0 : j + 1;
            copyPoint(triangles[i].vertexes[j], edge2[0]);
            copyPoint(triangles[i].vertexes[next], edge2[1]);
            if (isTheSameEdge(edge1, edge2)) {
                ids[count] = triangles[i].triangleId;
                count++;
            }
        }
    }
    
    *idCount = count > 2 ? 2 : count;
}

bool isAdded(vector<Edge> edges, Edge temp)
{
    int i;
    GzPoint edge1[2];
    GzPoint edge2[2];
    
    copyPoint(temp.endPoint1, edge1[0]);
    copyPoint(temp.endPoint2, edge1[1]);
    
    for (i = 0; i < edges.size(); i++) {
        copyPoint(edges[i].endPoint1, edge2[0]);
        copyPoint(edges[i].endPoint2, edge2[1]);
        
        if (isTheSameEdge(edge1, edge2))
            return true;
    }
    
    return false;
}

void getEdgesFromTriangles(Triangle triangles[], int num, vector<Edge> *edges)
{
    int i, j, k, next;
    Edge tempEdge;
    GzPoint p1, p2;
    int ids[2], idCount;
    
    for (i = 0; i < num; i++) {
        for (j = 0; j < 3; j++) {
            next = j + 1 == 3 ? 0 : j + 1;
            copyPoint(triangles[i].vertexes[j], p1);
            copyPoint(triangles[i].vertexes[next], p2);
            
            findEdgeTriangleIds(triangles, i + 1, num, p1, p2, ids, &idCount);
            
            copyPoint(triangles[i].vertexesInScreen[j], tempEdge.endPoint1);
            copyPoint(triangles[i].vertexesInScreen[next], tempEdge.endPoint2);
            
            for (k = 0; k < idCount; k++) {
                tempEdge.triangleIds[k] = ids[k];
            }
            tempEdge.triangleIds[1] = triangles[i].triangleId;
            
            tempEdge.edgeId = 3 * i + j;
            tempEdge.triangleCount = idCount + 1;
            tempEdge.D = triangles[i].D;
            
            if (idCount == 0)
                tempEdge.triangleIds[0] = triangles[i].triangleId;
            
            for (k = 0; k < 3; k++)
                tempEdge.normal[k] = triangles[i].normal[k];

            if (!isAdded((*edges), tempEdge))
                (*edges).push_back(tempEdge);
        }
    }
    
    i = 2;
}

void copyPoint(GzPoint from, GzPoint to)
{
    int i;
    for (i = 0; i < 4; i++)
        to[i] = from[i];
}


bool isTheSameEdge(GzPoint edge1[], GzPoint edge2[])
{
    if (isTheSamePoint(edge1[0], edge2[0]) && isTheSamePoint(edge1[1], edge2[1]))
        return true;
    if (isTheSamePoint(edge1[1], edge2[0]) && isTheSamePoint(edge1[0], edge2[1]))
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

bool checkEdgeTonesDiff(Triangle triangles[], int num, Edge edge)
{
    if (edge.triangleCount == 1) return true;
    else {
        int tri1 = getTriangleIndexById(triangles, num, edge.triangleIds[0]);
        int tri2 = getTriangleIndexById(triangles, num, edge.triangleIds[1]);
        
        return isToneDiff(triangles[tri1], triangles[tri2]);
    }
}

bool isAllVisible(Triangle triangles[], int num, bool visibility[], Edge edge)
{
    if (edge.triangleCount == 1) return false;
    else {
        return visibility[edge.triangleIds[0]] && visibility[edge.triangleIds[1]];
    }
}

int getTriangleIndexById(Triangle triangles[], int num, int triId)
{
    int i;
    for (i = 0; i < num; i++) {
        if (triangles[i].triangleId == triId)
            return i;
    }
    return -1;
}

bool isToneDiff(Triangle triangle1, Triangle triangle2)
{
    float threshold = 2.0;
    float ratio = triangle1.tone / triangle2.tone;
    ratio = ratio < 1 ? 1 / ratio : ratio;
    return ratio > threshold;
}