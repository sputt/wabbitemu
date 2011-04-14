//
//  NSArray+WCExtensions.h
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSArray.h>

@class WCSymbol,WCFile,WCAlias;

@interface NSArray (NSArray_WCExtensions)
- (id)firstObject;
- (NSUInteger)symbolIndexForLocation:(NSUInteger)location;
- (NSUInteger)previousRangeIndexForCharacterIndex:(NSUInteger)index;
- (NSUInteger)nextRangeIndexForCharacterIndex:(NSUInteger)index;
- (WCFile *)fileMatchingAlias:(WCAlias *)alias;
@end
