//
//  LSQBase.c
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 26.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#include "LSQBase.h"
#include <stdio.h>
#include <libkern/OSAtomic.h>

#pragma mark - Base Functions

LSQTypeRef LSQTypeNew(LSQVTableRef vtable)
{
    // Allocate memory
    struct base_type *self = LSQAllocatorAllocType(base_type);
    // Set properties
    self->vtable    = vtable;
    self->refcount  = 0;
    self->version   = 0;
    return (LSQTypeRef)self;
}

void* LSQBaseRetain(void* self)
{
    bool success = false;
    // Increment counter
    while (!success)
    {
        success = OSAtomicIncrement32(&((LSQTypeRef)self)->refcount);
    }
    // Return object
    return self;
}

void LSQBaseRelease(void* self)
{
    bool success = false;
    // Decrement counter
    while (!success)
    {
        success = OSAtomicDecrement32(&((LSQTypeRef)self)->refcount);
    }
}

void LSQBaseDealloc(void* self)
{
    LSQAllocatorDealloc(self);
}
