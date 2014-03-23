//
//  LSQHashSetTest.m
//  TheMightyMIDISequencer
//
//  Created by Павел Литвиненко on 20.03.14.
//  Copyright (c) 2014 Casual Underground. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "LSQHashSet.h"

@interface LSQHashSetTest : XCTestCase
@end

@implementation LSQHashSetTest

- (void)testInsertNumberInHashSet
{
    LSQHashSetRef set = NewLSQHashSet();
    NSNumber *number = @(rand());
    LSQHashSetAddItem(set, (__bridge LSQHashSetElement)(number));
    XCTAssert(LSQHashSetContainsItem(set, (__bridge LSQHashSetElement)(number)));
    LSQHashSetRemoveAll(set);
    XCTAssert(LSQHashSetGetCount(set) == 0);
    LSQHashSetRelease(set);
}

- (void)testInsertManyNumbersInHashSet
{
    LSQHashSetRef set = NewLSQHashSet();
    for (int i = 0; i < MAX(arc4random() % 1000, 1024); ++i)
    {
        NSNumber *number = @(rand());
        LSQHashSetAddItem(set, (__bridge LSQHashSetElement)(number));
        XCTAssert(LSQHashSetContainsItem(set, (__bridge LSQHashSetElement)(number)));
    }
    LSQHashSetRemoveAll(set);
    XCTAssert(LSQHashSetGetCount(set) == 0);
    LSQHashSetRelease(set);
}

- (void)testEqualityOfNumbersInHashSet
{
    NSMutableArray *array = [NSMutableArray array];
    LSQHashSetRef set     = NewLSQHashSet();
    for (int i = 0; i < MAX(arc4random() % 1000, 1024); ++i)
    {
        NSNumber *number = @(rand());
        [array addObject:number];
    }
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *number = array[i];
        LSQHashSetAddItem(set, (__bridge LSQHashSetElement)(number));
        XCTAssert(LSQHashSetContainsItem(set, (__bridge LSQHashSetElement)(number)));
    }
    XCTAssert(LSQHashSetGetCount(set) == array.count);
    for (int i = 0; i < array.count; ++i)
    {
        NSNumber *number = array[i];
        XCTAssert(LSQHashSetContainsItem(set, (__bridge LSQHashSetElement)(number)));
    }
    LSQHashSetRemoveAll(set);
    XCTAssert(LSQHashSetGetCount(set) == 0);
    LSQHashSetRelease(set);
}

@end
