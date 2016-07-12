//
//  LSQCommon.h
//  LSQWheelView
//
//  Created by Павел Литвиненко on 13.03.14.
//  Copyright (c) 2014 Casual Underground. All rights reserved.
//

#ifndef LSQWheelView_LSQCommon_h
#define LSQWheelView_LSQCommon_h

#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <libkern/OSAtomic.h>
#include <math.h>
#include "MacTypes.h"

//________________________________________________________________________________________

#pragma mark - Macro and Constants

#define WAIT(time) ({\
uint64_t _time = time + mach_absolute_time();\
while (mach_absolute_time() < _time) { mach_wait_until(_time); }\
})

#define ATOMICSWAP_PTR(old, new) ({\
bool __success = false;\
while (!__success) { __success = OSAtomicCompareAndSwapPtr((void*)old, (void*)new, (void* volatile*)&old); }\
__success;\
})

#define ATOMICSWAP_INT(old, new) ({\
bool __success = false;\
while (!__success) { __success = OSAtomicCompareAndSwapInt(old, new, (volatile int*)&old); }\
__success;\
})

#define ATOMICSWAP_LONG(old, new) ({\
bool __success = false;\
while (!__success) { __success = OSAtomicCompareAndSwapLong(old, new, (volatile long*)&old); }\
__success;\
})

#define ATOMICSWAP_FLOAT(old, new) ({\
bool __success = false;\
while (!__success) { __success = OSAtomicCompareAndSwap32(old, new, (volatile int32_t*)&old); }\
__success;\
})

#define ATOMICSWAP_INT32(old, new) ({\
bool __success = false;\
while (!__success) { __success = OSAtomicCompareAndSwap32(old, new, (volatile int32_t*)&old); }\
__success;\
})

#define ATOMICINCREMENT_INT32(val) ({\
bool __success = false;\
while (!__success) { __success = OSAtomicIncrement32((volatile int32_t*)&val) >= 0; }\
__success;\
})

#define ATOMICDECRIMENT_INT32(val) ({\
bool __success = false;\
while (!__success) { __success = OSAtomicDecrement32((volatile int32_t*)&val) || val == 0; }\
__success;\
})

//________________________________________________________________________________________

#endif
