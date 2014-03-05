//
//  LSQQueue.c
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 24.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#include "LSQQueue.h"
#include <stdio.h>
#include <libkern/OSAtomic.h>

#pragma mark - Base Functions

void* LSQQueueRetain_Internal(void* self)
{
    LSQQueue _self = (LSQQueue)self;
    return LSQSuperRetain(_self);
}

void LSQQueueRelease_Internal(void* self)
{
    LSQQueue _self = (LSQQueue)self;
    LSQSuperRelease(_self);
}

void LSQQueueDealloc_Internal(void* self)
{
    LSQQueue _self = (LSQQueue)self;
    // Release nodes
    while (_self->head != NULL)
    {
        LSQNode node = _self->pop_head(_self, NULL);
        _self->node_release(_self, node);
    }
    // Dealloc super
    LSQSuperDealloc(_self);
}

#pragma mark - Hidden Functions

// Return error code for push function
LSQError LSQQueuePushErrorCode(LSQQueue queue, LSQNode node)
{
    LSQError error;
    if (queue == NULL || node == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_InvalidArgs);
    }
    else if (queue->head == NULL && queue->tail == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_QueueEmpty);
    }
    else if (queue->head == NULL || queue->tail == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_HeadTailNull);
    }
    else
    {
        LSQErrorMake(&error, LSQQueueError_NoError);
    }
    return error;
}

// Return error code for pop function
LSQError LSQQueuePopErrorCode(LSQQueue queue)
{
    LSQError error;
    if (queue == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_InvalidArgs);
    }
    else if (queue->head == NULL && queue->tail == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_QueueEmpty);
    }
    else if (queue->head == NULL || queue->tail == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_HeadTailNull);
    }
    else
    {
        LSQErrorMake(&error, LSQQueueError_NoError);
    }
    return error;
}

// Return error code for get function
LSQError LSQQueueGetErrorCode(LSQQueue queue, CFIndex index)
{
    LSQError error;
    if (queue == NULL || index < 0 || index > queue->size)
    {
        LSQErrorMake(&error, LSQQueueError_InvalidArgs);
    }
    else if (queue->head == NULL && queue->tail == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_QueueEmpty);
    }
    else if (queue->head == NULL || queue->tail == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_HeadTailNull);
    }
    else
    {
        LSQErrorMake(&error, LSQQueueError_NoError);
    }
    return error;
}

// Try to add first item
bool LSQQUeueTryPushFirst(LSQQueue queue, LSQNode node)
{
    LSQNode head;
    LSQNode tail;
    // Keep trying until Enqueue is done
    while (true)
    {
        head = queue->head;
        tail = queue->tail;
        if (head == queue->head) { OSAtomicCompareAndSwapPtr(head, node, (void* volatile*)&queue->head); }
        if (tail == queue->tail) { OSAtomicCompareAndSwapPtr(tail, node, (void* volatile*)&queue->tail); }
        if (queue->head == node && queue->tail == node)
        {
            break;
        }
    }
    return true;
}

// Try to add item to tail
bool LSQQueueTryPushBack(LSQQueue queue, LSQNode node)
{
    LSQNode tail;
    LSQNode next;
    // Keep trying until Enqueue is done
    while (true)
    {
        tail = queue->tail;
        next = tail->front;
        if (tail == queue->tail)
        {
            // Was Tail pointing to the last node?
            if (next == NULL)
            {
                // Try to link node at the end of the linked list
                if (OSAtomicCompareAndSwapPtr(next, node, (void* volatile*)&tail->front))
                {
                    // Enqueue is done. Exit loop
                    break;
                }
            }
            else
            {
                // Tail was not pointing to the last node
                // Try to swing Tail to the next node
                OSAtomicCompareAndSwapPtr(tail, next, (void* volatile*)&queue->tail);
            }
        }
    }
    // Enqueue is done. Try to swing Tail to the inserted node
    return OSAtomicCompareAndSwapPtr(tail, node, (void* volatile*)&queue->tail);
}

// Try to add item to head
bool LSQQueueTryPushFront(LSQQueue queue, LSQNode node)
{
    LSQNode head;
    LSQNode prev;
    // Keep trying until Enqueue is done
    while (true)
    {
        head = queue->head;
        prev = head->back;
        if (head == queue->head)
        {
            // Was Head pointing to the first node?
            if (prev == NULL)
            {
                // Try to link node at the start of the linked list
                if (OSAtomicCompareAndSwapPtr(prev, node, (void* volatile*)&head->back))
                {
                    // Enqueue is done. Exit loop
                    break;
                }
            }
            else
            {
                // Head was not pointing to the first node
                // Try to swing Head to the start node
                OSAtomicCompareAndSwapPtr(head, prev, (void* volatile*)&queue->head);
            }
        }
    }
    // Enqueue is done. Try to swing Head to the inserted node
    return OSAtomicCompareAndSwapPtr(head, node, (void* volatile*)&queue->head);
}

// Try to get item from head and remove it from queue
bool LSQQueueTryPopFront(LSQQueue queue, LSQNode* node)
{
    LSQNode tail;
    LSQNode head;
    LSQNode next;
    // Keep trying until Dequeue is done
    while (true)
    {
        tail = queue->tail;
        head = queue->head;
        next = head->front;
        // Are head, tail, and next consistent?
        if (head == queue->head)
        {
            // Is queue empty or Tail falling behind?
            if (head == tail)
            {
                if (head != NULL && next == NULL)
                {
                    // Queue contains last node
                    *node = head;
                    // Remove head and tail node
                    OSAtomicCompareAndSwapPtr(tail, NULL, (void* volatile*)&queue->tail);
                    OSAtomicCompareAndSwapPtr(head, NULL, (void* volatile*)&queue->head);
                    // Dequeue is done. Exit loop
                    break;
                }
                else if (next == NULL)
                {
                    // Queue is empty, couldn't dequeue
                    return false;
                }
                // Tail is falling behind. Try to advance it
                OSAtomicCompareAndSwapPtr(tail, next, (void* volatile*)&queue->tail);
            }
            else
            {
                *node = head;
                // Try to swing Head to the next node
                if (OSAtomicCompareAndSwapPtr(head, next, (void* volatile*)&queue->head))
                {
                    // Set prev node of next to NULL
                    OSAtomicCompareAndSwapPtr(next->back, NULL, (void* volatile*)&next->back);
                    // Dequeue is done. Exit loop
                    break;
                }
            }
        }
    }
    return true;
}

// Try to get item from tail and remove it from queue
bool LSQQueueTryPopBack(LSQQueue queue, LSQNode* node)
{
    LSQNode head;
    LSQNode tail;
    LSQNode prev;
    // Keep trying until Dequeue is done
    while (true)
    {
        head = queue->head;
        tail = queue->tail;
        prev = tail->back;
        // Are head, tail, and next consistent?
        if (tail == queue->tail)
        {
            // Is queue empty or Tail falling behind?
            if (tail == head)
            {
                if (head != NULL && prev == NULL)
                {
                    // Queue contains last node
                    *node = tail;
                    // Remove head and tail node
                    OSAtomicCompareAndSwapPtr(head, NULL, (void* volatile*)&queue->head);
                    OSAtomicCompareAndSwapPtr(tail, NULL, (void* volatile*)&queue->tail);
                    // Dequeue is done. Exit loop
                    break;
                }
                else if (prev == NULL)
                {
                    // Queue is empty, couldn't dequeue
                    return false;
                }
                // Tail is falling behind. Try to advance it
                OSAtomicCompareAndSwapPtr(head, prev, (void* volatile*)&queue->head);
            }
            else
            {
                *node = tail;
                // Try to swing Tail to the prev node
                if (OSAtomicCompareAndSwapPtr(tail, prev, (void* volatile*)&queue->tail))
                {
                    // Set next node of prev to NULL
                    OSAtomicCompareAndSwapPtr(prev->front, NULL, (void* volatile*)&prev->front);
                    // Dequeue is done. Exit loop
                    break;
                }
            }
        }
    }
    return true;
}

LSQNode LSQQueueUpdateNodeIndexAndReturn(LSQNode node, CFIndex index)
{
    if (node != NULL && OSAtomicCompareAndSwapLong(node->index, index, &node->index))
    {
        return node;
    }
    // Return node anyway
    return node;
}

#pragma mark - Private Functions

// Add item to tail
LSQError push_back(void* self, LSQNode node)
{
    if (self != NULL)
    {
        LSQQueue queue = (LSQQueue)self;
        LSQError error = LSQQueuePushErrorCode(queue, node);
        switch (error.code)
        {
            case LSQQueueError_InvalidArgs:
            case LSQQueueError_HeadTailNull:
            {
                LSQErrorLog(&error);
                break;
            }
            case LSQQueueError_QueueEmpty:
            {
                if (queue->capacity > queue->size)
                {
                    node->back = node->front = NULL;
                    if (LSQQUeueTryPushFirst(queue, node))
                    {
                        bool success = false;
                        // Update size
                        while (!success)
                        {
                            success = OSAtomicIncrement32(&queue->size);
                        }
                    }
                }
                break;
            }
            default:
            {
                if (queue->capacity > queue->size)
                {
                    node->back = node->front = NULL;
                    node->back = queue->tail;
                    if (LSQQueueTryPushBack(queue, node))
                    {
                        bool success = false;
                        // Update size
                        while (!success)
                        {
                            success = OSAtomicIncrement32(&queue->size);
                        }
                    }
                }
                break;
            }
        }
        return error;
    }
    return LSQErrorNULL;
}

// Add item to head
LSQError push_front(void* self, LSQNode node)
{
    if (self != NULL)
    {
        LSQQueue queue = (LSQQueue)self;
        LSQError error = LSQQueuePushErrorCode(queue, node);
        switch (error.code)
        {
            case LSQQueueError_InvalidArgs:
            case LSQQueueError_HeadTailNull:
            {
                LSQErrorLog(&error);
                break;
            }
            case LSQQueueError_QueueEmpty:
            {
                if (queue->capacity > queue->size)
                {
                    node->back = node->front = NULL;
                    if (LSQQUeueTryPushFirst(queue, node))
                    {
                        bool success = false;
                        // Update size
                        while (!success)
                        {
                            success = OSAtomicIncrement32(&queue->size);
                        }
                    }
                }
                break;
            }
            default:
            {
                if (queue->capacity > queue->size)
                {
                    node->front = node->back = NULL;
                    node->front = queue->head;
                    if (LSQQueueTryPushFront(queue, node))
                    {
                        bool success = false;
                        // Update size
                        while (!success)
                        {
                            success = OSAtomicIncrement32(&queue->size);
                        }
                    }
                }
                break;
            }
        }
        return error;
    }
    return LSQErrorNULL;
}

// Get item from tail and remove it from queue
LSQNode pop_tail(void* self, LSQError* error)
{
    if (self != NULL)
    {
        LSQQueue queue = (LSQQueue)self;
        LSQError _error = LSQQueuePopErrorCode(queue);
        if (error != NULL) { *error = _error; }
        
        switch (_error.code)
        {
            case LSQQueueError_InvalidArgs:
            case LSQQueueError_HeadTailNull:
            case LSQQueueError_QueueEmpty:
            {
                LSQErrorLog(&_error);
                break;
            }
            default:
            {
                LSQNode node;
                if (LSQQueueTryPopBack(queue, &node))
                {
                    bool success = false;
                    // Update size
                    while (!success)
                    {
                        success = OSAtomicDecrement32(&queue->size) || queue->size == 0;
                    }
                    return node;
                }
                break;
            }
        }
    }
    return NULL;
}

// Get item from head and remove it from queue
LSQNode pop_head(void* self, LSQError* error)
{
    if (self != NULL)
    {
        LSQQueue queue  = (LSQQueue)self;
        LSQError _error = LSQQueuePopErrorCode(queue);
        if (error != NULL) { *error = _error; }
        
        switch (_error.code)
        {
            case LSQQueueError_InvalidArgs:
            case LSQQueueError_HeadTailNull:
            case LSQQueueError_QueueEmpty:
            {
                LSQErrorLog(&_error);
                break;
            }
            default:
            {
                LSQNode node;
                if (LSQQueueTryPopFront(queue, &node))
                {
                    bool success = false;
                    // Update size
                    while (!success)
                    {
                        success = OSAtomicDecrement32(&queue->size) || queue->size == 0;
                    }
                    return node;
                }
                break;
            }
        }
    }
    return NULL;
}

// Get item at index
LSQNode queue_get_node(void* self, CFIndex index)
{
    if (self != NULL)
    {
        LSQQueue queue = (LSQQueue)self;
        LSQError error = LSQQueueGetErrorCode(queue, index);
        switch (error.code)
        {
            case LSQQueueError_InvalidArgs:
            case LSQQueueError_HeadTailNull:
            case LSQQueueError_QueueEmpty:
            {
                LSQErrorLog(&error);
                break;
            }
            default:
            {
                if (index == 0)
                {
                    return LSQQueueUpdateNodeIndexAndReturn(queue->head, index);
                }
                else if (index == (queue->size - 1))
                {
                    return LSQQueueUpdateNodeIndexAndReturn(queue->tail, index);
                }
                else
                {
                    CFIndex indx = -1;
                    LSQNode node = NULL;
                    // Search for node
                    if (index < queue->size % 2)
                    {
                        // Start from tail
                        indx = queue->size;
                        node = queue->tail;
                        while (indx > index)
                        {
                            node = node->back;
                            indx--;
                        }
                    }
                    else
                    {
                        // Start from head
                        indx = 0;
                        node = queue->head;
                        while (indx < index)
                        {
                            node = node->front;
                            indx++;
                        }
                    }
                    return LSQQueueUpdateNodeIndexAndReturn(node, index);
                }
                break;
            }
        }
    }
    return NULL;
}

LSQNode queue_node_retain(void* self, LSQNode node)
{
    if (self != NULL && node != NULL)
    {
        LSQQueue queue = (LSQQueue)self;
        if (queue->callbacks != NULL && queue->callbacks->retain_callback != NULL)
        {
            queue->callbacks->retain_callback(node->data);
        }
        LSQNodeRetain(node);
    }
    return NULL;
}

void queue_node_release(void* self, LSQNode node)
{
    if (self != NULL && node != NULL)
    {
        LSQQueue queue = (LSQQueue)self;
        if (queue->callbacks != NULL && queue->callbacks->release_callback != NULL)
        {
            queue->callbacks->release_callback(node->data);
        }
        LSQNodeRelease(node);
    }
}

#pragma mark - Public functions

LSQQueue LSQQueueMake(CFIndex capacity, const struct LSQQueueCallbacks *callbacks)
{
    // Create new queue
    LSQQueue queue = LSQInit(LSQQueue, &LSQQueueRetain_Internal, &LSQQueueRelease_Internal, &LSQQueueDealloc_Internal);
    // Set properties
    queue->head       = NULL;
    queue->tail       = NULL;
    queue->callbacks  = callbacks;
    queue->capacity   = (int32_t)capacity;
    queue->size       = NAN;
    // Set functions
    queue->push_back    = &push_back;
    queue->push_front   = &push_front;
    queue->pop_tail     = &pop_tail;
    queue->pop_head     = &pop_head;
    queue->get_node     = &queue_get_node;
    queue->node_retain  = &queue_node_retain;
    queue->node_release = &queue_node_release;
    return queue;
}

LSQQueue LSQQueueRetain(LSQQueue self)
{
    return LSQQueueRetain_Internal(self);
}

void LSQQueueRelease(LSQQueue self)
{
    LSQQueueRelease_Internal(self);
}

// Add item to tail
void LSQQueuePushTail(LSQQueue queue, const void *data)
{
    LSQError error;
    if (queue == NULL || data == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else if (queue->push_back != NULL && queue->node_retain != NULL)
    {
        LSQNode node = LSQNodeMake(data);
        queue->node_retain(queue, node);
        queue->push_back(queue, node);
    }
}

// Add item to head
void LSQQueuePushHead(LSQQueue queue, const void *data)
{
    LSQError error;
    if (queue == NULL || data == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else if (queue->push_front != NULL && queue->node_retain != NULL)
    {
        LSQNode node = LSQNodeMake(data);
        queue->node_retain(queue, node);
        queue->push_front(queue, node);
    }
}

// Get item from tail and remove it from queue
LSQNode LSQQueuePopTail(LSQQueue queue)
{
    LSQError error;
    if (queue == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else
    {
        LSQNode node = queue->pop_tail(queue, &error);
        if (node != NULL && node->data != NULL)
        {
            return node;
        }
    }
    return NULL;
}

// Get item from head and remove it from queue
LSQNode LSQQueuePopHead(LSQQueue queue)
{
    LSQError error;
    if (queue == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else
    {
        LSQNode node = queue->pop_head(queue, &error);
        if (node != NULL && node->data != NULL)
        {
            return node;
        }
    }
    return NULL;
}

// Get item from head but keep it in queue
LSQNode LSQQueueHead(LSQQueue queue)
{
    LSQError error;
    if (queue == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else
    {
        LSQNode node = queue->head;
        if (node != NULL && node->data != NULL)
        {
            return node;
        }
    }
    return NULL;
}

// Get item from tail but keep it in queue
LSQNode LSQQueueTail(LSQQueue queue)
{
    LSQError error;
    if (queue == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else
    {
        LSQNode node = queue->tail;
        if (node != NULL && node->data != NULL)
        {
            return node;
        }
    }
    return NULL;
}

// Get node at index
LSQNode LSQQueueGetNodeAtIndex(LSQQueue queue, CFIndex index)
{
    LSQError error;
    if (queue == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else if (queue->get_node != NULL)
    {
        LSQNode node = queue->get_node(queue, index);
        if (node != NULL && node->data != NULL)
        {
            // Return node
            return node;
        }
    }
    return NULL;
}

// Get queue size
CFIndex LSQQueueGetSize(LSQQueue queue)
{
    LSQError error;
    if (queue == NULL)
    {
        LSQErrorMake(&error, LSQQueueError_InvalidArgs);
        LSQErrorLog(&error);
    }
    else
    {
        return queue->size;
    }
    return -1;
}
