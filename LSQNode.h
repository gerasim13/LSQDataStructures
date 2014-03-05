//
//  LSQNode.h
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 10.09.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LoopsequeDJ_LSQNode_h
#define LoopsequeDJ_LSQNode_h

#import <CoreFoundation/CoreFoundation.h>
#import "MacTypes.h"
#import "LSQBase.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#pragma mark - Data Structures
    
    LSQClass_Start(LSQNode, LSQTypeRef);
        const void* data;
        void*   front;
        void*   back;
        CFIndex index;
        // Class functions
        struct
        {
            // Reserved
        };
    LSQClass_End(LSQNode);
    
#pragma mark - Functions
    
    // Shorthand for creating node
    LSQNode LSQNodeMake(const void *data);
    // Memory management
    LSQNode LSQNodeRetain(LSQNode);
    void    LSQNodeRelease(LSQNode);
    
#ifdef __cplusplus
}
#endif

#endif
