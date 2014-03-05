//
//  LSQQueue.h
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 24.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LoopsequeDJ_LSQQueue_h
#define LoopsequeDJ_LSQQueue_h

#import <CoreFoundation/CoreFoundation.h>
#import "MacTypes.h"
#import "LSQNode.h"
#import "LSQError.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#pragma mark - Data Structures
    
    LSQClass_Start(LSQQueue, LSQTypeRef);
        int32_t capacity;
        int32_t size;
        LSQNode head;
        LSQNode tail;
        // Callbacks
        const struct LSQQueueCallbacks
        {
            const void * (*retain_callback)  (const void *);
            void         (*release_callback) (const void *);
        } *callbacks;
        // Functions
        struct
        {
            LSQError (*push_back)    (void*, LSQNode);   // Add item to tail
            LSQError (*push_front)   (void*, LSQNode);   // Add item to head
            LSQNode  (*pop_tail)     (void*, LSQError*); // Get item from tail and remove it from queue
            LSQNode  (*pop_head)     (void*, LSQError*); // Get item from head and remove it from queue
            LSQNode  (*get_node)     (void*, CFIndex);   // Get item at index
            LSQNode  (*node_retain)  (void*, LSQNode);   // Retain node
            void     (*node_release) (void*, LSQNode);   // Release node
        };
    LSQClass_End(LSQQueue);

#pragma mark - Functions
    
    // Shorthand for creating queue
    LSQQueue LSQQueueMake(CFIndex capacity, const struct LSQQueueCallbacks *callbacks);
    // Add item to tail
    void LSQQueuePushTail(LSQQueue queue, const void *data);
    // Add item to head
    void LSQQueuePushHead(LSQQueue queue, const void *data);
    // Get item from tail and remove it from queue
    LSQNode LSQQueuePopTail(LSQQueue queue);
    // Get item from head and remove it from queue
    LSQNode LSQQueuePopHead(LSQQueue queue);
    // Get item from head but keep it in queue
    LSQNode LSQQueueHead(LSQQueue queue);
    // Get item from tail but keep it in queue
    LSQNode LSQQueueTail(LSQQueue queue);
    // Get node at index
    LSQNode LSQQueueGetNodeAtIndex(LSQQueue queue, CFIndex index);
    // Get queue size
    CFIndex LSQQueueGetSize(LSQQueue queue);
    // Memory management
    LSQQueue LSQQueueRetain(LSQQueue);
    void     LSQQueueRelease(LSQQueue);

#ifdef __cplusplus
}
#endif

#endif
