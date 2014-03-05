//
//  LSQArray.h
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 27.10.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LoopsequeDJ_LSQArray_h
#define LoopsequeDJ_LSQArray_h

#import <CoreFoundation/CoreFoundation.h>
#import <Block.h>
#import "MacTypes.h"
#import "LSQNode.h"
#import "LSQError.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#pragma mark - Data Structures
    
    // Iteration block diffinition
    typedef void (^LSQArrayEnumerateBlock)(const void * data, CFIndex index);
    
    // Array structure
    LSQClass_Start(LSQArray, LSQTypeRef);
        int32_t   capacity;
        int32_t   count;
        LSQNode * elements;
        // Callbacks
        const struct LSQArrayCallbacks
        {
            const void * (*retain_callback)  (const void *);
            void         (*release_callback) (const void *);
        } *callbacks;
        // Functions
        struct
        {
            LSQError (*insert_node)     (void*, CFIndex, LSQNode); // Add item at index
            LSQError (*remove_node)     (void*, CFIndex);          // Remove item at index
            LSQError (*remove_all)      (void*);                   // Remove all nodes
            LSQNode  (*get_node)        (void*, CFIndex);          // Get node at index
            LSQNode  (*node_retain)     (void*, LSQNode);          // Retain node
            void     (*node_release)    (void*, LSQNode);          // Release node
            void     (*block_enumerate) (void*, CFRange, LSQArrayEnumerateBlock); // Enumerate over nodes with block
        };
    LSQClass_End(LSQArray);
    
    // Default callback (Internally uses CFRetain and CFRelease)
    extern const struct LSQArrayCallbacks kLSQArrayCFCallbacks;
    
#pragma mark - Functions
    
    // Shorthand for creating array
    LSQArray LSQArrayMake(CFIndex, const struct LSQArrayCallbacks *);
    // Values handling
    void        LSQArrayInsertValueAtIndex(LSQArray, CFIndex, const void *);
    void        LSQArrayRemoveValueAtIndex(LSQArray, CFIndex);
    void        LSQArrayRemoveAllValues   (LSQArray);
    const void *LSQArrayGetValueAtIndex   (LSQArray, CFIndex);
    // Enumerate with block
    void LSQArrayEnumerateWithBlock(LSQArray, CFRange, LSQArrayEnumerateBlock);
    // Getting count
    CFIndex LSQArrayGetCount(LSQArray);
    CFIndex LSQArrayGetCapacity(LSQArray);
    // Memory management
    LSQArray LSQArrayRetain(LSQArray);
    void     LSQArrayRelease(LSQArray);
    
#ifdef __cplusplus
}
#endif

#endif
