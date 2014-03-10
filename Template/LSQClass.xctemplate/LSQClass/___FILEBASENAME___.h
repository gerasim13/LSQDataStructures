//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//  Copyright ___YEAR___ ___ORGANIZATIONNAME___. All rights reserved.
//

#ifndef ___PROJECTNAME______FILEBASENAMEASIDENTIFIER____h
#define ___PROJECTNAME______FILEBASENAMEASIDENTIFIER____h

#import <CoreFoundation/CoreFoundation.h>
#import "LSQBase.h"

//________________________________________________________________________________________

CF_EXTERN_C_BEGIN

//________________________________________________________________________________________

typedef struct ___FILEBASENAMEASIDENTIFIER___ *___FILEBASENAMEASIDENTIFIER___Ref;

//________________________________________________________________________________________

#pragma mark - Data Structures

typedef struct ___FILEBASENAMEASIDENTIFIER____Data
{
} ___FILEBASENAMEASIDENTIFIER____Data;

//________________________________________________________________________________________

#pragma mark - Functions

CF_EXPORT ___FILEBASENAMEASIDENTIFIER___Ref New___FILEBASENAMEASIDENTIFIER___ (); // Constructor
CF_EXPORT ___FILEBASENAMEASIDENTIFIER___Ref ___FILEBASENAMEASIDENTIFIER___Retain (___FILEBASENAMEASIDENTIFIER___Ref); // Retain
CF_EXPORT void ___FILEBASENAMEASIDENTIFIER___Release (___FILEBASENAMEASIDENTIFIER___Ref); // Release
CF_EXPORT void ___FILEBASENAMEASIDENTIFIER___Dealloc (___FILEBASENAMEASIDENTIFIER___Ref); // Free memory

//________________________________________________________________________________________

CF_EXTERN_C_END

//________________________________________________________________________________________

#endif
