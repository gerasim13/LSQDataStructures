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

@end
