//
//  LSQNode.c
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 10.09.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#import "LSQNode.h"
#import "LSQCommon.h"

//________________________________________________________________________________________

#pragma mark - Data Structures

typedef struct LSQNode
{
    LSQBaseTypeRef base;
    LSQNode_Data   data;
} LSQNode;

//________________________________________________________________________________________

#pragma mark - Core Foundation Callbacks

void* LSQNodeRetainCFContent(LSQNodeRef node)
{
    if (node->data.content != NULL)
    {
        CFRetain(node->data.content);
    }
    return node->base;
}

void* LSQNodeReleaseCFContent(LSQNodeRef node)
{
    if (node->data.content != NULL)
    {
        CFRelease(node->data.content);
    }
    return node->base;
}

#pragma mark - CGColor Callbacks

void* LSQNodeRetainCGColorContent(LSQNodeRef node)
{
    if (node->data.content != NULL)
    {
        CGColorRetain((CGColorRef)node->data.content);
    }
    return node->base;
}

void* LSQNodeReleaseCGColorContent(LSQNodeRef node)
{
    if (node->data.content != NULL)
    {
        CGColorRetain((CGColorRef)node->data.content);
    }
    return node->base;
}

#pragma mark - CGPath Callbacks

void* LSQNodeRetainCGPathContent(LSQNodeRef node)
{
    if (node->data.content != NULL)
    {
        CGPathRetain((CGPathRef)node->data.content);
    }
    return node->base;
}

void* LSQNodeReleaseCGPathContent(LSQNodeRef node)
{
    if (node->data.content != NULL)
    {
        CGPathRelease((CGPathRef)node->data.content);
    }
    return node->base;
}

//________________________________________________________________________________________

const struct LSQBaseVtable kLSQNodeVtableCF = {
    (void*)LSQNodeRetainCFContent,
    (void*)LSQNodeReleaseCFContent
};

const struct LSQBaseVtable kLSQNodeVtableCGColor = {
    (void*)LSQNodeRetainCGColorContent,
    (void*)LSQNodeReleaseCGColorContent
};

const struct LSQBaseVtable kLSQNodeVtableCGPath = {
    (void*)LSQNodeRetainCGPathContent,
    (void*)LSQNodeReleaseCGPathContent
};

//________________________________________________________________________________________

#pragma mark - Base Functions

LSQNodeRef NewLSQNode(void *content, LSQBaseVtableRef vtable)
{
    // Create new node
    LSQNodeRef node = LSQALLOCK(LSQNode, vtable, (void*)&LSQNodeDealloc);
    // Set properties
    node->data.content = content;
    node->data.back    = NULL;
    node->data.front   = NULL;
    node->data.index   = -1;
    // Return node with data
    return node;
}

void* LSQNodeRetain(LSQNodeRef self)
{
    ATOMICSWAP_PTR(self->base, LSQBaseRetain(self->base));
    return self;
}

void LSQNodeRelease(LSQNodeRef self)
{
    LSQBaseRelease(self->base);
}

void LSQNodeDealloc(LSQNodeRef self)
{
    ATOMICSWAP_PTR(self->data.back, NULL);
    ATOMICSWAP_PTR(self->data.front, NULL);
    ATOMICSWAP_PTR(self->data.content, NULL);
    ATOMICSWAP_PTR(self->base, NULL);
    LSQAllocatorDealloc(self);
}

int32_t LSQNodeGetRefCount(LSQNodeRef self)
{
    return LSQBaseGetRefCount(self->base);
}

void* LSQNodeGetContent(LSQNodeRef self)
{
    return self->data.content;
}

LSQBaseTypeRef LSQNodeGetBase(LSQNodeRef self)
{
    return self->base;
}

CFIndex LSQNodeGetIndex(LSQNodeRef self)
{
    return self->data.index;
}

void LSQNodeSetIndex(LSQNodeRef self, CFIndex index)
{
    ATOMICSWAP_LONG(self->data.index, index);
}

LSQNodeRef LSQNodeGetFront(LSQNodeRef self)
{
    return self->data.front;
}

bool LSQNodeSetFront(LSQNodeRef self, LSQNodeRef node)
{
    return ATOMICSWAP_PTR(self->data.front, node);
}

LSQNodeRef LSQNodeGetBack(LSQNodeRef self)
{
    return self->data.back;
}

bool LSQNodeSetBack(LSQNodeRef self, LSQNodeRef node)
{
    return ATOMICSWAP_PTR(self->data.back, node);
}

bool LSQNodeSetFrontBack(LSQNodeRef self, LSQNodeRef node)
{
    return LSQNodeSetFront(self, node) && LSQNodeSetBack(self, node);
}

bool LSQNodeSetBackFront(LSQNodeRef self, LSQNodeRef node)
{
    return LSQNodeSetBack(self, node) && LSQNodeSetFront(self, node);
}

//________________________________________________________________________________________

#pragma mark - CoreFoundation Callbacks

const void * LSQNodeRetainCallBack(CFAllocatorRef allocator, const void *value)
{
    return LSQNodeRetain((LSQNodeRef)value);
}

void LSQNodeReleaseCallBack(CFAllocatorRef allocator, const void *value)
{
    LSQNodeRelease((LSQNodeRef)value);
}
