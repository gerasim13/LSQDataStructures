//
//  LSQNode.h
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 10.09.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LSQDataStructures_LSQNode_h
#define LSQDataStructures_LSQNode_h

#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import "LSQBase.h"

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
    void*       content; // Data container
    LSQNodeRef  front;   // Front node
    LSQNodeRef  back;    // Back node
    CFIndex     index;   // Node index
} LSQNode_Data;

//________________________________________________________________________________________

#pragma mark - Functions

CF_EXPORT NO_NULL(1) LSQNodeRef     NewLSQNode         (void *content, LSQBaseVtableRef); // Constructor
CF_EXPORT NO_NULL(1) void           LSQNodeDealloc     (LSQNodeRef);                      // Dealloc node
CF_EXPORT NO_NULL(1) void*          LSQNodeRetain      (LSQNodeRef);                      // Retain node and content
CF_EXPORT NO_NULL(1) void           LSQNodeRelease     (LSQNodeRef);                      // Release node and contenr
CF_EXPORT NO_NULL(1) int32_t        LSQNodeGetRefCount (LSQNodeRef);                      // Get ref count
CF_EXPORT NO_NULL(1) void*          LSQNodeGetContent  (LSQNodeRef);                      // Get node content
CF_EXPORT NO_NULL(1) LSQBaseTypeRef LSQNodeGetBase     (LSQNodeRef);                      // Get base ref
CF_EXPORT NO_NULL(1) CFIndex        LSQNodeGetIndex    (LSQNodeRef);                      // Get node index
CF_EXPORT NO_NULL(1) void           LSQNodeSetIndex    (LSQNodeRef, CFIndex);             // Set node index
CF_EXPORT NO_NULL(1) LSQNodeRef     LSQNodeGetFront    (LSQNodeRef);                      // Get front node
CF_EXPORT NO_NULL(1) bool           LSQNodeSetFront    (LSQNodeRef, LSQNodeRef);          // Set front node
CF_EXPORT NO_NULL(1) LSQNodeRef     LSQNodeGetBack     (LSQNodeRef);                      // Get back index
CF_EXPORT NO_NULL(1) bool           LSQNodeSetBack     (LSQNodeRef, LSQNodeRef);          // Set back node
CF_EXPORT NO_NULL(1) bool           LSQNodeSetFrontBack(LSQNodeRef, LSQNodeRef);          // Set front and back node
CF_EXPORT NO_NULL(1) bool           LSQNodeSetBackFront(LSQNodeRef, LSQNodeRef);          // Set back and front node

//________________________________________________________________________________________

CF_EXTERN_C_END

//________________________________________________________________________________________

#endif
