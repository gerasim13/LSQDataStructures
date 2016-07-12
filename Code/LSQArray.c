//
//  LSQArray.c
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 27.10.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#import "LSQArray.h"
#import "LSQCommon.h"

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

CF_INLINE OSStatus insert_at_index_check(LSQArrayRef array, CFIndex index, LSQNodeRef node)
{
#ifndef PRODUCTION
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
#endif
    return noErr;
}

CF_INLINE OSStatus remove_at_index_check(LSQArrayRef array, CFIndex index)
{
#ifndef PRODUCTION
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
#endif
    return noErr;
}

CF_INLINE OSStatus remove_all_check(LSQArrayRef array)
{
#ifndef PRODUCTION
    // Return error code for remove all function
    if (array == NULL)
    {
        return LSQArrayError_InvalidArgs;
    }
    else if (array->data.elements == NULL)
    {
        return LSQArrayError_ArrayNotInit;
    }
#endif
    return noErr;
}

CF_INLINE OSStatus get_node_at_index_check(LSQArrayRef array, CFIndex index)
{
#ifndef PRODUCTION
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
#endif
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
        success = ATOMICSWAP_PTR(array->data.elements[index], node);
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
            success = ATOMICSWAP_PTR(array->data.elements[index], next);
        }
        else
        {
            // Delete element
            success = ATOMICSWAP_PTR(array->data.elements[index], NULL);
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
        ATOMICSWAP_PTR(tmp[i], NULL);
    }
    // Swap elements with tmp and update capacity
    return ATOMICSWAP_PTR(array->data.elements, tmp) && ATOMICSWAP_INT32(array->data.capacity, capacity);
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
                ATOMICINCREMENT_INT32(self->data.count);
            }
            else
            {
                status = LSQArrayError_OutOfMemory;
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
    if (try_remove_node(self, index) && self->data.count >= 1)
    {
        // Update count
        ATOMICDECRIMENT_INT32(self->data.count);
    }
    return status;
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
    return status;
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
    if (*outNode)
    {
        LSQNodeSetIndex(*outNode, index);
    }
    else
    {
        status = -1;
    }
    return status;
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
    memset(array->data.elements, 0, capacity * sizeof(LSQNodeRef));
    return array;
}

void* LSQArrayRetain(LSQArrayRef self)
{
    ATOMICSWAP_PTR(self->base, LSQBaseRetain(self->base));
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
    ATOMICSWAP_PTR(self->base, NULL);
    // Dealloc array
    LSQAllocatorDealloc(self->data.elements);
    LSQAllocatorDealloc(self);
}

//________________________________________________________________________________________

#pragma mark - Public functions

void LSQArrayInsertValueAtIndex(LSQArrayRef self, CFIndex index, void* value)
{
    if (self->vtable->insert_node != NULL)
    {
        LSQNodeRef node = NewLSQNode(value, self->callbacks);
        self->vtable->insert_node(self, index, node);
    }
}

void LSQArrayRemoveValueAtIndex(LSQArrayRef self, CFIndex index)
{
    if (self->vtable->remove_node != NULL)
    {
        self->vtable->remove_node(self, index);
    }
}

void LSQArrayRemoveAllValues(LSQArrayRef self)
{
    if (self->vtable->remove_all != NULL)
    {
        self->vtable->remove_all(self);
    }
}

void* LSQArrayGetValueAtIndex(LSQArrayRef self, CFIndex index)
{
    if (self->vtable->get_node != NULL)
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
    if (self->vtable->block_enumerate != NULL)
    {
        self->vtable->block_enumerate(self, range, block);
    }
}

CFIndex LSQArrayGetValueIndex(LSQArrayRef self, void* value)
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
    return NAN;
}

CFIndex LSQArrayGetCount(LSQArrayRef self)
{
    return self->data.count;
}

CFIndex LSQArrayGetCapacity(LSQArrayRef self)
{
    return self->data.capacity;
}
