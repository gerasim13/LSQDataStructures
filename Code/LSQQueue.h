//
//  LSQQueue.h
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 24.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LSQDataStructures_LSQQueue_h
#define LSQDataStructures_LSQQueue_h

//________________________________________________________________________________________

#include <CoreFoundation/CoreFoundation.h>
#include <stdatomic.h>
#include <Block.h>
#include "LSQNode.h"

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
    LSQNodeRef _Atomic  head;     // Head node
    LSQNodeRef _Atomic  tail;     // Tail node
    atomic_int_fast32_t capacity; // Max number of elements
    atomic_int_fast32_t count;    // Current number of elements
} LSQQueue_Data;

// Private functions
typedef const struct LSQQueueVtable
{
    NO_NULL(1)   OSStatus (*push_back)  (LSQQueueRef, LSQNodeRef);           // Add item to tail
    NO_NULL(1)   OSStatus (*push_front) (LSQQueueRef, LSQNodeRef);           // Add item to head
    NO_NULL(1,2) OSStatus (*pop_tail)   (LSQQueueRef, LSQNodeRef*);          // Get tail and remove it from queue
    NO_NULL(1,2) OSStatus (*pop_head)   (LSQQueueRef, LSQNodeRef*);          // Get head and remove it from queue
    NO_NULL(1,3) OSStatus (*get_node)   (LSQQueueRef, CFIndex, LSQNodeRef*); // Get item at index
} LSQQueueVtable;

//________________________________________________________________________________________

#pragma mark - Functions

CF_EXPORT              LSQQueueRef NewLSQQueue           (CFIndex, LSQBaseVtableRef); // Shorthand for creating queue
CF_EXPORT NO_NULL(1,2) void        LSQQueuePushTail      (LSQQueueRef, void *);       // Add item to tail
CF_EXPORT NO_NULL(1,2) void        LSQQueuePushHead      (LSQQueueRef, void *);       // Add item to head
CF_EXPORT NO_NULL(1)   LSQNodeRef  LSQQueuePopTail       (LSQQueueRef);               // Get item from tail and remove it from queue
CF_EXPORT NO_NULL(1)   LSQNodeRef  LSQQueuePopHead       (LSQQueueRef);               // Get item from head and remove it from queue
CF_EXPORT NO_NULL(1)   LSQNodeRef  LSQQueueHead          (LSQQueueRef);               // Get item from head but keep it in queue
CF_EXPORT NO_NULL(1)   LSQNodeRef  LSQQueueTail          (LSQQueueRef);               // Get item from tail but keep it in queue
CF_EXPORT NO_NULL(1)   LSQNodeRef  LSQQueueGetNodeAtIndex(LSQQueueRef, CFIndex);      // Get node at index
CF_EXPORT NO_NULL(1)   CFIndex     LSQQueueGetCount      (LSQQueueRef);               // Get queue size
CF_EXPORT NO_NULL(1)   CFIndex     LSQQueueGetCapacity   (LSQQueueRef);               // Get queue capacity
CF_EXPORT NO_NULL(1)   void*       LSQQueueRetain        (LSQQueueRef);               // Retain queue
CF_EXPORT NO_NULL(1)   void        LSQQueueRelease       (LSQQueueRef);               // Release queue
CF_EXPORT NO_NULL(1)   void        LSQQueueDealloc       (LSQQueueRef);               // Dealloc queue

//________________________________________________________________________________________

CF_EXTERN_C_END

//________________________________________________________________________________________

#endif
