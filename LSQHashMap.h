//
//  LSQHashMap.h
//  TheMightyMIDISequencer
//
//  Created by Павел Литвиненко on 20.03.14.
//  Copyright 2014 Casual Underground. All rights reserved.
//

#ifndef LSQDataStructures_LSQHashMap_h
#define LSQDataStructures_LSQHashMap_h

#import <CoreFoundation/CoreFoundation.h>
#import "LSQBase.h"

//________________________________________________________________________________________

CF_EXTERN_C_BEGIN

//________________________________________________________________________________________

typedef struct LSQHashMap *LSQHashMapRef;

//________________________________________________________________________________________

#pragma mark - Data Structures

typedef struct LSQHashMap_Data
{
} LSQHashMap_Data;

//________________________________________________________________________________________

#pragma mark - Functions

CF_EXPORT LSQHashMapRef NewLSQHashMap (); // Constructor
CF_EXPORT LSQHashMapRef LSQHashMapRetain (LSQHashMapRef); // Retain
CF_EXPORT void LSQHashMapRelease (LSQHashMapRef); // Release
CF_EXPORT void LSQHashMapDealloc (LSQHashMapRef); // Free memory

//________________________________________________________________________________________

CF_EXTERN_C_END

//________________________________________________________________________________________

#endif
