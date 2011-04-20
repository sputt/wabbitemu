//
//  WCTextStorage.m
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTextStorage.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "NSObject+WCExtensions.h"


@interface WCTextStorage (Private)
- (void)_privateInit;
- (void)_calculateLineStartIndexes;
- (void)_calculateLineStartIndexesFromLineNumber:(NSUInteger)lineNumber;
@end

@implementation WCTextStorage

- (id)init {
	if (!(self = [super init]))
		return nil;
	
	_contents = [[NSMutableAttributedString alloc] init];
	
	[self _privateInit];
	
	return self;
}

- (id)initWithString:(NSString *)string {
	if (!(self = [super init]))
		return nil;
		
	_contents = [[NSMutableAttributedString alloc] initWithString:string];
	
	[self _privateInit];
	
	return self;
}

- (id)initWithString:(NSString *)string attributes:(NSDictionary *)attributes {
	if (!(self = [super init]))
		return nil;
		
	_contents = [[NSMutableAttributedString alloc] initWithString:string attributes:attributes];
	
	[self _privateInit];
	
	return self;
}

- (id)initWithAttributedString:(NSAttributedString *)string {
	if (!(self = [super init]))
		return nil;
	
	_contents = [string mutableCopy];
	
	[self _privateInit];
	
	return self;
}

- (void)dealloc {
	/*
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	 */
	//[self cleanupUserDefaultsObserving];
	[_contents release];
	[_lineStartIndexes release];
    [super dealloc];
}

- (NSString *)string {
	return [_contents string];
}

- (NSDictionary *)attributesAtIndex:(NSUInteger)location effectiveRange:(NSRangePointer)range {
	return [_contents attributesAtIndex:location effectiveRange:range];
}

- (void)replaceCharactersInRange:(NSRange)range withString:(NSString *)str {
	[_contents replaceCharactersInRange:range withString:str];
	
	// only calculate line indexes that could have changed; i.e. lines after the line that was edited
	[self _calculateLineStartIndexesFromLineNumber:[self lineNumberForCharacterIndex:range.location]];
	//[self _calculateLineStartIndexes];
	
	[self edited:NSTextStorageEditedCharacters range:range changeInLength:[str length] - range.length];
}

- (void)setAttributes:(NSDictionary *)attrs range:(NSRange)range {
	[_contents setAttributes:attrs range:range];
	[self edited:NSTextStorageEditedAttributes range:range changeInLength:0];
}

- (NSArray *)lineStartIndexes; {
	return [[_lineStartIndexes copy] autorelease];
}

- (NSUInteger)numberOfLines; {
	return [_lineStartIndexes count];
}

- (NSUInteger)lineNumberForCharacterIndex:(NSUInteger)index; {
	NSUInteger left = 0, right = [_lineStartIndexes count], mid, lineStart;
	
	while ((right - left) > 1) {
		mid = (right + left) / 2;
		lineStart = [[_lineStartIndexes objectAtIndex:mid] unsignedIntegerValue];
		
		if (index < lineStart)
			right = mid;
		else if (index > lineStart)
			left = mid;
		else
			return mid;
	}
	return left;
}

- (NSUInteger)lineStartIndexForLineNumber:(NSUInteger)line; {
	if (line >= [_lineStartIndexes count])
		line = [_lineStartIndexes count] - 1;
	return [[_lineStartIndexes objectAtIndex:line] unsignedIntegerValue];
}

- (NSUInteger)lineStartIndexForCharacterIndex:(NSUInteger)index; {
	return [[_lineStartIndexes objectAtIndex:[self lineNumberForCharacterIndex:index]] unsignedIntegerValue];
}

- (void)_calculateLineStartIndexes; {
	//[self _calculateLineStartIndexesStartingAtLine:0];
	
	[_lineStartIndexes removeAllObjects];
	
	NSString *string = [self string];
	NSUInteger length = [string length], index = 0;
	
	do {
		
		[_lineStartIndexes addObject:[NSNumber numberWithUnsignedInteger:index]];
		index = NSMaxRange([string lineRangeForRange:NSMakeRange(index, 0)]);
		
	} while (index < length);
	
	// check to see if the string ends with a newline
	NSUInteger lineEnd = 0, contentEnd = 0;
	
	[string getLineStart:NULL end:&lineEnd contentsEnd:&contentEnd forRange:NSMakeRange([[_lineStartIndexes lastObject] unsignedIntegerValue], 0)];
	
	if (contentEnd < lineEnd)
		[_lineStartIndexes addObject:[NSNumber numberWithUnsignedInteger:index]];
}

- (void)_calculateLineStartIndexesFromLineNumber:(NSUInteger)lineNumber; {
	[_lineStartIndexes removeObjectsAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(lineNumber, [_lineStartIndexes count] - lineNumber)]];
	
	NSString *string = [self string];
	NSUInteger length = [string length], index = 0;
	
	if ([_lineStartIndexes count] != 0)
		index = NSMaxRange([string lineRangeForRange:NSMakeRange([[_lineStartIndexes objectAtIndex:lineNumber - 1] unsignedIntegerValue], 0)]);
	
	do {
		
		[_lineStartIndexes addObject:[NSNumber numberWithUnsignedInteger:index]];
		index = NSMaxRange([string lineRangeForRange:NSMakeRange(index, 0)]);
		
	} while (index < length);
	
	// check to see if the string ends with a newline
	NSUInteger lineEnd = 0, contentEnd = 0;
	
	[string getLineStart:NULL end:&lineEnd contentsEnd:&contentEnd forRange:NSMakeRange([[_lineStartIndexes lastObject] unsignedIntegerValue], 0)];
	
	if (contentEnd < lineEnd)
		[_lineStartIndexes addObject:[NSNumber numberWithUnsignedInteger:index]];
}

- (void)_privateInit; {
	_lineStartIndexes = [[NSMutableArray alloc] init];
	
	//[self setupUserDefaultsObserving];
	
	[self _calculateLineStartIndexes];
}
@end
