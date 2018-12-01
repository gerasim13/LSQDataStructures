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
#include <math.h>
#include "MacTypes.h"

//________________________________________________________________________________________

#pragma mark - Macro and Constants

#define WAIT(time) ({\
uint64_t _time = time + mach_absolute_time();\
while (mach_absolute_time() < _time) { mach_wait_until(_time); }\
})

//________________________________________________________________________________________

#endif
