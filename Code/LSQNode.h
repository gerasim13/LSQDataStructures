//
//  LSQNode.h
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 10.09.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LSQDataStructures_LSQNode_h
#define LSQDataStructures_LSQNode_h

#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include "LSQBase.h"

//________________________________________________________________________________________

CF_EXTERN_C_BEGIN

//________________________________________________________________________________________

typedef         struct LSQNode       *LSQNodeRef;
CF_EXPORT const struct LSQBaseVtable kLSQNodeVtableCF;
CF_EXPORT const struct LSQBaseVtable kLSQNodeVtableCGColor;
CF_EXPORT const struct LSQBaseVtable kLSQNodeVtableCGPath;

//________________________________________________________________________________________

#pragma mark - Data Structures

// Node data
typedef struct LSQNode_Data
{
    void*      _Nonnull  content; // Data container
    LSQNodeRef _Nullable front;   // Front node
    LSQNodeRef _Nullable back;    // Back node
    CFIndex              index;   // Node index
} LSQNode_Data;

//________________________________________________________________________________________

#pragma mark - Functions

CF_EXPORT NO_NULL(1) LSQNodeRef     _Nonnull NewLSQNode         (void * _Nonnull content, LSQBaseVtableRef _Nonnull);  // Constructor
CF_EXPORT NO_NULL(1) void                    LSQNodeDealloc     (LSQNodeRef _Nonnull);                       // Dealloc node
CF_EXPORT NO_NULL(1) void*          _Nonnull LSQNodeRetain      (LSQNodeRef _Nonnull);                       // Retain node and content
CF_EXPORT NO_NULL(1) void                    LSQNodeRelease     (LSQNodeRef _Nonnull);                       // Release node and contenr
CF_EXPORT NO_NULL(1) int32_t                 LSQNodeGetRefCount (LSQNodeRef _Nonnull);                       // Get ref count
CF_EXPORT NO_NULL(1) void*          _Nonnull LSQNodeGetContent  (LSQNodeRef _Nonnull);                       // Get node content
CF_EXPORT NO_NULL(1) LSQBaseTypeRef _Nonnull LSQNodeGetBase     (LSQNodeRef _Nonnull);                       // Get base ref
CF_EXPORT NO_NULL(1) CFIndex                 LSQNodeGetIndex    (LSQNodeRef _Nonnull);                       // Get node index
CF_EXPORT NO_NULL(1) void                    LSQNodeSetIndex    (LSQNodeRef _Nonnull, CFIndex);              // Set node index
CF_EXPORT NO_NULL(1) LSQNodeRef     _Nonnull LSQNodeGetFront    (LSQNodeRef _Nonnull);                       // Get front node
CF_EXPORT NO_NULL(1) bool                    LSQNodeSetFront    (LSQNodeRef _Nonnull, LSQNodeRef _Nullable); // Set front node
CF_EXPORT NO_NULL(1) LSQNodeRef     _Nonnull LSQNodeGetBack     (LSQNodeRef _Nonnull);                       // Get back index
CF_EXPORT NO_NULL(1) bool                    LSQNodeSetBack     (LSQNodeRef _Nonnull, LSQNodeRef _Nullable); // Set back node
CF_EXPORT NO_NULL(1) bool                    LSQNodeSetFrontBack(LSQNodeRef _Nonnull, LSQNodeRef _Nullable); // Set front and back node
CF_EXPORT NO_NULL(1) bool                    LSQNodeSetBackFront(LSQNodeRef _Nonnull, LSQNodeRef _Nullable); // Set back and front node

//________________________________________________________________________________________

#pragma mark - CoreFoundation Callbacks

const void * _Nonnull LSQNodeRetainCallBack (CFAllocatorRef _Nonnull allocator, const void * _Nonnull value);
void                  LSQNodeReleaseCallBack(CFAllocatorRef _Nonnull allocator, const void * _Nonnull value);

//________________________________________________________________________________________

CF_EXTERN_C_END

//________________________________________________________________________________________

#endif
