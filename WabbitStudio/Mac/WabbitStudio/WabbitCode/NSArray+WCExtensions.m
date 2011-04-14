//
//  NSArray+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSArray+WCExtensions.h"
#import "WCSymbol.h"
#import "WCAlias.h"
#import "WCFile.h"
#import "NSURL+WCExtensions.h"

@implementation NSArray (NSArray_WCExtensions)
- (id)firstObject {
	if ([self count] == 0)
		return nil;
	return [self objectAtIndex:0];
}
// binary search; assumes the array is sorted
- (NSUInteger)symbolIndexForLocation:(NSUInteger)location; {
	NSUInteger left = 0, right = [self count], mid, lineStart;
	
	while ((right - left) > 1) {
		mid = (right + left) / 2;
		lineStart = [[self objectAtIndex:mid] symbolRange].location;
		
		if (location < lineStart)
			right = mid;
		else if (location > lineStart)
			left = mid;
		else
			return mid;
	}
	return left;
}
// binary search; assumes the array is sorted
- (NSUInteger)previousRangeIndexForCharacterIndex:(NSUInteger)index; {
	NSUInteger left = 0, right = [self count], mid, lineStart;
	
	while ((right - left) > 1) {
		mid = (right + left) / 2;
		lineStart = [[self objectAtIndex:mid] rangeValue].location;
		
		if (index < lineStart)
			right = mid;
		else if (index > lineStart)
			left = mid;
		else
			return mid;
	}
	return left;
}
// binary search; assumes the array is sorted
- (NSUInteger)nextRangeIndexForCharacterIndex:(NSUInteger)index; {
	NSUInteger left = 0, right = [self count], mid, lineStart;
	
	while ((right - left) > 1) {
		mid = (right + left) / 2;
		lineStart = [[self objectAtIndex:mid] rangeValue].location;
		
		if (index < lineStart)
			right = mid;
		else if (index > lineStart)
			left = mid;
		else
			return (++mid < [self count])?mid:--mid;
	}
	return (++left < [self count])?left:--left;
}

- (WCFile *)fileMatchingAlias:(WCAlias *)alias; {
	WCFile *mFile = nil;
	for (WCFile *file in self) {
		if ([[file URL] isEqualToFileURL:[alias URL]]) {
			mFile = file;
			break;
		}
	}
	return mFile;
}
@end
