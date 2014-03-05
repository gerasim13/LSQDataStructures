//
//  LSQBase.h
//  LoopsequeDJ
//
//  Created by Павел Литвиненко on 26.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#ifndef LoopsequeDJ_LSQBase_h
#define LoopsequeDJ_LSQBase_h

#import <CoreFoundation/CoreFoundation.h>
#import "MacTypes.h"
#import "LSQAllocator.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#pragma mark - Data Structures
    
    // Vtable for base structure
    const struct v_table {
        void* (*retain)  (void*);
        void  (*release) (void*);
        void  (*dealloc) (void*);
    } v_table;
    typedef const struct v_table *LSQVTableRef;
    
    // Base structure of all LSQ objects
    struct base_type {
        LSQVTableRef vtable;
        int32_t refcount;
        int32_t version;
    } base_type;
    typedef struct base_type *LSQTypeRef;
    
#pragma mark - Macros
    
    /* Single Inheritance Realization    */
    /* Used for memory management in LSQ */
    
    // Every class should start with this
    #define LSQClass_Start(class, superclass) \
    struct class##_struct { \
    superclass super;
    // And end with this macro
    #define LSQClass_End(class) \
    } class##_struct; \
    typedef struct class##_struct *class;
    
    // Macro for creation base type
    #define LSQSuperInit(retain, release, dealloc) ({ \
    /* First we need to create new v_table */ \
    const struct v_table vtable = { retain, release, dealloc }; \
    /* Then we should pass ref point of v_table to constructor fuction */ \
    LSQVTableRef vtableRef = &vtable; \
    LSQTypeRef   baseType  = LSQTypeNew(vtableRef); \
    /* Return pointer to base type and end loop */ \
    baseType; \
    })
    
    // Macro for super retain
    #define LSQSuperRetain(self) ({ \
    LSQBaseRetain(self->super); \
    self; \
    })
    
    // Macro for destruction base type
    #define LSQSuperRelease(self) \
    LSQTypeRef super = self->super; \
    LSQBaseRelease(super); \
    /* Deallocate self */ \
    if (super->refcount <= 0) \
    { super->vtable->dealloc(self); }
    
    // Macro for super dealloc
    #define LSQSuperDealloc(self) \
    LSQTypeRef super = self->super; \
    LSQBaseDealloc(super); \
    self->super = NULL; \
    LSQAllocatorDealloc(_self); \
    self = NULL;
    
    // Macro for creation new object
    #define LSQInit(type, retain, release, dealloc) ({ \
    struct type##_struct *obj = LSQAllocatorAllocType(type##_struct); \
    type self   = (type)obj; \
    self->super = LSQSuperInit(retain, release, dealloc); \
    self; \
    })
    
    // Usage exmple:
    /*
    LSQClass_Start(ExampleClass, LSQTypeRef)
    int a;
    int b;
    LSQClass_End(ExampleClass)
    */
    
#pragma mark - Functions
    
    // AllocInit function
    CF_EXPORT
    LSQTypeRef LSQTypeNew(LSQVTableRef);
    // Dealloc function
    CF_EXPORT
    void LSQBaseDealloc(void*);
    // Increment ref counter
    CF_EXPORT
    void* LSQBaseRetain(void*);
    // Decrement ref counter
    CF_EXPORT
    void LSQBaseRelease(void*);
    
#ifdef __cplusplus
}
#endif

#endif
