//
//  LSQAllocator.h
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 26.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LoopsequeDJ_LSQAllocator_h
#define LoopsequeDJ_LSQAllocator_h

#import <CoreFoundation/CoreFoundation.h>
#import "MacTypes.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    // Allocator variable
    CF_EXPORT
    CFAllocatorRef LSQLocklessAllocator;
    
    // Allocator setter
    CF_EXPORT
    void LSQAllocatorSetCurrentAllocator(CFAllocatorRef);
    
    // Allock macro with size
    #define LSQAllocatorAllocSize(size) \
    CFAllocatorAllocate(LSQLocklessAllocator, size, 0)
    
    // Allock macro with type
    #define LSQAllocatorAllocType(type) \
    LSQAllocatorAllocSize(sizeof(type))
    
    // Dealloc macro
    #define LSQAllocatorDealloc(ptr) \
    CFAllocatorDeallocate(LSQLocklessAllocator, ptr);
    
    // Realloc macro
    #define LSQAllocatorRealloc(ptr, size) \
    CFAllocatorReallocate(LSQLocklessAllocator, ptr, size, 0);
    
#ifdef __cplusplus
}
#endif

#endif
