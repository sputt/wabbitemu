//
//  WCMutableRangeArray.m
//  WabbitStudio
//
//  Created by William Towe on 4/6/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCMutableRangeArray.h"


@implementation WCMutableRangeArray

- (id)init {
    if (!(self = [super init]))
		return nil;
	
	_count = 0;
	_data = [[NSMutableData alloc] initWithCapacity:0];
    
    return self;
}

- (void)dealloc {
	[_data release];
    [super dealloc];
}

- (NSUInteger)count {
	return _count;
}
- (id)objectAtIndex:(NSUInteger)index {
	return [NSValue valueWithRange:[self rangeAtIndex:index]];
}
- (void)insertObject:(id)object atIndex:(NSUInteger)index {
	[self insertRange:[(NSValue *)object rangeValue] atIndex:index];
}
- (void)removeAllObjects {
	[self removeAllRanges];
}

- (void)addRange:(NSRange)range; {
	[self insertRange:range atIndex:[self count]];
}
- (NSRange)rangeAtIndex:(NSUInteger)index; {
	NSRangePointer range = &((NSRangePointer)[_data bytes])[index];
	
	return *range;
}
- (void)insertRange:(NSRange)range atIndex:(NSUInteger)index; {
	[_data appendBytes:&range length:sizeof(range)];
	_count++;
}
- (void)removeAllRanges; {
	[_data setLength:0];
	_count = 0;
}

- (NSArray *)allRangesAsObjects; {
	NSMutableArray *retval = [NSMutableArray arrayWithCapacity:[self count]];
	
	for (NSUInteger index = 0; index < [self count]; index++) {
		[retval addObject:[self objectAtIndex:index]];
	}
	
	return [[retval copy] autorelease];
}
@end
