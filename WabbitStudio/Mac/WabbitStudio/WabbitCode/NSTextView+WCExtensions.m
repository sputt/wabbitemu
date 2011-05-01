//
//  NSTextView+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 4/6/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSTextView+WCExtensions.h"


@implementation NSTextView (NSTextView_WCExtensions)
// convenience method to replace each range with the same string
- (NSArray *)replaceCharactersInRanges:(NSArray *)replacementRanges withString:(NSString *)string; {
#ifdef DEBUG
	NSAssert(replacementRanges != nil, @"replacementRanges cannot be nil!");
	NSAssert(string != nil, @"string cannot be nil!");
	NSAssert([replacementRanges count] > 0, @"replaceRanges must have count > 0!");
#endif
	NSUInteger count = [replacementRanges count];
	NSString *objects[count];
	
	for (NSUInteger index = 0; index < count; index++)
		objects[index] = string;
	
	return [self replaceCharactersInRanges:replacementRanges withStrings:[NSArray arrayWithObjects:objects count:count]];
}
// replaces the strings at replacementRanges with strings, returns the ranges of the new strings
// returns empty array if no replacements were performed
- (NSArray *)replaceCharactersInRanges:(NSArray *)replacementRanges withStrings:(NSArray *)strings; {
#ifdef DEBUG
	NSAssert(replacementRanges != nil, @"replacementRanges cannot be nil!");
	NSAssert(strings != nil, @"strings cannot be nil!");
	NSAssert([replacementRanges count] > 0, @"replaceRanges must have count > 0!");
	NSAssert([replacementRanges count] == [strings count], @"replacementRanges and stringRanges must have the same count!");
#endif

	NSMutableArray *retval = [NSMutableArray arrayWithCapacity:[replacementRanges count]];
	NSTextStorage *textStorage = [self textStorage];
	NSInteger locationShift = 0;
	
	if ([self shouldChangeTextInRanges:replacementRanges replacementStrings:strings]) {
		[textStorage beginEditing];
		
		NSUInteger stringsIndex = 0;
		for (NSValue *rValue in replacementRanges) {
			NSRange range = [rValue rangeValue];
			NSRange sRange = NSMakeRange(range.location + locationShift, range.length);
			NSString *string = [strings objectAtIndex:stringsIndex++];
			
			[textStorage replaceCharactersInRange:sRange withString:string];
			
			locationShift += [string length] - range.length;
			
			[retval addObject:[NSValue valueWithRange:NSMakeRange(sRange.location, [string length])]];
		}
		
		[textStorage endEditing];
	}
	return [[retval copy] autorelease];
}

- (void)setSelectedRangeSafely:(NSRange)range; {
	[self setSelectedRangeSafely:range scrollRangeToVisible:NO];
}

- (void)setSelectedRangeSafely:(NSRange)range scrollRangeToVisible:(BOOL)scrollRangeToVisible; {
	if (NSMaxRange(range) >= [[self string] length])
		range = NSMakeRange(0, 0);
	
	[self setSelectedRange:range];
	
	if (scrollRangeToVisible)
		[self scrollRangeToVisible:range];
}

- (void)setSelectedLineNumber:(NSUInteger)lineNumber; {
	[self setSelectedLineNumber:lineNumber scrollRangeToVisible:NO];
}
- (void)setSelectedLineNumber:(NSUInteger)lineNumber scrollRangeToVisible:(BOOL)scrollRangeToVisible; {
	NSString *string = [self string];
	NSUInteger numberOfLines, index, stringLength = [string length];
	for (index = 0, numberOfLines = 0; index < stringLength; numberOfLines++) {
		if (numberOfLines == lineNumber) {
			[self setSelectedRangeSafely:NSMakeRange(index, 0) scrollRangeToVisible:YES];
			return;
		}
		index = NSMaxRange([string lineRangeForRange:NSMakeRange(index, 0)]);
	}
}
@end
