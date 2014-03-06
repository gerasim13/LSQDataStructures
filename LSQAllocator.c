//
//  LSQAllocator.c
//  LSQDataStructures
//
//  Created by Павел Литвиненко on 26.08.13.
//  Copyright (c) 2013 Casual Underground. All rights reserved.
//

#include <stdio.h>
#include <malloc/malloc.h>
#include "LSQAllocator.h"
#include "nedmalloc.h"

#ifdef NEDMALLOC_H

//________________________________________________________________________________________

static malloc_zone_t          zone;
static malloc_introspection_t zone_introspect;

//________________________________________________________________________________________

#pragma mark - malloc_introspection_t callbacks

static kern_return_t ned_enumerator(task_t task, void *ptr, unsigned type_mask, vm_address_t zone_address, memory_reader_t reader, vm_range_recorder_t recorder)
{
    return 0;
}

static size_t ned_good_size(malloc_zone_t *zone, size_t size)
{
	size_t ret;
	void *ptr;
    
	/*
	 * Actually create an object of the appropriate size, then find out
	 * how large it could have been without moving up to the next size
	 * class.
	 */
	ptr = nedmalloc(size);
	if (ptr != NULL)
    {
        ret = nedblksize(0, (void *RESTRICT)ptr, 0);
		nedfree(ptr);
	}
    else
    {
        ret = size;
    }
	return (ret);
}

static boolean_t ned_check(malloc_zone_t *zone)
{
    return true;
}

static void ned_print(malloc_zone_t *zone, boolean_t verbose)
{
    
}
static void ned_log(malloc_zone_t *zone, void *address)
{
    
}

static void ned_force_lock(malloc_zone_t *zone)
{
}

static void ned_force_unlock(malloc_zone_t *zone)
{
}

static void ned_statistics(malloc_zone_t *zone, malloc_statistics_t *stats)
{
    
}

static boolean_t ned_zone_locked(malloc_zone_t *zone)
{
    return false;
}

static boolean_t ned_enable_discharge_checking(malloc_zone_t *zone)
{
    return false;
}

static void	ned_disable_discharge_checking(malloc_zone_t *zone)
{
    
}

static void	ned_discharge(malloc_zone_t *zone, void *memory)
{
    
}

static void ned_enumerate_discharged_pointers(malloc_zone_t *zone, void (^report_discharged)(void *memory, void *info))
{
    
}

//________________________________________________________________________________________

#pragma mark - malloc_zone_t callbacks

static size_t ned_size(struct _malloc_zone_t *zone, const void *ptr)
{
    return nedblksize(0, (void *RESTRICT)ptr, 0);
}

static void * ned_malloc(struct _malloc_zone_t *zone, size_t size)
{
    return nedmalloc(size);
}

static void * ned_calloc(struct _malloc_zone_t *zone, size_t num_items, size_t size)
{
    return nedcalloc(num_items, size);
}
static void * ned_valloc(struct _malloc_zone_t *zone, size_t size)
{
    return nedmalloc(size);
}

static void ned_free(struct _malloc_zone_t *zone, void *ptr)
{
    nedfree(ptr);
}

static void * ned_realloc(struct _malloc_zone_t *zone, void *ptr, size_t size)
{
    return nedrealloc(ptr, size);
}

static void * ned_destroy(struct _malloc_zone_t *zone)
{
    // This function should never be called.
	assert(false);
	return (NULL);
}

static unsigned ned_batch_malloc(malloc_zone_t *zone, size_t size, void **results, unsigned num_requested)
{
	// Don't bother implementing this interface, since it isn't required.
	return (0);
}

static void ned_batch_free(malloc_zone_t *zone, void **to_be_freed, unsigned num)
{
	unsigned i;
	for (i = 0; i < num; i++) ned_free(zone, to_be_freed[i]);
}

static void * ned_memalign(struct _malloc_zone_t *zone, size_t alignment, size_t size)
{
    return nedmemalign(alignment, size);
}

static void ned_free_definite_size(malloc_zone_t *zone, void *ptr, size_t size)
{
	assert(nedblksize(0, (void *RESTRICT)ptr, 0) == size);
    nedfree(ptr);
}

//________________________________________________________________________________________

#pragma mark - Malloc Zone Wrapper

static malloc_zone_t * malloc_ned_zone()
{
    zone.size               = (void *)ned_size;
	zone.malloc             = (void *)ned_malloc;
	zone.calloc             = (void *)ned_calloc;
	zone.valloc             = (void *)ned_valloc;
	zone.free               = (void *)ned_free;
	zone.realloc            = (void *)ned_realloc;
	zone.destroy            = (void *)ned_destroy;
	zone.zone_name          = "nedmalloc_zone";
	zone.batch_malloc       = ned_batch_malloc;
	zone.batch_free         = ned_batch_free;
	zone.introspect         = &zone_introspect;
	zone.version            = 7;
	zone.memalign           = ned_memalign;
	zone.free_definite_size = ned_free_definite_size;
    
	zone_introspect.enumerator                    = ned_enumerator;
	zone_introspect.good_size                     = ned_good_size;
	zone_introspect.check                         = ned_check;
	zone_introspect.print                         = ned_print;
	zone_introspect.log                           = ned_log;
	zone_introspect.force_lock                    = (void *)ned_force_lock;
	zone_introspect.force_unlock                  = (void *)ned_force_unlock;
	zone_introspect.statistics                    = ned_statistics;
	zone_introspect.zone_locked                   = ned_zone_locked;
    zone_introspect.enable_discharge_checking     = ned_enable_discharge_checking;
    zone_introspect.disable_discharge_checking    = ned_disable_discharge_checking;
    zone_introspect.discharge                     = ned_discharge;
    zone_introspect.enumerate_discharged_pointers = ned_enumerate_discharged_pointers;
    
	return (&zone);
}

#endif

//________________________________________________________________________________________

CFAllocatorRef kLSQLocklessAllocator = NULL;

//________________________________________________________________________________________

#pragma mark - CFAllocator Callbacks

static CFStringRef LSQLocklessAllocatorCopyDescription(const void *info)
{
    CFStringRef format = CFStringCreateWithCString(NULL, "LSQLocklessAllocator <%p>", kCFStringEncodingUTF8);
    CFStringRef ret    = CFStringCreateWithFormat(NULL, NULL, format, info);
    CFRelease(format);
    return ret;
}

//static const void *	LSQLocklessAllocatorRetain(const void *info)
//{
//    return info;
//}
//
//static void LSQLocklessAllocatorRelease(const void *info)
//{
//}

static void * LSQLocklessAllocatorAllocate(CFIndex allocSize, CFOptionFlags hint, void *info)
{
    malloc_zone_t *zone = (malloc_zone_t *)info;
    return malloc_zone_malloc(zone, allocSize);
}

static void * LSQLocklessAllocatorReallocate(void *ptr, CFIndex newsize, CFOptionFlags hint, void *info)
{
    malloc_zone_t *zone = (malloc_zone_t *)info;
    return malloc_zone_realloc(zone, ptr, newsize);
}

static void LSQLocklessAllocatorDeallocate(void *ptr, void *info)
{
    malloc_zone_t *zone = (malloc_zone_t *)info;
    malloc_zone_free(zone, ptr);
}

static CFIndex LSQLocklessAllocatorPreferredSize(CFIndex size, CFOptionFlags hint, void *info)
{
    malloc_zone_t *zone = (malloc_zone_t *)info;
    return zone->introspect->good_size(zone, size);
}

// This function return pointer to lockless allocator
CFAllocatorRef NewLSQLocklessAllocator()
{
    #ifdef NEDMALLOC_H
        neddisablethreadcache(0);
        // Create new malloc zone
        malloc_zone_t *allocatorZone = malloc_ned_zone();
    #else
        malloc_zone_t *allocatorZone = malloc_default_zone();
    #endif
    // Create allocator context
    CFAllocatorContext context =
    {
        0,
        (void*)allocatorZone,
        NULL,
        NULL,
        LSQLocklessAllocatorCopyDescription,
        LSQLocklessAllocatorAllocate,
        LSQLocklessAllocatorReallocate,
        LSQLocklessAllocatorDeallocate,
        LSQLocklessAllocatorPreferredSize
    };
    context.info = allocatorZone;
    // Create allocator
    return CFAllocatorCreate(kCFAllocatorUseContext, &context);
}

struct task_basic_info LSQAllocatorGetMemoryInfo()
{
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    task_info(mach_task_self(),
              TASK_BASIC_INFO,
              (task_info_t)&info,
              &size);
    return info;
}

//________________________________________________________________________________________

#pragma mark - CFAllocator wrapper

// Set LSQLocklessAllocator to Defaul Allocator
__attribute__((constructor)) static void LSQAllocator(void)
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        kLSQLocklessAllocator = NewLSQLocklessAllocator();
    });
}

