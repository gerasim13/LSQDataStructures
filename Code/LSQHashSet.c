//
//  LSQHashSet.m
//  TheMightyMIDISequencer
//
//  Created by Павел Литвиненко on 20.03.14.
//  Copyright 2014 Casual Underground. All rights reserved.
//

#import "LSQHashSet.h"
#import "LSQCommon.h"

////________________________________________________________________________________________
//
//#pragma mark - SuperFastHash Macro
//
//#undef get16bits
//#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
//|| defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
//#define get16bits(d) (*((const uint16_t *) (d)))
//#endif
//
//#if !defined (get16bits)
//#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
//+(uint32_t)(((const uint8_t *)(d))[0]) )
//#endif
//
////________________________________________________________________________________________
//
//#pragma mark - SuperFastHash Function
//
///* http://www.azillionmonkeys.com/qed/hash.html */
//
//uint32_t SuperFastHash (const char * data, int len, uint32_t hash)
//{
//    uint32_t tmp;
//    int rem;
//    
//    if (len <= 0 || data == NULL) return 0;
//    
//    rem = len & 3;
//    len >>= 2;
//    
//    /* Main loop */
//    for (;len > 0; len--)
//    {
//        hash  += get16bits (data);
//        tmp    = (get16bits (data+2) << 11) ^ hash;
//        hash   = (hash << 16) ^ tmp;
//        data  += 2*sizeof (uint16_t);
//        hash  += hash >> 11;
//    }
//    
//    /* Handle end cases */
//    switch (rem)
//    {
//        case 3: hash += get16bits (data);
//            hash ^= hash << 16;
//            hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
//            hash += hash >> 11;
//            break;
//        case 2: hash += get16bits (data);
//            hash ^= hash << 11;
//            hash += hash >> 17;
//            break;
//        case 1: hash += (signed char)*data;
//            hash ^= hash << 10;
//            hash += hash >> 1;
//    }
//    
//    /* Force "avalanching" of final 127 bits */
//    hash ^= hash << 3;
//    hash += hash >> 5;
//    hash ^= hash << 4;
//    hash += hash >> 17;
//    hash ^= hash << 25;
//    hash += hash >> 6;
//    
//    return hash;
//}

//________________________________________________________________________________________

static const unsigned int prime_1 = 73;
static const unsigned int prime_2 = 5009;

//________________________________________________________________________________________

#pragma mark - Data Structures

typedef struct LSQHashSet
{
    LSQBaseTypeRef      base;
    LSQHashSetVtableRef vtable;
    LSQHashSet_Data     data;
} LSQHashSet;

typedef struct LSQHashSetIterator
{
    LSQHashSetRef set;
    int32_t index;
} LSQHashSetIterator;

// Error codes
enum
{
    LSQHashSetError_InvalidArgs = 999999L << 1,
    LSQHashSetError_OutOfMemory = 999999L << 2,
    LSQHashSetError_SetNotInit  = 999999L << 3
};

// Iterate options
enum
{
    LSQHashSetIterateOptions_None  = 0,
    LSQHashSetIterateOptions_Sync  = 1,
    LSQHashSetIterateOptions_Async = 2,
};

//________________________________________________________________________________________

#pragma mark - Iterator

bool hashset_iterator_has_next(LSQHashSetIteratorRef itr)
{
    // Empty or end of the set
    if (itr->set->data.count != 0 && itr->index != itr->set->data.capacity - 1)
    {
        // Peek to find another entry
        int32_t index = itr->index;
        while (index <= itr->set->data.capacity - 1)
        {
            if (itr->set->data.elements[index] != 0)
            {
                return true;
            }
            ++index;
        }
    }
    return false;
}

bool hashset_iterator_next(LSQHashSetIteratorRef itr)
{
    
    if (!hashset_iterator_has_next(itr))
    {
        return false;
    }
    else
    {
        itr->index++;
    }
    while (itr->set->data.elements[(itr->index)] == 0 && itr->index < itr->set->data.capacity)
    {
        itr->index++;
    }
    return true;
}

LSQHashSetElement hashset_iterator_value(LSQHashSetIteratorRef itr)
{
    /* Check to verify we're not at a null value, this can happen if an iterator is created
     * before items are added to the set.
     */
    if (itr->set->data.elements[itr->index] == 0)
    {
        hashset_iterator_next(itr);
    }
    return itr->set->data.elements[itr->index];
}

LSQHashSetIteratorRef NewLSQHashSetIterator(LSQHashSetRef set)
{
    LSQHashSetIteratorRef itr = LSQAllocatorAllocType(LSQHashSetIterator);
    itr->set   = set;
    itr->index = 0;
//    /* advance to the first item if one is present */
//    if (set->data.count > 0) hashset_iterator_next(itr);
    return itr;
}

//________________________________________________________________________________________

#pragma mark - Hidden Functions

OSStatus element_and_set_check(LSQHashSetRef set, LSQHashSetElement element)
{
    if (set == NULL || element == NULL)
    {
        return LSQHashSetError_InvalidArgs;
    }
    else if (set->data.elements == NULL)
    {
        return LSQHashSetError_SetNotInit;
    }
    return noErr;
}

OSStatus set_check(LSQHashSetRef set)
{
    if (set == NULL)
    {
        return LSQHashSetError_InvalidArgs;
    }
    else if (set->data.elements == NULL)
    {
        return LSQHashSetError_SetNotInit;
    }
    return noErr;
}

bool check_if_element_is_in_set(LSQHashSetRef set, LSQHashSetElement element)
{
    size_t ii = set->data.mask & (prime_1 * (size_t)element);
    
    while (set->data.elements[ii] != 0)
    {
        if (set->data.elements[ii] == element)
        {
            return true;
        } else
        {
            ii = set->data.mask & (ii + prime_2);
        }
    }
    return false;
}

bool try_insert_element(LSQHashSetRef set, LSQHashSetElement element)
{
    size_t ii = set->data.mask & (prime_1 * (size_t)element);
    
    while ((size_t)set->data.elements[ii] != 0 &&
           (size_t)set->data.elements[ii] != 1)
    {
        if (set->data.elements[ii] == element)
        {
            return false;
        }
        else
        {
            /* search free slot */
            ii = set->data.mask & (ii + prime_2);
        }
    }
    return ATOMICSWAP_PTR(set->data.elements[ii], element);
}

bool try_remove_element(LSQHashSetRef set, LSQHashSetElement element)
{
    size_t ii      = set->data.mask & (prime_1 * (size_t)element);
    bool   removed = false;
    while (!removed)
    {
        // Check index
        if (ii < set->data.count - 1)
        {
            // Shift elements
            LSQHashSetElement next = set->data.elements[ii + 1];
            // Move memory blocks
            memmove(&set->data.elements[ii],
                    &set->data.elements[ii + 1],
                    (set->data.count - ii - 1) * sizeof(LSQHashSetElement));
            // Put next node at removed index
            removed = ATOMICSWAP_PTR(set->data.elements[ii], next);
        }
        else if (set->data.elements[ii] == element)
        {
            // Delete element
            removed = ATOMICSWAP_PTR(set->data.elements[ii], NULL);
        }
        else
        {
            ii = set->data.mask & (ii + prime_2);
        }
    }
    return removed;
}

bool try_rehash_set(LSQHashSetRef set)
{
    LSQHashSetElement *old_items;
    int32_t old_capacity, ii;
    
    if ((float)set->data.count >= (size_t)((double)set->data.capacity * 0.85))
    {
        old_items    = set->data.elements;
        old_capacity = set->data.capacity;
        set->data.nbits++;
        set->data.capacity = (1 << set->data.nbits);
        set->data.mask     = set->data.capacity - 1;
        set->data.elements = LSQAllocatorAllocSize(sizeof(LSQHashSetElement) * set->data.capacity);
        set->data.count    = 0;
        memset(&set->data.elements[0], 0, set->data.capacity * sizeof(set->data.elements[0]));
        assert(set->data.elements);
        // Iterate over all elements
        for (ii = 0; ii < old_capacity; ++ii)
        {
            LSQHashSetAddItem(set, old_items[ii]);
        }
        LSQAllocatorDealloc(old_items);
    }
    return true;
}

//________________________________________________________________________________________

#pragma mark - Private Functions

void iterate_with_block(LSQHashSetRef self, LSQHashSetBlock block, int options)
{
    LSQHashSetIteratorRef iter = NewLSQHashSetIterator(self);
    while(hashset_iterator_has_next(iter))
    {
        if (block != NULL)
        {
            LSQHashSetElement value = hashset_iterator_value(iter);
            switch (options)
            {
                case LSQHashSetIterateOptions_Sync:
                {
                    dispatch_sync(dispatch_get_main_queue(), ^{
                        block(value);
                    });
                    break;
                }
                case LSQHashSetIterateOptions_Async:
                {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        block(value);
                    });
                    break;
                }
                default:
                {
                    block(value);
                    break;
                }
            }
        }
        hashset_iterator_next(iter);
    }
    LSQAllocatorDealloc(iter);
}

Boolean contains_element(LSQHashSetRef self, LSQHashSetElement element)
{
    OSStatus status;
    if ((status = element_and_set_check(self, element)) != noErr)
    {
        return status;
    }
    return check_if_element_is_in_set(self, element);
}

OSStatus insert_element(LSQHashSetRef self, LSQHashSetElement element)
{
    OSStatus status;
    if ((status = element_and_set_check(self, element)) != noErr)
    {
        return status;
    }
    // Insert new element
    if (try_insert_element(self, element) && ATOMICINCREMENT_INT32(self->data.count))
    {
        // Rehash set
        try_rehash_set(self);
    }
    return status;
}

OSStatus remove_element(LSQHashSetRef self, LSQHashSetElement element)
{
    OSStatus status;
    if ((status = element_and_set_check(self, element)) != noErr)
    {
        return status;
    }
    // Remove element
    if (try_remove_element(self, element))
    {
        // Update count
        ATOMICDECRIMENT_INT32(self->data.count);
    }
    return status;
}

OSStatus remove_all_elements(LSQHashSetRef self)
{
    LSQHashSetIteratorRef iter = NewLSQHashSetIterator(self);
    while(hashset_iterator_has_next(iter))
    {
        // Remove element
        LSQHashSetRemoveItem(self, hashset_iterator_value(iter));
    }
    LSQAllocatorDealloc(iter);
    return noErr;
}

//________________________________________________________________________________________

static const struct LSQHashSetVtable kLSQHashSetDefaultVtable = {
    &contains_element,
    &insert_element,
    &remove_element,
    &remove_all_elements,
    &iterate_with_block
};

//________________________________________________________________________________________

#pragma mark - Base Functions

LSQHashSetRef NewLSQHashSet ()
{
    LSQHashSetRef self = LSQALLOCK(LSQHashSet, NULL, (void*)&LSQHashSetDealloc);
    // Set properties
    self->data.nbits    = 3;
    self->data.count    = 0;
    self->data.capacity = (int32_t)(1 << self->data.nbits);
    self->data.mask     = self->data.capacity - 1;
    self->data.elements = LSQAllocatorAllocSize(sizeof(LSQHashSetElement) * self->data.capacity);
    self->vtable        = &kLSQHashSetDefaultVtable;
    // Fill set with NULL
    memset(&self->data.elements[0], 0, self->data.capacity * sizeof(self->data.elements[0]));
    return self;
}

LSQHashSetRef LSQHashSetRetain (LSQHashSetRef self)
{
    self->base = LSQBaseRetain(self->base);
    return self;
}

void LSQHashSetRelease (LSQHashSetRef self)
{
    LSQBaseRelease(self->base);
}

void LSQHashSetDealloc (LSQHashSetRef self)
{
    // Release elements
    if (self->vtable->remove_all != NULL)
    {
        self->vtable->remove_all(self);
    }
    ATOMICSWAP_PTR(self->base, NULL);
    // Dealloc set
    LSQAllocatorDealloc(self->data.elements);
    LSQAllocatorDealloc(self);
}

//________________________________________________________________________________________

#pragma mark - Public Functions

CFIndex LSQHashSetGetCount(LSQHashSetRef self)
{
    if (self)
    {
        return self->data.count;
    }
    return NAN;
}

CFIndex LSQHashSetGetCapacity(LSQHashSetRef self)
{
    if (self)
    {
        return self->data.capacity;
    }
    return NAN;
}

Boolean LSQHashSetAddItem(LSQHashSetRef self, LSQHashSetElement element)
{
    if (element && self && self->vtable->insert != NULL)
    {
        return self->vtable->insert(self, element) == noErr;
    }
    return false;
}

Boolean LSQHashSetRemoveItem(LSQHashSetRef self, LSQHashSetElement element)
{
    if (element && self  && self->vtable->remove != NULL)
    {
        return self->vtable->remove(self, element) == noErr;
    }
    return false;
}

Boolean LSQHashSetRemoveAll(LSQHashSetRef self)
{
    if (self && self->vtable->remove_all != NULL)
    {
        return self->vtable->remove_all(self) == noErr;
    }
    return false;
}

Boolean LSQHashSetContainsItem(LSQHashSetRef self, LSQHashSetElement element)
{
    if (self && self->vtable->is_member != NULL)
    {
        return self->vtable->is_member(self, element);
    }
    return false;
}

void LSQHashSetIterate(LSQHashSetRef self, LSQHashSetBlock block)
{
    if (self && self->vtable->iterate != NULL)
    {
        return self->vtable->iterate(self, block, 0);
    }
}

void LSQHashSetIterateSync(LSQHashSetRef self, LSQHashSetBlock block)
{
    if (self && self->vtable->iterate != NULL)
    {
        return self->vtable->iterate(self, block, 1);
    }
}

void LSQHashSetIterateAsync(LSQHashSetRef self, LSQHashSetBlock block)
{
    if (self && self->vtable->iterate != NULL)
    {
        return self->vtable->iterate(self, block, 2);
    }
}
