//
//  LSQQueue.h
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 24.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LSQDataStructures_LSQQueue_h
#define LSQDataStructures_LSQQueue_h

#import <CoreFoundation/CoreFoundation.h>
#import <Block.h>
#import "LSQNode.h"

//________________________________________________________________________________________

CF_EXTERN_C_BEGIN

//________________________________________________________________________________________

typedef       struct LSQQueue       *LSQQueueRef;
typedef const struct LSQQueueVtable *LSQQueueVtableRef;

//________________________________________________________________________________________

#pragma mark - Data Structures

// Array data
typedef struct LSQQueue_Data
{
    int32_t    capacity; // Max number of elements
    int32_t    count;    // Current number of elements
    LSQNodeRef head;     // Head node
    LSQNodeRef tail;     // Tail node
} LSQQueue_Data;

// Private functions
typedef const struct LSQQueueVtable
{
    OSStatus (*push_back)  (LSQQueueRef, LSQNodeRef);           // Add item to tail
    OSStatus (*push_front) (LSQQueueRef, LSQNodeRef);           // Add item to head
    OSStatus (*pop_tail)   (LSQQueueRef, LSQNodeRef*);          // Get tail and remove it from queue
    OSStatus (*pop_head)   (LSQQueueRef, LSQNodeRef*);          // Get head and remove it from queue
    OSStatus (*get_node)   (LSQQueueRef, CFIndex, LSQNodeRef*); // Get item at index
} LSQQueueVtable;

//________________________________________________________________________________________

#pragma mark - Functions

CF_EXPORT LSQQueueRef NewLSQQueue           (CFIndex, LSQBaseVtableRef); // Shorthand for creating queue
CF_EXPORT void        LSQQueuePushTail      (LSQQueueRef, void *);       // Add item to tail
CF_EXPORT void        LSQQueuePushHead      (LSQQueueRef, void *);       // Add item to head
CF_EXPORT LSQNodeRef  LSQQueuePopTail       (LSQQueueRef);               // Get item from tail and remove it from queue
CF_EXPORT LSQNodeRef  LSQQueuePopHead       (LSQQueueRef);               // Get item from head and remove it from queue
CF_EXPORT LSQNodeRef  LSQQueueHead          (LSQQueueRef);               // Get item from head but keep it in queue
CF_EXPORT LSQNodeRef  LSQQueueTail          (LSQQueueRef);               // Get item from tail but keep it in queue
CF_EXPORT LSQNodeRef  LSQQueueGetNodeAtIndex(LSQQueueRef, CFIndex);      // Get node at index
CF_EXPORT CFIndex     LSQQueueGetCount      (LSQQueueRef);               // Get queue size
CF_EXPORT void*       LSQQueueRetain        (LSQQueueRef);               // Retain queue
CF_EXPORT void        LSQQueueRelease       (LSQQueueRef);               // Release queue
CF_EXPORT void        LSQQueueDealloc       (LSQQueueRef);               // Dealloc queue

//________________________________________________________________________________________

CF_EXTERN_C_END

//________________________________________________________________________________________

#endif
