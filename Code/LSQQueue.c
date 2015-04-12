//
//  LSQQueue.c
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 24.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#import "LSQQueue.h"
#import "LSQCommon.h"

//________________________________________________________________________________________

#pragma mark - Data Structures

// Array data
typedef struct LSQQueue
{
    LSQBaseTypeRef    base;
    LSQBaseVtableRef  callbacks;
    LSQQueueVtableRef vtable;
    LSQQueue_Data     data;
} LSQQueue;

// Error codes
enum
{
    LSQQueueError_InvalidArgs  = 888888L << 1,
    LSQQueueError_WrongCount   = 888888L << 2,
    LSQQueueError_HeadTailNull = 888888L << 3,
    LSQQueueError_QueueEmpty   = 888888L << 4
};

//________________________________________________________________________________________

#pragma mark - Hidden Functions

OSStatus push_node_check(LSQQueueRef queue, LSQNodeRef node)
{
    // Return error code for push function
    if (queue == NULL || node == NULL)
    {
        return LSQQueueError_InvalidArgs;
    }
    else if (queue->data.capacity < queue->data.count)
    {
        return LSQQueueError_WrongCount;
    }
    else if (queue->data.head == NULL && queue->data.tail == NULL)
    {
        return LSQQueueError_QueueEmpty;
    }
    else if (queue->data.head == NULL || queue->data.tail == NULL)
    {
        return LSQQueueError_HeadTailNull;
    }
    return noErr;
}

OSStatus pop_node_check(LSQQueueRef queue)
{
    // Return error code for pop function
    if (queue == NULL)
    {
        return LSQQueueError_InvalidArgs;
    }
    else if (queue->data.capacity < queue->data.count)
    {
        return LSQQueueError_WrongCount;
    }
    else if (queue->data.head == NULL && queue->data.tail == NULL)
    {
        return LSQQueueError_QueueEmpty;
    }
    else if (queue->data.head == NULL || queue->data.tail == NULL)
    {
        return LSQQueueError_HeadTailNull;
    }
    return noErr;
}

OSStatus get_node_check(LSQQueueRef queue, CFIndex index)
{
    // Return error code for get function
    if (queue == NULL || index < 0 || index > queue->data.count)
    {
        return LSQQueueError_InvalidArgs;
    }
    else if (queue->data.capacity < queue->data.count)
    {
        return LSQQueueError_WrongCount;
    }
    else if (queue->data.head == NULL && queue->data.tail == NULL)
    {
        return LSQQueueError_QueueEmpty;
    }
    else if (queue->data.head == NULL || queue->data.tail == NULL)
    {
        return LSQQueueError_HeadTailNull;
    }
    return noErr;
}

bool try_push_first_element(LSQQueueRef queue, LSQNodeRef node)
{
    return ATOMICSWAP_PTR(queue->data.head, node) && ATOMICSWAP_PTR(queue->data.tail, node);
}

bool try_push_back(LSQQueueRef queue, LSQNodeRef node)
{
    LSQNodeRef tail;
    LSQNodeRef next;
    // Try to add item to tail
    bool success = false;
    while (!success)
    {
        tail = queue->data.tail;
        next = LSQNodeGetFront(tail);
        // Was Tail pointing to the last node?
        if (next == NULL)
        {
            // Try to link node at the end of the linked list
            success = LSQNodeSetFront(tail, node);
        }
        else
        {
            // Tail was not pointing to the last node
            // Try to swing Tail to the next node
            success = ATOMICSWAP_PTR(queue->data.tail, next);
        }
    }
    // Enqueue is done. Try to swing Tail to the inserted node
    return ATOMICSWAP_PTR(queue->data.tail, node);
}

bool try_push_front(LSQQueueRef queue, LSQNodeRef node)
{
    LSQNodeRef head;
    LSQNodeRef prev;
    // Try to add item to head
    bool success = false;
    while (!success)
    {
        head = queue->data.head;
        prev = LSQNodeGetBack(head);
        // Was Head pointing to the first node?
        if (prev == NULL)
        {
            // Try to link node at the start of the linked list
            success = LSQNodeSetBack(head, node);
        }
        else
        {
            // Head was not pointing to the first node
            // Try to swing Head to the start node
            success = ATOMICSWAP_PTR(queue->data.head, prev);
        }
    }
    // Enqueue is done. Try to swing Head to the inserted node
    return ATOMICSWAP_PTR(queue->data.head, node);
}

bool try_pop_front(LSQQueueRef queue, LSQNodeRef* node)
{
    // Try to get item from head and remove it from queue
    LSQNodeRef tail;
    LSQNodeRef head;
    LSQNodeRef next;
    // Keep trying until Dequeue is done
    bool success = false;
    while (!success)
    {
        tail = queue->data.tail;
        head = queue->data.head;
        next = LSQNodeGetFront(head);
        // Are head, tail, and next consistent?
        // Is queue empty or Tail falling behind?
        if (head == tail)
        {
            if (head != NULL && next == NULL)
            {
                // Queue contains last node
                *node = head;
                // Remove head and tail node
                success = ATOMICSWAP_PTR(queue->data.tail, NULL) && ATOMICSWAP_PTR(queue->data.head, NULL);
                break;
            }
            else if (next == NULL)
            {
                // Queue is empty, couldn't dequeue
                break;
            }
            // Tail is falling behind. Try to advance it
            ATOMICSWAP_PTR(queue->data.tail, next);
        }
        else
        {
            *node = head;
            // Try to swing Head to the next node
            // Set prev node of next to NULL
            success = ATOMICSWAP_PTR(queue->data.head, next) && LSQNodeSetBack(next, NULL);
            break;
        }
    }
    return success;
}

bool try_pop_back(LSQQueueRef queue, LSQNodeRef* node)
{
    LSQNodeRef head;
    LSQNodeRef tail;
    LSQNodeRef prev;
    // Try to get item from tail and remove it from queue
    bool success = false;
    while (!success)
    {
        head = queue->data.head;
        tail = queue->data.tail;
        prev = LSQNodeGetBack(tail);
        // Are head, tail, and next consistent?
        if (tail == queue->data.tail)
        {
            // Is queue empty or Tail falling behind?
            if (tail == head)
            {
                if (head != NULL && prev == NULL)
                {
                    // Queue contains last node
                    *node = tail;
                    // Remove head and tail node
                    success = ATOMICSWAP_PTR(queue->data.head, NULL) && ATOMICSWAP_PTR(queue->data.tail, NULL);
                    break;
                }
                else if (prev == NULL)
                {
                    // Queue is empty, couldn't dequeue
                    break;
                }
                // Tail is falling behind. Try to advance it
                ATOMICSWAP_PTR(queue->data.head, prev);
            }
            else
            {
                *node = tail;
                // Try to swing Tail to the prev node
                // Set next node of prev to NULL
                success = ATOMICSWAP_PTR(queue->data.tail, prev) && LSQNodeSetFront(prev, NULL);
                break;
            }
        }
    }
    return success;
}

void increment_count(LSQQueueRef self)
{
    ATOMICINCREMENT_INT32(self->data.count);
}

void decrement_count(LSQQueueRef self)
{
    ATOMICDECRIMENT_INT32(self->data.count);
}

LSQNodeRef search_for_node_at_index(LSQQueueRef self, CFIndex index)
{
    LSQNodeRef node = NULL;
    CFIndex    indx = -1;
    // Search for node
    if (index < self->data.count / 2)
    {
        // Start from tail
        indx = self->data.count - 1;
        node = self->data.tail;
        while (indx > index)
        {
            node = LSQNodeGetBack(node);
            indx--;
        }
    }
    else
    {
        // Start from head
        indx = 0;
        node = self->data.head;
        while (indx < index)
        {
            node = LSQNodeGetFront(node);
            indx++;
        }
    }
    LSQNodeSetIndex(node, index);
    return node;
}

//________________________________________________________________________________________

#pragma mark - Private Functions

OSStatus push_back(LSQQueueRef self, LSQNodeRef node)
{
    OSStatus status = push_node_check(self, node);
    switch (status)
    {
        case LSQQueueError_QueueEmpty:
        {
            if ((node = LSQNodeRetain(node))    &&
                LSQNodeSetBackFront(node, NULL) &&
                try_push_first_element(self, node))
            {
                increment_count(self);
            }
            break;
        }
        default:
        {
            if ((node = LSQNodeRetain(node))          &&
                LSQNodeSetBackFront(node, NULL)       &&
                LSQNodeSetBack(node, self->data.tail) &&
                try_push_back(self, node))
            {
                increment_count(self);
            }
            break;
        }
    }
    return status;
}

OSStatus push_front(LSQQueueRef self, LSQNodeRef node)
{
    OSStatus status = push_node_check(self, node);
    switch (status)
    {
        case LSQQueueError_QueueEmpty:
        {
            if ((node = LSQNodeRetain(node))    &&
                LSQNodeSetFrontBack(node, NULL) &&
                try_push_first_element(self, node))
            {
                increment_count(self);
            }
            break;
        }
        default:
        {
            if ((node = LSQNodeRetain(node))           &&
                LSQNodeSetFrontBack(node, NULL)        &&
                LSQNodeSetFront(node, self->data.head) &&
                try_push_front(self, node))
            {
                increment_count(self);
            }
            break;
        }
    }
    return status;
}

OSStatus pop_tail(LSQQueueRef self, LSQNodeRef* outNode)
{
    OSStatus status;
    if ((status = pop_node_check(self) != noErr))
    {
        return status;
    }
    // Get item from tail and remove it from queue
    if (try_pop_back(self, outNode))
    {
        //LSQNodeRelease(*outNode);
        decrement_count(self);
    }
    return noErr;
}

OSStatus pop_head(LSQQueueRef self, LSQNodeRef* outNode)
{
    OSStatus status;
    if ((status = pop_node_check(self) != noErr))
    {
        return status;
    }
    // Get item from head and remove it from queue
    if (try_pop_front(self, outNode))
    {
        decrement_count(self);
    }
    return noErr;
}

OSStatus queue_get_node(LSQQueueRef self, CFIndex index, LSQNodeRef* outNode)
{
    OSStatus status;
    if ((status = get_node_check(self, index) != noErr))
    {
        return status;
    }
    // Get item at index
    if (index == 0)
    {
        *outNode = self->data.head;
        LSQNodeSetIndex(*outNode, index);
    }
    else if (index == self->data.count - 1)
    {
        *outNode = self->data.tail;
        LSQNodeSetIndex(*outNode, index);
    }
    else
    {
        *outNode = search_for_node_at_index(self, index);
    }
    return noErr;
}

//________________________________________________________________________________________

static const struct LSQQueueVtable kLSQQueueDefaultVtable = {
    &push_back,
    &push_front,
    &pop_tail,
    &pop_head,
    &queue_get_node
};

//________________________________________________________________________________________

#pragma mark - Base Functions

LSQQueueRef NewLSQQueue(CFIndex capacity, LSQBaseVtableRef vtable)
{
    // Create new queue
    LSQQueueRef queue = LSQALLOCK(LSQQueue, NULL, (void*)&LSQQueueDealloc);
    // Set properties
    queue->vtable        = &kLSQQueueDefaultVtable;
    queue->callbacks     = vtable;
    queue->data.tail     = NULL;
    queue->data.head     = NULL;
    queue->data.capacity = (int32_t)capacity;
    queue->data.count    = 0;
    return queue;
}

void* LSQQueueRetain(LSQQueueRef self)
{
    ATOMICSWAP_PTR(self->base, LSQBaseRetain(self->base));
    return self;
}

void LSQQueueRelease(LSQQueueRef self)
{
    LSQBaseRelease(self->base);
}

void LSQQueueDealloc(LSQQueueRef self)
{
    LSQNodeRef node = self->data.head;
    // Set head and tail to NULL
    if (ATOMICSWAP_PTR(self->data.tail, NULL) &&
        ATOMICSWAP_PTR(self->data.head, NULL))
    {
        // Release nodes
        while (node != NULL)
        {
            LSQNodeRef next = LSQNodeGetFront(node);
            LSQNodeRelease(node);
            node = next;
        }
    }
    ATOMICSWAP_PTR(self->base, NULL);
    LSQAllocatorDealloc(self);
}

#pragma mark - Public functions

void LSQQueuePushTail(LSQQueueRef self, void *value)
{
    // Add item to tail
    if (value && self && self->vtable->push_back != NULL)
    {
        LSQNodeRef node = NewLSQNode(value, self->callbacks);
        self->vtable->push_back(self, node);
    }
}

void LSQQueuePushHead(LSQQueueRef self, void *value)
{
    // Add item to head
    if (value && self && self->vtable->push_front != NULL)
    {
        LSQNodeRef node = NewLSQNode(value, self->callbacks);
        self->vtable->push_front(self, node);
    }
}

LSQNodeRef LSQQueuePopTail(LSQQueueRef self)
{
    // Get item from tail and remove it from queue
    if (self && self->vtable->pop_tail != NULL)
    {
        LSQNodeRef node;
        if (self->vtable->pop_tail(self, &node) == noErr)
        {
            return node;
        }
    }
    return NULL;
}

LSQNodeRef LSQQueuePopHead(LSQQueueRef self)
{
    // Get item from head and remove it from queue
    if (self && self->vtable->pop_head != NULL)
    {
        LSQNodeRef node;
        if (self->vtable->pop_head(self, &node) == noErr)
        {
            return node;
        }
    }
    return NULL;
}

LSQNodeRef LSQQueueHead(LSQQueueRef self)
{
    // Get item from head but keep it in queue
    if (self && self->data.head != NULL)
    {
        return self->data.head;
    }
    return NULL;
}

LSQNodeRef LSQQueueTail(LSQQueueRef self)
{
    // Get item from tail but keep it in queue
    if (self && self->data.tail != NULL)
    {
        return self->data.tail;
    }
    return NULL;
}

LSQNodeRef LSQQueueGetNodeAtIndex(LSQQueueRef self, CFIndex index)
{
    // Get node at index
    if (self && self->vtable->get_node != NULL)
    {
        LSQNodeRef node;
        if (self->vtable->get_node(self, index, &node) == noErr)
        {
            return node;
        }
    }
    return NULL;
}

CFIndex LSQQueueGetCount(LSQQueueRef self)
{
    if (self)
    {
        return self->data.count;
    }
    return -1;
}

CFIndex LSQQueueGetCapacity(LSQQueueRef self)
{
    if (self)
    {
        return self->data.capacity;
    }
    return -1;
}
