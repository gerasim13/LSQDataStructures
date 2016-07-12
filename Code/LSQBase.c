//
//  LSQBase.c
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 26.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#import "LSQBase.h"
#import "LSQCommon.h"

//________________________________________________________________________________________

#pragma mark - Data Structures

// Base type declaration
typedef struct LSQBaseType
{
    LSQBaseType_Data data;
    LSQBaseVtableRef vtable;
} LSQBaseType;

//________________________________________________________________________________________

#pragma mark - Base Functions

LSQBaseTypeRef NewLSQBaseType(LSQBaseVtableRef vtable, LSQBaseDeallocCallback dealloc_callback)
{
    // Allocate memory
    LSQBaseType *self = LSQAllocatorAllocType(LSQBaseType);
    // Set properties
    self->vtable        = vtable;
    self->data.dealloc  = dealloc_callback;
    self->data.refcount = 0;
    self->data.version  = 0;
    self->data.userdata = self;
    return self;
}

void* LSQBaseRetain(LSQBaseTypeRef self)
{
    ATOMICINCREMENT_INT32(self->data.refcount);
    // Execute callback
    if (self->vtable != NULL && self->vtable->retain != NULL)
    {
        self = self->vtable->retain(self->data.userdata);
    }
    // Return object
    return self;
}

void LSQBaseRelease(LSQBaseTypeRef self)
{
    ATOMICDECRIMENT_INT32(self->data.refcount);
    // Execute release callback
    if (self->vtable != NULL && self->vtable->release != NULL)
    {
        self->vtable->release(self->data.userdata);
    }
    // Execute deallock
    if (self->data.refcount <= 0)
    {
        LSQBaseDealloc(self);
    }
}

void LSQBaseDealloc(LSQBaseTypeRef self)
{
    if (self->data.dealloc != NULL)
    {
        self->data.dealloc(self->data.userdata);
    }
    ATOMICSWAP_PTR(self->data.userdata, NULL);
    LSQAllocatorDealloc(self);
}

int32_t LSQBaseGetRefCount(LSQBaseTypeRef self)
{
    return self->data.refcount;
}

void LSQBaseSetUserdata(LSQBaseTypeRef self, void* data)
{
    ATOMICSWAP_PTR(self->data.userdata, data);
}
