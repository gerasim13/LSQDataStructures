//
//  LSQArray.c
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 27.10.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#include "LSQArray.h"
#include <stdio.h>
#include <libkern/OSAtomic.h>

//________________________________________________________________________________________

#pragma mark - Data Structures

// Array data
typedef struct LSQArray
{
    LSQBaseTypeRef    base;
    LSQBaseVtableRef  callbacks;
    LSQArrayVtableRef vtable;
    LSQArray_Data     data;
} LSQArray;

// Error codes
enum
{
    LSQArrayError_InvalidArgs    = 999999L << 1,
    LSQArrayError_OutOfMemory    = 999999L << 2,
    LSQArrayError_IndexOutBounds = 999999L << 3,
    LSQArrayError_ArrayNotInit   = 999999L << 4
};

//________________________________________________________________________________________

#pragma mark - Hidden Functions

OSStatus insert_at_index_check(LSQArrayRef array, CFIndex index, LSQNodeRef node)
{
    // Return error code for insert function
    if (array == NULL || node == NULL)
    {
        return LSQArrayError_InvalidArgs;
    }
    else if (index >= array->data.capacity)
    {
        return LSQArrayError_IndexOutBounds;
    }
    else if (array->data.elements == NULL)
    {
        return LSQArrayError_ArrayNotInit;
    }
    return noErr;
}

OSStatus remove_at_index_check(LSQArrayRef array, CFIndex index)
{
    // Return error code for remove function
    if (array == NULL)
    {
        return LSQArrayError_InvalidArgs;
    }
    else if (index > array->data.capacity)
    {
        return LSQArrayError_IndexOutBounds;
    }
    else if (array->data.elements == NULL)
    {
        return LSQArrayError_ArrayNotInit;
    }
    return noErr;
}

OSStatus remove_all_check(LSQArrayRef array)
{
    // Return error code for remove all function
    if (array == NULL)
    {
        return LSQArrayError_InvalidArgs;
    }
    else if (array->data.elements == NULL)
    {
        return LSQArrayError_ArrayNotInit;
    }
    return noErr;
}

OSStatus get_node_at_index_check(LSQArrayRef array, CFIndex index)
{
    // Return error code for get function
    if (array == NULL)
    {
        return LSQArrayError_InvalidArgs;
    }
    else if (index < 0 || index > array->data.capacity)
    {
        return LSQArrayError_IndexOutBounds;
    }
    else if (array->data.elements == NULL)
    {
        return LSQArrayError_ArrayNotInit;
    }
    return noErr;
}

bool try_insert_node(LSQArrayRef array, CFIndex index, LSQNodeRef node)
{
    // Try to insert item at index
    bool success= false;
    while (!success)
    {
        LSQNodeRef old = array->data.elements[index];
        // Remove old node
        if (old != NULL && array->vtable->remove_node != NULL)
        {
            array->vtable->remove_node(array, index);
        }
        // Insert new node
        if (node != NULL)
        {
            LSQNodeRetain(node);
        }
        success = OSAtomicCompareAndSwapPtr(array->data.elements[index], node, (void* volatile*)&array->data.elements[index]);
    }
    return (array->data.elements[index] == node);
}

bool try_remove_node(LSQArrayRef array, CFIndex index)
{
    // Try to remove item at index
    bool success = false;
    while (!success)
    {
        LSQNodeRef node = array->data.elements[index];
        // Remove node
        if (node == array->data.elements[index])
        {
            if (node != NULL)
            {
                LSQNodeRelease(node);
            }
            // Check index
            if (index < array->data.count - 1)
            {
                // Shift elements
                LSQNodeRef next = array->data.elements[index + 1];
                // Move memory blocks
                memmove(&array->data.elements[index],
                        &array->data.elements[index + 1],
                        (array->data.count - index - 1) * sizeof(LSQNodeRef));
                // Put next node at removed index
                success = OSAtomicCompareAndSwapPtr(array->data.elements[index], next, (void* volatile*)&array->data.elements[index]);
            }
            else
            {
                // Delete element
                success = OSAtomicCompareAndSwapPtr(node, NULL, (void* volatile*)&array->data.elements[index]);
            }
        }
    }
    return success;
}

bool try_realloc_array(LSQArrayRef array, uint32_t capacity)
{
    bool success = false;
    LSQNodeRef *tmp = NULL;
    // Realloc array
    while (!success)
    {
        tmp = LSQAllocatorRealloc(array->data.elements, sizeof(LSQNodeRef) * capacity);
        success = (tmp != NULL);
    }
    // Update tmp content
    for (int i = array->data.count; i < capacity; ++i)
    {
        tmp[i] = NULL;
    }
    // Swap elements with tmp
    success = false;
    while (!success)
    {
        success = OSAtomicCompareAndSwapPtr(array->data.elements, tmp, (void* volatile*)&array->data.elements);
    }
    // Update capacity
    success = false;
    while (!success)
    {
        success = OSAtomicCompareAndSwap32(array->data.capacity, capacity, (volatile int32_t*)&array->data.capacity);
    }
    return success;
}

//________________________________________________________________________________________

#pragma mark - Private Functions

OSStatus insert_node(LSQArrayRef self, CFIndex index, LSQNodeRef node)
{
    OSStatus status;
    switch ((status = insert_at_index_check(self, index, node)))
    {
        case LSQArrayError_InvalidArgs:
        case LSQArrayError_OutOfMemory:
        {
            break;
        }
        case LSQArrayError_ArrayNotInit:
        case LSQArrayError_IndexOutBounds:
        {
            if (!try_realloc_array(self, (uint32_t)index + 1))
            {
                break;
            }
        }
        default:
        {
            // Insert new node at index
            if (try_insert_node(self, index, node))
            {
                // Update count
                bool success = false;
                while (!success)
                {
                    success = OSAtomicIncrement32(&self->data.count);
                }
            }
            break;
        }
    }
    return status;
}

OSStatus remove_node(LSQArrayRef self, CFIndex index)
{
    OSStatus status;
    if ((status = remove_at_index_check(self, index)) != noErr)
    {
        return status;
    }
    // Try to remove node
    if (try_remove_node(self, index))
    {
        // Update count
        bool success = false;
        while (!success && self->data.count >= 1)
        {
            success = OSAtomicDecrement32(&self->data.count);
        }
    }
    return noErr;
}

OSStatus remove_all(LSQArrayRef self)
{
    OSStatus status;
    if ((status = remove_all_check(self)) != noErr)
    {
        return status;
    }
    // Iterate over all elements
    if (self->data.count > 0 && self->vtable->remove_node != NULL)
    {
        while (self->data.count > 0)
        {
            self->vtable->remove_node(self, 0);
        }
    }
    return noErr;
}

OSStatus array_get_node(LSQArrayRef self, CFIndex index, LSQNodeRef* outNode)
{
    OSStatus status;
    if ((status = get_node_at_index_check(self, index)) != noErr)
    {
        return status;
    }
    // Get node
    *outNode = self->data.elements[index];
    LSQNodeSetIndex(*outNode, index);
    return noErr;
}

void block_enumerate(LSQArrayRef self, CFRange range, LSQArrayBlock block)
{
    if (self != NULL)
    {
        for (CFIndex i = range.location; i < range.length; ++i)
        {
            LSQNodeRef node;
            if (self->vtable->get_node(self, i, &node) == noErr)
            {
                block(LSQNodeGetContent(node), i);
            }
        }
    }
}

//________________________________________________________________________________________

static const struct LSQArrayVtable kLSQArrayDefaultVtable = {
    &insert_node,
    &remove_node,
    &remove_all,
    &array_get_node,
    &block_enumerate
};

//________________________________________________________________________________________

#pragma mark - Base Functions

LSQArrayRef NewLSQArray(CFIndex capacity, LSQBaseVtableRef vtable)
{
    // Create new Array
    LSQArrayRef array = LSQALLOCK(LSQArray, NULL, (void*)&LSQArrayDealloc);
    // Set properties
    array->vtable        = &kLSQArrayDefaultVtable;
    array->callbacks     = vtable;
    array->data.elements = LSQAllocatorAllocSize(sizeof(LSQNodeRef) * capacity);
    array->data.capacity = (int32_t)capacity;
    array->data.count    = 0;
    // Fill array with NULL
    for (CFIndex i = 0; i < capacity; ++i)
    {
        array->data.elements[i] = NULL;
    }
    return array;
}

void* LSQArrayRetain(LSQArrayRef self)
{
    self->base = LSQBaseRetain(self->base);
    return self;
}

void LSQArrayRelease(LSQArrayRef self)
{
    LSQBaseRelease(self->base);
}

void LSQArrayDealloc(LSQArrayRef self)
{
    // Release elements
    if (self->vtable->remove_all != NULL)
    {
        self->vtable->remove_all(self);
    }
    // Dealloc array
    LSQAllocatorDealloc(self->data.elements);
    LSQAllocatorDealloc(self);
}

//________________________________________________________________________________________

#pragma mark - Public functions

void LSQArrayInsertValueAtIndex(LSQArrayRef self, CFIndex index, void* value)
{
    if (value != NULL && self != NULL && self->vtable->insert_node != NULL)
    {
        LSQNodeRef node = NewLSQNode(value, self->callbacks);
        self->vtable->insert_node(self, index, node);
    }
}

void LSQArrayRemoveValueAtIndex(LSQArrayRef self, CFIndex index)
{
    if (self != NULL && self->vtable->remove_node != NULL)
    {
        self->vtable->remove_node(self, index);
    }
}

void LSQArrayRemoveAllValues(LSQArrayRef self)
{
    if (self != NULL && self->vtable->remove_all != NULL)
    {
        self->vtable->remove_all(self);
    }
}

void* LSQArrayGetValueAtIndex(LSQArrayRef self, CFIndex index)
{
    if (self != NULL && self->vtable->get_node != NULL)
    {
        LSQNodeRef node;
        if (self->vtable->get_node(self, index, &node) == noErr)
        {
            return LSQNodeGetContent(node);
        }
    }
    return NULL;
}

void LSQArrayEnumerate(LSQArrayRef self, CFRange range, LSQArrayBlock block)
{
    if (self != NULL && self->vtable->block_enumerate != NULL)
    {
        self->vtable->block_enumerate(self, range, block);
    }
}

CFIndex LSQArrayGetValueIndex(LSQArrayRef self, void* value)
{
    if (self != NULL && value != NULL)
    {
        // Seqrch for node with value
        for (int i = 0; i < self->data.count; ++i)
        {
            LSQNodeRef node = NULL;
            if (self->vtable->get_node(self, i, &node) == noErr &&
                node != NULL && LSQNodeGetContent(node) == value)
            {
                return i;
            }
        }
    }
    return NAN;
}

CFIndex LSQArrayGetCount(LSQArrayRef self)
{
    if (self != NULL)
    {
        return self->data.count;
    }
    return 0;
}

CFIndex LSQArrayGetCapacity(LSQArrayRef self)
{
    if (self != NULL)
    {
        return self->data.capacity;
    }
    return 0;
}
