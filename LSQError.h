//
//  LSQError.h
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 27.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LoopsequeDJ_LSQError_h
#define LoopsequeDJ_LSQError_h

#import <CoreFoundation/CoreFoundation.h>
#import "MacTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // Error codes
    typedef enum
    {
        // Queue codes
        LSQQueueError_NoError      = 0,
        LSQQueueError_InvalidArgs  = 1,
        LSQQueueError_OutOfMemory  = 2,
        LSQQueueError_HeadTailNull = 3,
        LSQQueueError_QueueEmpty   = 4,
        // Array codes
        LSQArrayError_NoError        = 5,
        LSQArrayError_InvalidArgs    = 6,
        LSQArrayError_OutOfMemory    = 7,
        LSQArrayError_IndexOutBounds = 8,
        LSQArrayError_ArrayNotInit   = 9
    } LSQErrorCode;
    // Data structure
    typedef struct LSQError
    {
        int8_t code;
        int8_t line;
    } LSQError;
    // NULL macro
    #define LSQErrorNULL ({ \
    LSQError error; \
    error; \
    })
    
    // Functions
    void LSQErrorMake(LSQError *error, LSQErrorCode code);
    void LSQErrorLog(LSQError *error);
    
#ifdef __cplusplus
}
#endif
    
#endif
