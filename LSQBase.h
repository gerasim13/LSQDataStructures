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

#pragma mark - Data Structures

// Ref counter and version
typedef struct LSQBaseType_Data
{
    int32_t refcount;
    int32_t version;
    void    (*dealloc)(void*); // Dealloc callback
    void*   userdata;          // This will be passed into all callbacks
} LSQBaseType_Data;

// Memory management functions
typedef const struct LSQBaseVtable
{
    void* (*retain)  (void*);
    void  (*release) (void*);
} LSQBaseVtable;

//________________________________________________________________________________________
    
#pragma mark - Functions

CF_EXPORT LSQBaseTypeRef NewLSQBaseType    (LSQBaseVtableRef, void(void*)); // AllocInit function
CF_EXPORT void           LSQBaseDealloc    (LSQBaseTypeRef);                // Dealloc function
CF_EXPORT void*          LSQBaseRetain     (LSQBaseTypeRef);                // Increment ref counter
CF_EXPORT void           LSQBaseRelease    (LSQBaseTypeRef);                // Decrement ref counter
CF_EXPORT int32_t        LSQBaseGetRefCount(LSQBaseTypeRef);                // Get ref count
CF_EXPORT void           LSQBaseSetUserdata(LSQBaseTypeRef, void*);         // Set userdata

#pragma mark - Macros

#define LSQALLOCK(type, vtable, dealloc) ({\
type *self = LSQAllocatorAllocType(type);\
self->base = NewLSQBaseType(vtable, (void*)dealloc);\
LSQBaseSetUserdata(self->base, self);\
self;\
})

//#define LSQDEALLOCK(expr) ({\
//LSQBaseDealloc(expr->base);\
//LSQAllocatorDealloc(expr);\
//})

//________________________________________________________________________________________

CF_EXTERN_C_END

//________________________________________________________________________________________
    
#endif
