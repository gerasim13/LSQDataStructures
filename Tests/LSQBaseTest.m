//
//  LSQBaseTest.m
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 26.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "LSQBase.h"

@interface LSQBaseTest : XCTestCase
@end

@implementation LSQBaseTest

- (void)testLSQTypeShouldBeReleased
{
    LSQBaseTypeRef type = NewLSQBaseType(NULL, NULL);
    XCTAssert(LSQBaseGetRefCount(type) == 0);
    // Retain
    LSQBaseRetain(type);
    XCTAssert(LSQBaseGetRefCount(type) == 1);
    // Retain again
    LSQBaseRetain(type);
    XCTAssert(LSQBaseGetRefCount(type) == 2);
    // Release
    LSQBaseRelease(type);
    XCTAssert(LSQBaseGetRefCount(type) == 1);
    // Release and dealloc
    LSQBaseRelease(type);
    XCTAssert(LSQBaseGetRefCount(type) <= 0);
}

@end
