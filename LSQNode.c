//
//  LSQNode.c
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 10.09.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#include "LSQNode.h"
#include <stdio.h>
#include <libkern/OSAtomic.h>

#pragma mark - Base Functions

void* LSQNodeRetain_Internal(void* self)
{
    LSQNode _self = (LSQNode)self;
    return LSQSuperRetain(_self);
}

void LSQNodeRelease_Internal(void* self)
{
    LSQNode _self = (LSQNode)self;
    LSQSuperRelease(_self);
}

void LSQNodeDealloc_Internal(void* self)
{
    LSQNode _self = (LSQNode)self;
    // Dealloc super
    LSQSuperDealloc(_self);
}

#pragma mark - Public functions

LSQNode LSQNodeMake(const void *data)
{
    // Create new node
    LSQNode node = LSQInit(LSQNode, &LSQNodeRetain_Internal, &LSQNodeRelease_Internal, &LSQNodeDealloc_Internal);
    // Set properties
    node->data  = data;
    node->back  = NULL;
    node->front = NULL;
    node->index = NAN;
    // Return node with data
    return node;
}

LSQNode LSQNodeRetain(LSQNode self)
{
    return LSQNodeRetain_Internal(self);
}

void LSQNodeRelease(LSQNode self)
{
    LSQNodeRelease_Internal(self);
}
