//
//  LSQQueueTest.m
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 24.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "LSQQueue.h"

@interface LSQQueueTest : XCTestCase
@end

@implementation LSQQueueTest

- (void)pushNumberToHead:(NSNumber*)number toQueue:(LSQQueue)queue
{
    CFIndex size = LSQQueueGetSize(queue);
    LSQQueuePushHead(queue, (__bridge const void *)(number));
    NSNumber *_num = (__bridge NSNumber*)(LSQQueueHead(queue)->data);
    XCTAssertEqual([_num integerValue], [number integerValue]);
    XCTAssertEqual(LSQQueueGetSize(queue), size + 1);
}

- (void)popNumbersFromHead:(LSQQueue)queue
{
    while (LSQQueueGetSize(queue) > 0)
    {
        LSQNode node = LSQQueuePopHead(queue);
        NSNumber  *_num = (__bridge NSNumber*)node->data;
        XCTAssertNotNil(_num);
        LSQNodeRelease(node);
        node = NULL;
    }
    XCTAssert(LSQQueueGetSize(queue) == 0);
}

- (void)pushNumberToTail:(NSNumber*)number toQueue:(LSQQueue)queue
{
    CFIndex size = LSQQueueGetSize(queue);
    LSQQueuePushTail(queue, (__bridge const void *)(number));
    NSNumber *_num = (__bridge NSNumber*)(LSQQueueTail(queue)->data);
    XCTAssertEqual([_num integerValue], [number integerValue]);
    XCTAssertEqual(LSQQueueGetSize(queue), size + 1);
}

- (void)popNumbersFromTail:(LSQQueue)queue
{
    while (LSQQueueGetSize(queue) > 0)
    {
        LSQNode node = LSQQueuePopTail(queue);
        NSNumber  *_num = (__bridge NSNumber*)node->data;
        XCTAssertNotNil(_num);
        LSQNodeRelease(node);
        node = NULL;
    }
    XCTAssert(LSQQueueGetSize(queue) == 0);
}

- (void)testLSQQueueShouldPushToHeadAndPopFromTail
{
    int random = MIN(1024, rand());
    // Create queue
    const struct LSQQueueCallbacks callbacks = { CFRetain, CFRelease };
    LSQQueue queue = LSQQueueMake(random, &callbacks);
    XCTAssert(queue != NULL);
    // Add random numbers
    for (int i = 0; i < random; ++i)
    {
        NSNumber *num = [NSNumber numberWithInt:rand()];
        [self pushNumberToHead:num toQueue:queue];
    }
    // Pop from tail
    [self popNumbersFromTail:queue];
    // Release
    LSQQueueRelease(queue);
    queue = NULL;
}

- (void)testLSQQueueShouldPushToTailAndPopFromHead
{
    int random = MIN(1024, rand());
    // Create queue
    const struct LSQQueueCallbacks callbacks = { CFRetain, CFRelease };
    LSQQueue queue = LSQQueueMake(random, &callbacks);
    XCTAssert(queue != NULL);
    // Add random numbers
    for (int i = 0; i < random; ++i)
    {
        NSNumber *num = [NSNumber numberWithInt:rand()];
        [self pushNumberToTail:num toQueue:queue];
    }
    // Pop from head
    [self popNumbersFromHead:queue];
    // Release
    LSQQueueRelease(queue);
    queue = NULL;
}

- (void)testLSQQueueShouldPushToHeadAndPopFromTailWithEuqualityChecks
{
    int random = MIN(1024, rand());
    // Create array
    NSMutableArray *array = [NSMutableArray array];
    // Add random numbers
    for (int i = 0; i < random; ++i)
    {
        NSNumber *num = [NSNumber numberWithInt:rand()];
        [array addObject:num];
    }
    // Create queue
    const struct LSQQueueCallbacks callbacks = { CFRetain, CFRelease };
    LSQQueue queue = LSQQueueMake(array.count, &callbacks);
    XCTAssert(queue != NULL);
    // Add numbers to queue
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *num = [array objectAtIndex:i];
        [self pushNumberToTail:num toQueue:queue];
    }
    // Remove numbers from queue
    for (int i = (int)array.count; i > 0; --i)
    {
        NSNumber *num = [array objectAtIndex:i - 1];
        // Pop from tail
        LSQNode node = LSQQueuePopTail(queue);
        NSNumber  *_num = (__bridge NSNumber*)node->data;
        XCTAssertEqual([num integerValue], [_num integerValue]);
        LSQNodeRelease(node);
        node = NULL;
    }
    // Check queue size
    XCTAssert(LSQQueueGetSize(queue) == 0);
    // Release
    LSQQueueRelease(queue);
    queue = NULL;
}

- (void)testLSQQueueShouldPushToHeadAndPopFromHeadWithEuqualityChecks
{
    int random = MIN(1024, rand());
    // Create array
    NSMutableArray *array = [NSMutableArray array];
    // Add random numbers
    for (int i = 0; i < random; ++i)
    {
        NSNumber *num = [NSNumber numberWithInt:rand()];
        [array addObject:num];
    }
    // Create queue
    const struct LSQQueueCallbacks callbacks = { CFRetain, CFRelease };
    LSQQueue queue = LSQQueueMake(array.count, &callbacks);
    XCTAssert(queue != NULL);
    // Add numbers to queue
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *num = [array objectAtIndex:i];
        [self pushNumberToTail:num toQueue:queue];
    }
    // Remove numbers from queue
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *num = [array objectAtIndex:i];
        // Pop from tail
        LSQNode node = LSQQueuePopHead(queue);
        NSNumber  *_num = (__bridge NSNumber*)node->data;
        XCTAssertEqual([num integerValue], [_num integerValue]);
        LSQNodeRelease(node);
        node = NULL;
    }
    // Check queue size
    XCTAssert(LSQQueueGetSize(queue) == 0);
    // Release
    LSQQueueRelease(queue);
    queue = NULL;
}

- (void)testLSQQueueShouldReturnObjectAtIndex
{
    int random = MIN(1024, rand());
    // Create array
    NSMutableArray *array = [NSMutableArray array];
    // Add random numbers
    for (int i = 0; i < random; ++i)
    {
        NSNumber *num = [NSNumber numberWithInt:rand()];
        [array addObject:num];
    }
    // Create queue
    const struct LSQQueueCallbacks callbacks = { CFRetain, CFRelease };
    LSQQueue queue = LSQQueueMake(array.count, &callbacks);
    XCTAssert(queue != NULL);
    // Add numbers to queue
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *num = [array objectAtIndex:i];
        [self pushNumberToTail:num toQueue:queue];
    }
    // Check equality of numbers
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *num = [array objectAtIndex:i];
        // Get number at index
        LSQNode node = LSQQueueGetNodeAtIndex(queue, i);
        NSNumber  *_num = (__bridge NSNumber*)node->data;
        XCTAssertEqual([num integerValue], [_num integerValue]);
    }
    // Release
    LSQQueueRelease(queue);
    queue = NULL;
}

@end
