//
//  LSQHashMap.m
//  TheMightyMIDISequencer
//
//  Created by Павел Литвиненко on 20.03.14.
//  Copyright 2014 Casual Underground. All rights reserved.
//

#import "LSQHashMap.h"
#import "LSQBase.h"

//________________________________________________________________________________________

#pragma mark - Data Structures

typedef struct LSQHashMap
{
    LSQBaseTypeRef base;
    struct LSQHashMap_Data data;
} LSQHashMap;

//________________________________________________________________________________________

#pragma mark - Base Functions

LSQHashMapRef NewLSQHashMap ()
{
    LSQHashMapRef self = LSQALLOCK(LSQHashMap, NULL, (void*)&LSQHashMapDealloc);
    return self;
}

LSQHashMapRef LSQHashMapRetain (LSQHashMapRef self)
{
    self->base = LSQBaseRetain(self->base);
    return self;
}

void LSQHashMapRelease (LSQHashMapRef self)
{
    LSQBaseRelease(self->base);
}

void LSQHashMapDealloc (LSQHashMapRef self)
{
    LSQAllocatorDealloc(self);
}