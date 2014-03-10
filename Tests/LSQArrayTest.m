//
//  LSQArrayTest.m
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 29.10.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#import <XCTest/XCTest.h>
#import <CoreGraphics/CoreGraphics.h>
#import <UIKit/UIKit.h>
#import "LSQArray.h"

@interface LSQArrayTest : XCTestCase
@end

@implementation LSQArrayTest

#pragma mark - Test Cases

- (UIColor*)randomColor
{
    CGFloat red =  (CGFloat)arc4random() / (CGFloat)RAND_MAX;
    CGFloat blue = (CGFloat)arc4random() / (CGFloat)RAND_MAX;
    CGFloat green = (CGFloat)arc4random() / (CGFloat)RAND_MAX;
    return [UIColor colorWithRed:red green:green blue:blue alpha:1.0];
}

- (void)addNumberOfNodes:(NSInteger)num toArray:(LSQArrayRef)array
{
    CFIndex c = LSQArrayGetCount(array);
    CFIndex i = c > 0 ? c : 0;
    for (; i < c + num; ++i)
    {
        CGColorRef color = [[self randomColor] CGColor];
        LSQArrayInsertValueAtIndex(array, i, color);
    }
}

- (void)removeNumberOfNodes:(NSInteger)num fromArray:(LSQArrayRef)array
{
    CFIndex c = LSQArrayGetCount(array);
    CFIndex i = c > 0 ? c - 1 : 0;
    for (; i >= c - num && i >= 0; --i)
    {
        LSQArrayRemoveValueAtIndex(array, i);
    }
}

- (void)testArray
{
    int capacity = MIN(1024, rand());
    int random   = MIN(1024, arc4random() % 100);
    // Create array
    LSQArrayRef array = NewLSQArray(capacity, &kLSQNodeVtableCGColor);
    XCTAssert(LSQArrayGetCount(array)    == 0);
    XCTAssert(LSQArrayGetCapacity(array) == capacity);
    // Test 1
    {
        // Add nodes
        random    = MIN(random, MIN(1024, arc4random() % 1000));
        int count = (int)LSQArrayGetCount(array) + random;
        [self addNumberOfNodes:random toArray:array];
        XCTAssert(LSQArrayGetCount(array) == count);
    }
    // Test 2
    {
        // Remove nodes
        random    = MIN(random, MIN(1024, arc4random() % 100));
        int count = (int)LSQArrayGetCount(array) - random;
        [self removeNumberOfNodes:random fromArray:array];
        XCTAssert(LSQArrayGetCount(array) == count);
    }
    // Test 3
    {
        // Add nodes
        random    = MIN(random, MIN(1024, arc4random() % 1000));
        int count = (int)LSQArrayGetCount(array) + random;
        [self addNumberOfNodes:random toArray:array];
        XCTAssert(LSQArrayGetCount(array) == count);
    }
    // Test 4
    {
        // Remove nodes
        random    = MIN(random, MIN(1024, arc4random() % 100));
        int count = (int)LSQArrayGetCount(array) - random;
        [self removeNumberOfNodes:random fromArray:array];
        XCTAssert(LSQArrayGetCount(array) == count);
    }
    // Test 5
    {
        // Add nodes
        random    = MIN(random, MIN(1024, arc4random() % 1000));
        int count = (int)LSQArrayGetCount(array) + random;
        [self addNumberOfNodes:random toArray:array];
        XCTAssert(LSQArrayGetCount(array) == count);
    }
    // Remove all
    LSQArrayRemoveAllValues(array);
    // Array should be empty
    CFIndex count = LSQArrayGetCount(array);
    XCTAssert(count == 0);
    // Release
    LSQArrayRelease(array);
}

- (void)testArrayRemove
{
    int capacity = MIN(1024, rand());
    // Create array
    LSQArrayRef array1 = NewLSQArray(capacity, &kLSQNodeVtableCGColor);
    XCTAssert(LSQArrayGetCount(array1)    == 0);
    XCTAssert(LSQArrayGetCapacity(array1) == capacity);
    // Create NSArray with random numbers
    NSMutableArray *array2 = [NSMutableArray array];
    // Fill arrays with random numbers
    for (int i = 0; i < capacity; ++i)
    {
        NSNumber *num = [NSNumber numberWithInt:MIN(1024, arc4random() % 100)];
        // Add to arrays
        LSQArrayInsertValueAtIndex(array1, i, (__bridge void *)(num));
        [array2 insertObject:num atIndex:i];
    }
    // Check array conts
    XCTAssert(LSQArrayGetCount(array1)    == array2.count);
    XCTAssert(LSQArrayGetCapacity(array1) == capacity);
    // Remove random element from arrays
    {
        int index = MIN(1024, arc4random() % 100);
        LSQArrayRemoveValueAtIndex(array1, index);
        [array2 removeObjectAtIndex:index];
        // Check equality of object at removed index
        NSNumber *num1 = (__bridge NSNumber *)(LSQArrayGetValueAtIndex(array1, index));
        NSNumber *num2 = [array2 objectAtIndex:index];
        XCTAssert(num1.intValue == num2.intValue);
    }
    // Check array conts
    XCTAssert(LSQArrayGetCount(array1)    == array2.count);
    XCTAssert(LSQArrayGetCapacity(array1) == capacity);
    // Check equality of arrays
    for (int i = 0; i < LSQArrayGetCount(array1); ++i)
    {
        NSNumber *num1 = (__bridge NSNumber *)(LSQArrayGetValueAtIndex(array1, i));
        NSNumber *num2 = [array2 objectAtIndex:i];
        XCTAssert(num1.intValue == num2.intValue);
    }
    // Remove all
    LSQArrayRemoveAllValues(array1);
    [array2 removeAllObjects];
    // Check array conts
    XCTAssert(LSQArrayGetCount(array1)    == array2.count);
    XCTAssert(LSQArrayGetCapacity(array1) == capacity);
    // Release
    LSQArrayRelease(array1);
}

- (void)testArrayDinamicSize
{
    // Create array
    LSQArrayRef array = NewLSQArray(0, &kLSQNodeVtableCGColor);
    XCTAssert(LSQArrayGetCount(array)    == 0);
    XCTAssert(LSQArrayGetCapacity(array) == 0);
    // Test 1
    {
        // Add nodes
        int random = MIN(random, MIN(1024, arc4random() % 1000));
        int count  = (int)LSQArrayGetCount(array) + random;
        [self addNumberOfNodes:random toArray:array];
        XCTAssert(LSQArrayGetCount(array) == count);
    }
    // Test 2
    {
        // Remove nodes
        int random = MIN(random, MIN(1024, arc4random() % 100));
        int count  = (int)LSQArrayGetCount(array) - random;
        [self removeNumberOfNodes:random fromArray:array];
        XCTAssert(LSQArrayGetCount(array) == count);
    }
    // Test 3
    {
        // Add nodes
        int random = MIN(random, MIN(1024, arc4random() % 1000));
        int count  = (int)LSQArrayGetCount(array) + random;
        [self addNumberOfNodes:random toArray:array];
        XCTAssert(LSQArrayGetCount(array) == count);
    }
    // Test 4
    {
        // Remove nodes
        int random = MIN(random, MIN(1024, arc4random() % 100));
        int count  = (int)LSQArrayGetCount(array) - random;
        [self removeNumberOfNodes:random fromArray:array];
        XCTAssert(LSQArrayGetCount(array) == count);
    }
    // Test 5
    {
        // Add nodes
        int random = MIN(random, MIN(1024, arc4random() % 1000));
        int count  = (int)LSQArrayGetCount(array) + random;
        [self addNumberOfNodes:random toArray:array];
        XCTAssert(LSQArrayGetCount(array) == count);
    }
    // Remove all
    LSQArrayRemoveAllValues(array);
    // Array should be empty
    CFIndex count = LSQArrayGetCount(array);
    XCTAssert(count == 0);
    // Release
    LSQArrayRelease(array);
}

@end
