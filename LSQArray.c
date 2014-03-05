//
//  LSQArray.c
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 27.10.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#include "LSQArray.h"
#include <stdio.h>
#include <libkern/OSAtomic.h>

const struct LSQArrayCallbacks kLSQArrayCFCallbacks = { CFRetain, CFRelease };

#pragma mark - Base Functions

void* LSQArrayRetain_Internal(void* self)
{
    LSQArray _self = (LSQArray)self;
    return LSQSuperRetain(_self);
}

void LSQArrayRelease_Internal(void* self)
{
    LSQArray _self = (LSQArray)self;
    LSQSuperRelease(_self);
}

void LSQArrayDealloc_Internal(void* self)
{
    LSQArray _self = (LSQArray)self;
    // Release elements
    if (_self->remove_all != NULL)
    {
        _self->remove_all(_self);
    }
    LSQBaseDealloc(_self->elements);
    // Dealloc super
    LSQSuperDealloc(_self);
}

#pragma mark - Hidden Functions

// Return error code for insert function
LSQError LSQArrayInsertErrorCode(LSQArray array, CFIndex index, LSQNode node)
{
    LSQError error;
    if (array == NULL || node == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_InvalidArgs);
    }
    else if (index >= array->capacity)
    {
        LSQErrorMake(&error, LSQArrayError_IndexOutBounds);
    }
    else if (array->elements == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_ArrayNotInit);
    }
    else
    {
        LSQErrorMake(&error, LSQArrayError_NoError);
    }
    return error;
}

// Return error code for remove function
LSQError LSQArrayRemoveErrorCode(LSQArray array, CFIndex index)
{
    LSQError error;
    if (array == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_InvalidArgs);
    }
    else if (index > array->capacity)
    {
        LSQErrorMake(&error, LSQArrayError_IndexOutBounds);
    }
    else if (array->elements == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_ArrayNotInit);
    }
    else
    {
        LSQErrorMake(&error, LSQArrayError_NoError);
    }
    return error;
}

// Return error code for remove function
LSQError LSQArrayRemoveAllErrorCode(LSQArray array)
{
    LSQError error;
    if (array == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_InvalidArgs);
    }
    else if (array->elements == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_ArrayNotInit);
    }
    else
    {
        LSQErrorMake(&error, LSQArrayError_NoError);
    }
    return error;
}

// Return error code for get function
LSQError LSQArrayGetErrorCode(LSQArray array, CFIndex index)
{
    LSQError error;
    if (array == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_InvalidArgs);
    }
    else if (index < 0 || index > array->capacity)
    {
        LSQErrorMake(&error, LSQArrayError_IndexOutBounds);
    }
    else if (array->elements == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_ArrayNotInit);
    }
    else
    {
        LSQErrorMake(&error, LSQArrayError_NoError);
    }
    return error;
}

// Try to insert item at index
bool LSQArrayTryInsertNode(LSQArray array, CFIndex index, LSQNode node)
{
    // Keep trying
    while (true)
    {
        LSQNode old = array->elements[index];
        // Remove old node
        if (old != NULL && old->data != NULL && array->remove_node != NULL)
        {
            array->remove_node(array, index);
        }
        // Insert new node
        if (old == array->elements[index])
        {
            OSAtomicCompareAndSwapPtr(old, node, (void* volatile*)&array->elements[index]);
        }
        // Break loop
        if (array->elements[index] == node)
        {
            break;
        }
    }
    return true;
}

// Try to remove item at index
bool LSQArrayTryRemoveNode(LSQArray array, CFIndex index)
{
    // Keep trying
    while (true)
    {
        LSQNode old = array->elements[index];
        // Remove old node
        if (old == array->elements[index])
        {
            OSAtomicCompareAndSwapPtr(old, NULL, (void* volatile*)&array->elements[index]);
        }
        // Release old node
        if (old != NULL && array->node_release != NULL)
        {
            array->node_release(array, old);
        }
        // Break loop
        if (array->elements[index] == NULL)
        {
            break;
        }
    }
    return true;
}

LSQNode LSQArrayUpdateNodeIndexAndReturn(LSQNode node, CFIndex index)
{
    if (node != NULL && OSAtomicCompareAndSwapLong(node->index, index, &node->index))
    {
        return node;
    }
    // Return node anyway
    return node;
}

#pragma mark - Private Functions

LSQError insert_node(void* self, CFIndex index, LSQNode node)
{
    if (self != NULL)
    {
        LSQArray array = (LSQArray)self;
        LSQError error = LSQArrayInsertErrorCode(array, index, node);
        switch (error.code)
        {
            case LSQArrayError_InvalidArgs:
            case LSQArrayError_ArrayNotInit:
            case LSQArrayError_IndexOutBounds:
            {
                LSQErrorLog(&error);
                break;
            }
            default:
            {
                // Insert new node at index
                if (LSQArrayTryInsertNode(array, index, node) && index >= array->count)
                {
                    bool success = false;
                    // Update count
                    while (!success)
                    {
                        success = OSAtomicIncrement32(&array->count);
                    }
                }
                break;
            }
        }
        return error;
    }
    return LSQErrorNULL;
}

LSQError remove_node(void* self, CFIndex index)
{
    if (self != NULL)
    {
        LSQArray array = (LSQArray)self;
        LSQError error = LSQArrayRemoveErrorCode(array, index);
        switch (error.code)
        {
            case LSQArrayError_InvalidArgs:
            case LSQArrayError_ArrayNotInit:
            {
                LSQErrorLog(&error);
                break;
            }
            default:
            {
                // Try to remove node
                if (array->elements[index] != NULL && LSQArrayTryRemoveNode(array, index))
                {
                    if (array->count > 1)
                    {
                        bool success = false;
                        // Update count
                        while (!success)
                        {
                            success = OSAtomicDecrement32(&array->count);
                        }
                    }
                    else
                    {
                        array->count = 0;
                    }
                }
                break;
            }
        }
        return error;
    }
    return LSQErrorNULL;
}

LSQError remove_all(void* self)
{
    if (self != NULL)
    {
        LSQArray array = (LSQArray)self;
        LSQError error = LSQArrayRemoveAllErrorCode(array);
        switch (error.code)
        {
            case LSQArrayError_InvalidArgs:
            case LSQArrayError_ArrayNotInit:
            {
                LSQErrorLog(&error);
                break;
            }
            default:
            {
                // Iterate over all elements
                for (CFIndex i = array->count; i >= 0; --i)
                {
                    array->remove_node(array, i);
                }
                break;
            }
        }
        return error;
    }
    return LSQErrorNULL;
}

LSQNode array_get_node(void* self, CFIndex index)
{
    if (self != NULL)
    {
        LSQArray array = (LSQArray)self;
        LSQError error = LSQArrayGetErrorCode(array, index);
        switch (error.code)
        {
            case LSQArrayError_InvalidArgs:
            case LSQArrayError_IndexOutBounds:
            case LSQArrayError_ArrayNotInit:
            {
                LSQErrorLog(&error);
                break;
            }
            default:
            {
                return LSQArrayUpdateNodeIndexAndReturn(array->elements[index], index);
            }
        }
    }
    return NULL;
}

void block_enumerate(void* self, CFRange range, LSQArrayEnumerateBlock block)
{
    if (self != NULL)
    {
        LSQArray array = (LSQArray)self;
        for (CFIndex i = range.location; i < range.length; ++i)
        {
            LSQNode node = array->get_node(array, i);
            if (node != NULL)
            {
                block(node->data, i);
            }
        }
    }
}

LSQNode array_node_retain(void* self, LSQNode node)
{
    if (self != NULL && node != NULL && node->data != NULL)
    {
        LSQArray array = (LSQArray)self;
        if (array->callbacks != NULL && array->callbacks->retain_callback != NULL)
        {
            array->callbacks->retain_callback(node->data);
        }
        LSQNodeRetain(node);
    }
    return NULL;
}

void array_node_release(void* self, LSQNode node)
{
    if (self != NULL && node != NULL && node->data != NULL)
    {
        LSQArray array = (LSQArray)self;
        if (array->callbacks != NULL && array->callbacks->release_callback != NULL)
        {
            array->callbacks->release_callback(node->data);
        }
        LSQNodeRelease(node);
    }
}

#pragma mark - Public functions

LSQArray LSQArrayMake(CFIndex capacity, const struct LSQArrayCallbacks *callbacks)
{
    // Create new Array
    LSQArray array = LSQInit(LSQArray, &LSQArrayRetain_Internal, &LSQArrayRelease_Internal, &LSQArrayDealloc_Internal);
    // Set properties
    array->elements   = LSQAllocatorAllocSize(sizeof(LSQNode) * capacity);
    array->callbacks  = callbacks;
    array->capacity   = (int32_t)capacity;
    array->count      = 0;
    // Fill array with NULL
    for (CFIndex i = 0; i < capacity; ++i)
    {
        array->elements[i] = NULL;
    }
    // Set functions
    array->insert_node     = &insert_node;
    array->remove_node     = &remove_node;
    array->remove_all      = &remove_all;
    array->get_node        = &array_get_node;
    array->block_enumerate = &block_enumerate;
    array->node_retain     = &array_node_retain;
    array->node_release    = &array_node_release;
    return array;
}

LSQArray LSQArrayRetain(LSQArray self)
{
    return LSQArrayRetain_Internal(self);
}

void LSQArrayRelease(LSQArray self)
{
    LSQArrayRelease_Internal(self);
}

void LSQArrayInsertValueAtIndex(LSQArray self, CFIndex index, const void * value)
{
    LSQError error;
    if (self == NULL || value == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else if (self->insert_node != NULL && self->node_retain)
    {
        LSQNode node = LSQNodeMake(value);
        self->node_retain(self, node);
        self->insert_node(self, index, node);
    }
}

void LSQArrayRemoveValueAtIndex(LSQArray self, CFIndex index)
{
    LSQError error;
    if (self == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else if (self->remove_node != NULL)
    {
        self->remove_node(self, index);
    }
}

void LSQArrayRemoveAllValues(LSQArray self)
{
    LSQError error;
    if (self == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else if (self->remove_all != NULL)
    {
        self->remove_all(self);
    }
}

const void* LSQArrayGetValueAtIndex(LSQArray self, CFIndex index)
{
    LSQError error;
    if (self == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else if (self->get_node != NULL)
    {
        LSQNode node = self->get_node(self, index);
        return node->data;
    }
    return NULL;
}

void LSQArrayEnumerateWithBlock(LSQArray self, CFRange range, LSQArrayEnumerateBlock block)
{
    LSQError error;
    if (self == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else if (self->block_enumerate != NULL)
    {
        self->block_enumerate(self, range, block);
    }
}

CFIndex LSQArrayGetCount(LSQArray self)
{
    LSQError error;
    if (self == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else
    {
        return self->count;
    }
    return 0;
}

CFIndex LSQArrayGetCapacity(LSQArray self)
{
    LSQError error;
    if (self == NULL)
    {
        LSQErrorMake(&error, LSQArrayError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else
    {
        return self->capacity;
    }
    return 0;
}
