//
//  LSQQueueTest.m
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 24.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "LSQQueue.h"

@interface LSQQueueTest : XCTestCase
@end

@implementation LSQQueueTest

- (void)pushNumberToHead:(NSNumber*)number toQueue:(LSQQueueRef)queue
{
    CFIndex size = LSQQueueGetCount(queue);
    LSQQueuePushHead(queue, (__bridge void *)(number));
    NSNumber *_num = (__bridge NSNumber*)(LSQNodeGetContent(LSQQueueHead(queue)));
    XCTAssertEqual([_num integerValue], [number integerValue]);
    XCTAssertEqual(LSQQueueGetCount(queue), size + 1);
}

- (void)popNumbersFromHead:(LSQQueueRef)queue
{
    while (LSQQueueGetCount(queue) > 0)
    {
        LSQNodeRef node = LSQQueuePopHead(queue);
        NSNumber  *_num = (__bridge NSNumber*)LSQNodeGetContent(node);
        XCTAssertNotNil(_num);
        LSQNodeRelease(node);
        node = NULL;
    }
    XCTAssert(LSQQueueGetCount(queue) == 0);
}

- (void)pushNumberToTail:(NSNumber*)number toQueue:(LSQQueueRef)queue
{
    CFIndex size = LSQQueueGetCount(queue);
    LSQQueuePushTail(queue, (__bridge void *)(number));
    NSNumber *_num = (__bridge NSNumber*)(LSQNodeGetContent(LSQQueueTail(queue)));
    XCTAssertEqual([_num integerValue], [number integerValue]);
    XCTAssertEqual(LSQQueueGetCount(queue), size + 1);
}

- (void)popNumbersFromTail:(LSQQueueRef)queue
{
    while (LSQQueueGetCount(queue) > 0)
    {
        LSQNodeRef node = LSQQueuePopTail(queue);
        NSNumber  *_num = (__bridge NSNumber*)LSQNodeGetContent(node);
        XCTAssertNotNil(_num);
        LSQNodeRelease(node);
        node = NULL;
    }
    XCTAssert(LSQQueueGetCount(queue) == 0);
}

- (void)testLSQQueueShouldPushToHeadAndPopFromTail
{
    int random = MIN(1024, rand());
    // Create queue
    LSQQueueRef queue = NewLSQQueue(random, &kLSQNodeVtableCF);
    XCTAssert(queue != NULL);
    // Add random numbers
    for (int i = 0; i < random; ++i)
    {
        NSNumber *num = @(rand());
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
    LSQQueueRef queue = NewLSQQueue(random, &kLSQNodeVtableCF);
    XCTAssert(queue != NULL);
    // Add random numbers
    for (int i = 0; i < random; ++i)
    {
        NSNumber *num = @(rand());
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
        NSNumber *num = @(rand());
        [array addObject:num];
    }
    // Create queue
    LSQQueueRef queue = NewLSQQueue(array.count, &kLSQNodeVtableCF);
    XCTAssert(queue != NULL);
    // Add numbers to queue
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *num = array[i];
        [self pushNumberToTail:num toQueue:queue];
    }
    // Remove numbers from queue
    for (int i = (int)array.count; i > 0; --i)
    {
        NSNumber *num = array[i - 1];
        // Pop from tail
        LSQNodeRef node = LSQQueuePopTail(queue);
        NSNumber  *_num = (__bridge NSNumber*)LSQNodeGetContent(node);
        XCTAssertEqual([num integerValue], [_num integerValue]);
        LSQNodeRelease(node);
        node = NULL;
    }
    // Check queue size
    XCTAssert(LSQQueueGetCount(queue) == 0);
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
        NSNumber *num = @(rand());
        [array addObject:num];
    }
    // Create queue
    LSQQueueRef queue = NewLSQQueue(array.count, &kLSQNodeVtableCF);
    XCTAssert(queue != NULL);
    // Add numbers to queue
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *num = array[i];
        [self pushNumberToTail:num toQueue:queue];
    }
    // Remove numbers from queue
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *num = array[i];
        // Pop from tail
        LSQNodeRef node = LSQQueuePopHead(queue);
        NSNumber  *_num = (__bridge NSNumber*)LSQNodeGetContent(node);
        XCTAssertEqual([num integerValue], [_num integerValue]);
        LSQNodeRelease(node);
        node = NULL;
    }
    // Check queue size
    XCTAssert(LSQQueueGetCount(queue) == 0);
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
        NSNumber *num = @(rand());
        [array addObject:num];
    }
    // Create queue
    LSQQueueRef queue = NewLSQQueue(array.count, &kLSQNodeVtableCF);
    XCTAssert(queue != NULL);
    // Add numbers to queue
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *num = array[i];
        [self pushNumberToTail:num toQueue:queue];
    }
    // Check equality of numbers
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *num = array[i];
        // Get number at index
        LSQNodeRef node = LSQQueueGetNodeAtIndex(queue, i);
        NSNumber  *_num = (__bridge NSNumber*)LSQNodeGetContent(node);
        XCTAssertEqual([num integerValue], [_num integerValue]);
    }
    // Release
    LSQQueueRelease(queue);
    queue = NULL;
}

@end
