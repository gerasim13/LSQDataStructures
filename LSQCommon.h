//
//  LSQCommon.h
//  LSQWheelView
//
//  Created by Павел Литвиненко on 13.03.14.
//  Copyright (c) 2014 Casual Underground. All rights reserved.
//

#ifndef LSQWheelView_LSQCommon_h
#define LSQWheelView_LSQCommon_h

#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <libkern/OSAtomic.h>
#import <math.h>
#import "MacTypes.h"

//________________________________________________________________________________________

#pragma mark - Macro and Constants

#define M_2PI 6.28318530717958647692528676655900576 /* pi*2 */

#define CLAMP(A, B, C) ({\
if (C > B) { C = B; }\
if (C < A) { C = A; }\
C;})

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

#pragma mark - C Functions

inline static CGColorRef CGColorWithRGBA(CGFloat r, CGFloat g, CGFloat b, CGFloat a)
{
    const CGFloat components[4] = {r, g, b, a};
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGColorRef color = CGColorCreate(colorSpace, components);
    CGColorSpaceRelease(colorSpace);
    return color;
}

inline static Float64 DegreesToRadians(CGFloat degrees)
{
    return (degrees * M_PI) / 180.0;
}

inline static Float64 RadiansToDegrees(CGFloat radians)
{
    return (radians * 180.0) / M_PI;
}

inline static Float32 ClampFloat32(Float32 min, Float32 max, Float32 value)
{
    return CLAMP(min, max, value);
}

inline static Float64 ClampFloat64(Float64 min, Float64 max, Float64 value)
{
    return CLAMP(min, max, value);
}

inline static SInt32 ClampInt(SInt32 min, SInt32 max, SInt32 value)
{
    return CLAMP(min, max, value);
}

inline static CFIndex ClampIndex(CFIndex min, CFIndex max, CFIndex value)
{
    return CLAMP(min, max, value);
}

inline static CGSize CGSizeAdd(CGSize size1, CGSize size2)
{
    size1.width  += size2.width;
    size1.height += size2.height;
    return size1;
}

inline static CGSize CGSizeSubtract(CGSize size1, CGSize size2)
{
    size1.width  -= size2.width;
    size1.height -= size2.height;
    return size1;
}

inline static CGSize CGSizeMultiply(CGSize size, CGFloat mult)
{
    size.width  *= mult;
    size.height *= mult;
    return size;
}

inline static CGFloat CGSizeGetHalfWidth(CGSize size)
{
    return size.width / 2.0;
}

inline static CGFloat CGSizeGetHalfHeight(CGSize size)
{
    return size.height / 2.0;
}

inline static CGPoint CGPointGetLandscapePoint(CGPoint point)
{
    return CGPointMake(point.y, point.x);
}

inline static CGSize CGSizeGetLandscapeSize(CGSize size)
{
    return CGSizeMake(size.height, size.width);
}

inline static CGRect CGRectGetLandscapeRect(CGRect rect)
{
    return CGRectMake(rect.origin.y, rect.origin.x, rect.size.height, rect.size.width);
}

inline static CGRect CGRectSliceFromTop(CGRect rect, CGFloat amount)
{
    CGRect zero;
    CGRectDivide(rect, &zero, &rect, amount, CGRectMinYEdge);
    return rect;
}

inline static CGRect CGRectSliceFromBottom(CGRect rect, CGFloat amount)
{
    CGRect zero;
    CGRectDivide(rect, &zero, &rect, amount, CGRectMaxYEdge);
    return rect;
}

inline static CGRect CGRectSliceFromLeft(CGRect rect, CGFloat amount)
{
    CGRect zero;
    CGRectDivide(rect, &zero, &rect, amount, CGRectMinXEdge);
    return rect;
}

inline static CGRect CGRectSliceFromRight(CGRect rect, CGFloat amount)
{
    CGRect zero;
    CGRectDivide(rect, &zero, &rect, amount, CGRectMaxXEdge);
    return rect;
}

inline static CFHashCode CGRectGetHash(CGRect rect)
{
    return (*(CFIndex*)&rect.origin.x << 10 ^ *(CFIndex*)&rect.origin.y) + (*(CFIndex*)&rect.size.width << 10 ^ *(CFIndex*)&rect.size.height);
}

#endif
