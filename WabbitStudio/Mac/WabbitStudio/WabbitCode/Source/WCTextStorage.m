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
	[self cleanupUserDefaultsObserving];
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
	
	[self _calculateLineStartIndexes];
	
	[self edited:NSTextStorageEditedCharacters range:range changeInLength:[str length] - range.length];
}

- (void)setAttributes:(NSDictionary *)attrs range:(NSRange)range {
	[_contents setAttributes:attrs range:range];
	[self edited:NSTextStorageEditedAttributes range:range changeInLength:0];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	if ([(NSString *)context isEqualToString:kWCPreferencesEditorFontKey])
		[self setAttributes:[NSDictionary dictionaryWithObjectsAndKeys:[[NSUserDefaults standardUserDefaults] fontForKey:kWCPreferencesEditorFontKey],NSFontAttributeName, nil] range:NSMakeRange(0, [[self string] length])];
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (NSArray *)userDefaultsKeys {
	return [NSArray arrayWithObjects:kWCPreferencesEditorFontKey, nil];
}

- (NSArray *)lineStartIndexes; {
	return _lineStartIndexes;
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

- (void)_privateInit; {
	_lineStartIndexes = [[NSMutableArray alloc] init];
	
	[self setupUserDefaultsObserving];
	
	[self _calculateLineStartIndexes];
}
@end
