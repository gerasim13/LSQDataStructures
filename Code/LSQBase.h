//
//  LSQBase.h
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 26.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LSQDataStructures_LSQBase_h
#define LSQDataStructures_LSQBase_h

#import <CoreFoundation/CoreFoundation.h>
#import "MacTypes.h"
#import "LSQAllocator.h"

//________________________________________________________________________________________

CF_EXTERN_C_BEGIN
    
//________________________________________________________________________________________
    
typedef struct       LSQBaseType   *LSQBaseTypeRef;
typedef const struct LSQBaseVtable *LSQBaseVtableRef;

//________________________________________________________________________________________

#define NO_NULL(...) __attribute__((nonnull (__VA_ARGS__)))

//________________________________________________________________________________________

#pragma mark - Data Structures

// Ref counter and version
typedef struct LSQBaseType_Data
{
    int32_t refcount;
    int32_t version;
    void*   userdata;                     // This will be passed into all callbacks
    void    (*dealloc)(void*) NO_NULL(1); // Dealloc callback
} LSQBaseType_Data;

// Memory management functions
typedef const struct LSQBaseVtable
{
    void* (*retain)  (void*) NO_NULL(1);
    void  (*release) (void*) NO_NULL(1);
} LSQBaseVtable;

//________________________________________________________________________________________
    
#pragma mark - Functions

CF_EXPORT NO_NULL(2)   LSQBaseTypeRef NewLSQBaseType    (LSQBaseVtableRef, void(void*)); // AllocInit function
CF_EXPORT NO_NULL(1)   void           LSQBaseDealloc    (LSQBaseTypeRef);                // Dealloc function
CF_EXPORT NO_NULL(1)   void*          LSQBaseRetain     (LSQBaseTypeRef);                // Increment ref counter
CF_EXPORT NO_NULL(1)   void           LSQBaseRelease    (LSQBaseTypeRef);                // Decrement ref counter
CF_EXPORT NO_NULL(1)   int32_t        LSQBaseGetRefCount(LSQBaseTypeRef);                // Get ref count
CF_EXPORT NO_NULL(1,2) void           LSQBaseSetUserdata(LSQBaseTypeRef, void*);         // Set userdata

#pragma mark - Macros

#define LSQALLOCK(type, vtable, dealloc) ({\
type *__self = LSQAllocatorAllocType(type);\
__self->base = NewLSQBaseType(vtable, (void*)dealloc);\
LSQBaseSetUserdata(__self->base, __self);\
__self;\
})

//________________________________________________________________________________________

CF_EXTERN_C_END

//________________________________________________________________________________________
    
#endif
