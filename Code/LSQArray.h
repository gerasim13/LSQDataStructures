//
//  LSQArray.h
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 27.10.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LSQDataStructures_LSQArray_h
#define LSQDataStructures_LSQArray_h

//________________________________________________________________________________________

#include <CoreFoundation/CoreFoundation.h>
#include <Block.h>
#include "LSQNode.h"

//________________________________________________________________________________________

CF_EXTERN_C_BEGIN

//________________________________________________________________________________________

typedef       struct LSQArray       *LSQArrayRef;
typedef const struct LSQArrayVtable *LSQArrayVtableRef;
typedef void (^LSQArrayBlock)(const void *data, CFIndex index);

//________________________________________________________________________________________

#pragma mark - Data Structures

// Array data
typedef struct LSQArray_Data
{
    int32_t    capacity;  // Max number of elements
    int32_t    count;     // Current number of elements
    LSQNodeRef *elements; // Array of nodes
} LSQArray_Data;

// Private functions
typedef const struct LSQArrayVtable
{
    NO_NULL(1,3) OSStatus (*insert_node)     (LSQArrayRef, CFIndex, LSQNodeRef);    // Add item at index
    NO_NULL(1)   OSStatus (*remove_node)     (LSQArrayRef, CFIndex);                // Remove item at index
    NO_NULL(1)   OSStatus (*remove_all)      (LSQArrayRef);                         // Remove all nodes
    NO_NULL(1,3) OSStatus (*get_node)        (LSQArrayRef, CFIndex, LSQNodeRef*);   // Get node at index
    NO_NULL(1,3) void     (*block_enumerate) (LSQArrayRef, CFRange, LSQArrayBlock); // Enumerate over nodes with block
} LSQArrayVtable;

//________________________________________________________________________________________

#pragma mark - Functions

CF_EXPORT              LSQArrayRef NewLSQArray               (CFIndex, LSQBaseVtableRef);           // Shorthand for creating array
CF_EXPORT NO_NULL(1,3) void        LSQArrayInsertValueAtIndex(LSQArrayRef, CFIndex, void*);         // Insert data at index
CF_EXPORT NO_NULL(1)   void        LSQArrayRemoveValueAtIndex(LSQArrayRef, CFIndex);                // Remove data
CF_EXPORT NO_NULL(1)   void        LSQArrayRemoveAllValues   (LSQArrayRef);                         // Clear array
CF_EXPORT NO_NULL(1)   void*       LSQArrayGetValueAtIndex   (LSQArrayRef, CFIndex);                // Get data from node at index
CF_EXPORT NO_NULL(1,3) void        LSQArrayEnumerate         (LSQArrayRef, CFRange, LSQArrayBlock); // Enumerate with block
CF_EXPORT NO_NULL(1,2) CFIndex     LSQArrayGetValueIndex     (LSQArrayRef, void*);                  // Get index of value
CF_EXPORT NO_NULL(1)   CFIndex     LSQArrayGetCount          (LSQArrayRef);                         // Getting count
CF_EXPORT NO_NULL(1)   CFIndex     LSQArrayGetCapacity       (LSQArrayRef);                         // Get capacity
CF_EXPORT NO_NULL(1)   void*       LSQArrayRetain            (LSQArrayRef);                         // Retain array
CF_EXPORT NO_NULL(1)   void        LSQArrayRelease           (LSQArrayRef);                         // Release array
CF_EXPORT NO_NULL(1)   void        LSQArrayDealloc           (LSQArrayRef);                         // Dealloc array

//________________________________________________________________________________________

CF_EXTERN_C_END

//________________________________________________________________________________________

#endif
