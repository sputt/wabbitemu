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
- (WCSymbol *)functionSymbolForLocation:(NSUInteger)location indexOfSymbol:(NSUInteger *)indexOfSymbol;
- (WCFile *)fileMatchingAlias:(WCAlias *)alias;
@end
