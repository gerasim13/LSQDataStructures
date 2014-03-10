//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//  Copyright ___YEAR___ ___ORGANIZATIONNAME___. All rights reserved.
//

#import "___FILEBASENAME___.h"
#import "___VARIABLE_LSQClassSubclass___.h"

//________________________________________________________________________________________

#pragma mark - Data Structures

typedef struct ___FILEBASENAMEASIDENTIFIER___
{
    LSQBaseTypeRef base;
    ___VARIABLE_LSQClassSubclass___Ref super;
    struct ___FILEBASENAMEASIDENTIFIER____Data data;
} ___FILEBASENAMEASIDENTIFIER___;

//________________________________________________________________________________________

#pragma mark - Base Functions

___FILEBASENAMEASIDENTIFIER___Ref New___FILEBASENAMEASIDENTIFIER___ ()
{
    ___FILEBASENAMEASIDENTIFIER___Ref self = LSQALLOCK(___FILEBASENAMEASIDENTIFIER___, NULL, (void*)&___FILEBASENAMEASIDENTIFIER___Dealloc);
    self->super = New___VARIABLE_LSQClassSubclass___(/* constructor parameters */);
    return self;
}

___FILEBASENAMEASIDENTIFIER___Ref ___FILEBASENAMEASIDENTIFIER___Retain (___FILEBASENAMEASIDENTIFIER___Ref self)
{
    self->base = LSQBaseRetain(self->base);
    return self;
}

void ___FILEBASENAMEASIDENTIFIER___Release (___FILEBASENAMEASIDENTIFIER___Ref self)
{
    LSQBaseRelease(self->base);
}

void ___FILEBASENAMEASIDENTIFIER___Dealloc (___FILEBASENAMEASIDENTIFIER___Ref self)
{
    ___VARIABLE_LSQClassSubclass___Release(self->super);
    LSQAllocatorDealloc(self);
}