//
//  LSQNodeTest.m
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 05.03.14.
//  Copyright (c) 2014 Casual Underground. All rights reserved.
//

#import <XCTest/XCTest.h>
#import <UIKit/UIKit.h>
#import "LSQNode.h"

@interface LSQNodeTest : XCTestCase
@end

@implementation LSQNodeTest

- (void)testLSQNodeWithNumber
{
    // Create random number
    NSNumber *number = @(rand());
    // Create node with number
    LSQNodeRef node = NewLSQNode((__bridge_retained void *)(number), &kLSQNodeVtableCF);
    XCTAssert(LSQNodeGetRefCount(node) == 0);
    // Test number equality
    XCTAssert(number.intValue == [(NSNumber*)CFBridgingRelease(LSQNodeGetContent(node)) intValue]);
    // Retain node
    LSQNodeRetain(node);
    XCTAssert(LSQNodeGetRefCount(node) == 1);
    // Release node and dealloc content
    LSQNodeRelease(node);
}

- (void)testLSQNodeWithString
{
    // Create string with random number
    NSString *string = [NSString stringWithFormat:@"%d", rand()];
    // Create node with string
    LSQNodeRef node = NewLSQNode((__bridge_retained void *)(string), &kLSQNodeVtableCF);
    XCTAssert(LSQNodeGetRefCount(node) == 0);
    // Test string equality
    XCTAssert([string isEqualToString:(NSString*)CFBridgingRelease(LSQNodeGetContent(node))]);
    // Retain node
    LSQNodeRetain(node);
    XCTAssert(LSQNodeGetRefCount(node) == 1);
    // Release node and dealloc content
    LSQNodeRelease(node);
}

- (void)testLSQNodeWithColor
{
    // Create color
    UIColor *color = [UIColor redColor];
    // Create node with color
    LSQNodeRef node = NewLSQNode(color.CGColor, &kLSQNodeVtableCGColor);
    XCTAssert(LSQNodeGetRefCount(node) == 0);
    // Test string equality
    XCTAssert(color.CGColor == (CGColorRef)LSQNodeGetContent(node));
    // Retain node
    LSQNodeRetain(node);
    XCTAssert(LSQNodeGetRefCount(node) == 1);
    // Release node and dealloc content
    LSQNodeRelease(node);
}

- (void)testLSQNodeWithPath
{
    // Create color
    UIBezierPath *path = [UIBezierPath bezierPath];
    // Create node with color
    LSQNodeRef node = NewLSQNode((void*)path.CGPath, &kLSQNodeVtableCGPath);
    XCTAssert(LSQNodeGetRefCount(node) == 0);
    // Test string equality
    XCTAssert(path.CGPath == (CGPathRef)LSQNodeGetContent(node));
    // Retain node
    LSQNodeRetain(node);
    XCTAssert(LSQNodeGetRefCount(node) == 1);
    // Release node and dealloc content
    LSQNodeRelease(node);
}

@end
