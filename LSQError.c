//
//  LSQError.c
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 27.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#include "LSQError.h"
#include <stdio.h>

void LSQErrorMake(LSQError *error, LSQErrorCode code)
{
    error->code = code;
    error->line = __LINE__;
}

void LSQErrorLog(LSQError *error)
{
    switch (error->code)
    {
        case LSQQueueError_NoError:
        {
            printf("Not an Error.\n");
            break;
        }
        case LSQQueueError_InvalidArgs:
        {
            fprintf(stderr, "IN: %s @ %d: Invalid Args.\n", __FILE__, error->line);
            break;
        }
        case LSQQueueError_OutOfMemory:
        {
            fprintf(stderr,"IN: %s @%d: Out of Memory.\n", __FILE__, error->line);
            break;
        }
        case LSQQueueError_HeadTailNull:
        {
            fprintf(stderr, "IN: %s @%d: Serious error.", __FILE__, error->line);
            fprintf(stderr,"List one of the list's head/tail is null while other is not.\n");
            break;
        }
        case LSQQueueError_QueueEmpty:
        {
            printf("Queue is Empty.\n");
            break;
        }
    }
}
