//
//  NSIndexPath+WCExtensions.m
//  files
//
//  Created by William Towe on 5/4/09.
//  Copyright 2009 Revolution Software. All rights reserved.
//

#import "NSIndexPath+WCExtensions.h"


@implementation NSIndexPath (WCExtensions)
// returns the last index of self
- (NSUInteger)lastIndex; {
	return [self indexAtPosition:[self length] - 1];
}
@end
