//
//  LSQHashSet.h
//  TheMightyMIDISequencer
//
//  Created by Павел Литвиненко on 20.03.14.
//  Copyright 2014 Casual Underground. All rights reserved.
//

#ifndef LSQDataStructures_LSQHashSet_h
#define LSQDataStructures_LSQHashSet_h

#import <CoreFoundation/CoreFoundation.h>
#import "LSQBase.h"

//________________________________________________________________________________________

/* HashSet data structure base on https://github.com/avsej/hashset.c */

//________________________________________________________________________________________

CF_EXTERN_C_BEGIN

//________________________________________________________________________________________

typedef       struct LSQHashSet         *LSQHashSetRef;
typedef       struct LSQHashSetIterator *LSQHashSetIteratorRef;
typedef const        void               *LSQHashSetElement;
typedef const struct LSQHashSetVtable   *LSQHashSetVtableRef;
typedef void (^LSQHashSetBlock)(const void *data);

//________________________________________________________________________________________

#pragma mark - Data Structures

typedef struct LSQHashSet_Data
{
    size_t  nbits;
    size_t  mask;
    int32_t capacity;            // Max number of elements
    int32_t count;               // Current number of elements
    LSQHashSetElement *elements; // Array of elements
} LSQHashSet_Data;

// Private functions
typedef const struct LSQHashSetVtable
{
    NO_NULL(1,2) Boolean  (*is_member)  (LSQHashSetRef, LSQHashSetElement);    // Check if hasmap contains element
    NO_NULL(1,2) OSStatus (*insert)     (LSQHashSetRef, LSQHashSetElement);    // Add item
    NO_NULL(1,2) OSStatus (*remove)     (LSQHashSetRef, LSQHashSetElement);    // Remove element
    NO_NULL(1)   OSStatus (*remove_all) (LSQHashSetRef);                       // Remove all elements
    NO_NULL(1,2) void     (*iterate)    (LSQHashSetRef, LSQHashSetBlock, int); // Iterate over all elements in set
} LSQHashSetVtable;

//________________________________________________________________________________________

#pragma mark - Functions

CF_EXPORT              LSQHashSetRef NewLSQHashSet         ();                 // Constructor
CF_EXPORT NO_NULL(1)   LSQHashSetRef LSQHashSetRetain      (LSQHashSetRef);    // Retain
CF_EXPORT NO_NULL(1)   void          LSQHashSetRelease     (LSQHashSetRef);    // Release
CF_EXPORT NO_NULL(1)   void          LSQHashSetDealloc     (LSQHashSetRef);    // Free memory
CF_EXPORT NO_NULL(1)   CFIndex       LSQHashSetGetCount    (LSQHashSetRef);
CF_EXPORT NO_NULL(1)   CFIndex       LSQHashSetGetCapacity (LSQHashSetRef);
CF_EXPORT NO_NULL(1,2) Boolean       LSQHashSetContainsItem(LSQHashSetRef, LSQHashSetElement);
CF_EXPORT NO_NULL(1,2) Boolean       LSQHashSetAddItem     (LSQHashSetRef, LSQHashSetElement);
CF_EXPORT NO_NULL(1,2) Boolean       LSQHashSetRemoveItem  (LSQHashSetRef, LSQHashSetElement);
CF_EXPORT NO_NULL(1)   Boolean       LSQHashSetRemoveAll   (LSQHashSetRef);
CF_EXPORT NO_NULL(1,2) void          LSQHashSetIterate     (LSQHashSetRef, LSQHashSetBlock);
CF_EXPORT NO_NULL(1,2) void          LSQHashSetIterateSync (LSQHashSetRef, LSQHashSetBlock);
CF_EXPORT NO_NULL(1,2) void          LSQHashSetIterateAsync(LSQHashSetRef, LSQHashSetBlock);

//________________________________________________________________________________________

CF_EXTERN_C_END

//________________________________________________________________________________________

#endif
