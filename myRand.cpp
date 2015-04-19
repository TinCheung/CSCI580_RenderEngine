//
//  myRand.cpp
//  CSCI580_RenderEngine
//
//  Created by Tan Tian Xiang on 15/4/19.
//  Copyright (c) 2015年 tantianx. All rights reserved.
//

#include "myRand.h"
#include <stdlib.h>

#define RAND_SIZE 100

bool recorded[RAND_SIZE];
int count = RAND_SIZE;

int rand2() {
    if (count > RAND_SIZE - 4) {
        for (int i = 0; i < RAND_SIZE; i++) {
            recorded[i] = false;
        }
    }
    
    int random = rand() % RAND_SIZE;
    while (recorded[random]) {
        random = rand() % RAND_SIZE;
    }
    recorded[random] = true;
    count++;
    return random;
}