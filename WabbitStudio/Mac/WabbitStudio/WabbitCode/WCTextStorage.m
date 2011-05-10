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
#import "WCDefines.h"


@interface WCTextStorage (Private)
+ (NSFont *)_defaultFont;
+ (NSParagraphStyle *)_defaultParagraphStyle;
+ (NSColor *)_defaultTextColor;
+ (NSColor *)_defaultBackgroundColor;

- (void)_privateInit;
- (void)_calculateLineStartIndexes;
- (void)_calculateLineStartIndexesFromLineNumber:(NSUInteger)lineNumber;
@end

@implementation WCTextStorage

- (id)init {
	if (!(self = [super init]))
		return nil;
	
	_contents = [[NSMutableAttributedString alloc] initWithString:@"" attributes:[[self class] defaultAttributes]];
	
	[self _privateInit];
	
	return self;
}

- (id)initWithString:(NSString *)string {
	if (!(self = [super init]))
		return nil;
		
	_contents = [[NSMutableAttributedString alloc] initWithString:string attributes:[[self class] defaultAttributes]];
	
	[self _privateInit];
	
	return self;
}

- (id)initWithString:(NSString *)string attributes:(NSDictionary *)attributes {
	if (!(self = [super init]))
		return nil;
		
	_contents = [[NSMutableAttributedString alloc] initWithString:string attributes:[[self class] defaultAttributes]];
	
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
	
	// only calculate line indexes that could have changed; i.e. lines after the line that was edited
	[self _calculateLineStartIndexesFromLineNumber:[self lineNumberForCharacterIndex:range.location]];
	
	[self edited:NSTextStorageEditedCharacters range:range changeInLength:[str length] - range.length];
}

- (void)setAttributes:(NSDictionary *)attrs range:(NSRange)range {
	// ensure our attributes don't get changed somehow (e.g. by any text views that we back)
	[_contents setAttributes:[[self class] defaultAttributes] range:range];
	[self edited:NSTextStorageEditedAttributes range:range changeInLength:0];
}

- (NSArray *)userDefaultsKeys {
	return [NSArray arrayWithObjects:kWCPreferencesEditorFontKey,kWCPreferencesEditorTabWidthKey,kWCPreferencesEditorTextColorKey,kWCPreferencesEditorBackgroundColorKey, nil];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	if ([(NSString *)context isEqualToString:kWCPreferencesEditorFontKey])
		[self setAttributes:[[self class] defaultAttributes] range:NSMakeRange(0, [self length])];
	else if ([(NSString *)context isEqualToString:kWCPreferencesEditorTabWidthKey])
		[self setAttributes:[[self class] defaultAttributes] range:NSMakeRange(0, [self length])];
	else if ([(NSString *)context isEqualToString:kWCPreferencesEditorTextColorKey])
		[self setAttributes:[[self class] defaultAttributes] range:NSMakeRange(0, [self length])];
	else if ([(NSString *)context isEqualToString:kWCPreferencesEditorBackgroundColorKey])
		[self setAttributes:[[self class] defaultAttributes] range:NSMakeRange(0, [self length])];
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (NSArray *)lineStartIndexes; {
	return [[_lineStartIndexes copy] autorelease];
}

- (NSUInteger)numberOfLines; {
	return [_lineStartIndexes count];
}

- (NSUInteger)lineNumberForCharacterIndex:(NSUInteger)characterIndex; {
	NSUInteger left = 0, right = [_lineStartIndexes count], mid, lineStart;
	
	while ((right - left) > 1) {
		mid = (right + left) / 2;
		lineStart = [[_lineStartIndexes objectAtIndex:mid] unsignedIntegerValue];
		
		if (characterIndex < lineStart)
			right = mid;
		else if (characterIndex > lineStart)
			left = mid;
		else
			return mid;
	}
	return left;
}

- (NSUInteger)lineStartIndexForLineNumber:(NSUInteger)lineNumber; {
	return [[_lineStartIndexes objectAtIndex:lineNumber] unsignedIntegerValue];
}

- (NSUInteger)lineStartIndexForCharacterIndex:(NSUInteger)characterIndex; {
	return [[_lineStartIndexes objectAtIndex:[self lineNumberForCharacterIndex:characterIndex]] unsignedIntegerValue];
}

- (NSUInteger)safeLineStartIndexForLineNumber:(NSUInteger)lineNumber; {
	if ([self numberOfLines] == 0)
		return 0;
	else if (lineNumber >= [self numberOfLines])
		return [self numberOfLines] - 1;
	return [self lineStartIndexForLineNumber:lineNumber];
}

- (NSRange)safeRangeForCharacterRange:(NSRange)characterRange; {
	if ([self numberOfLines] == 0)
		return WCEmptyRange;
	else if (NSMaxRange(characterRange) >= [[self string] length])
		return WCEmptyRange;
	return characterRange;
}

- (NSUInteger)safeLineNumberForCharacterIndex:(NSUInteger)characterIndex; {
	if ([self numberOfLines] == 0)
		return 0;
	else if (characterIndex >= [[self string] length])
		return [self numberOfLines] - 1;
	return [self lineNumberForCharacterIndex:characterIndex];
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

+ (NSDictionary *)defaultAttributes; {
	return [NSDictionary dictionaryWithObjectsAndKeys:[self _defaultFont],NSFontAttributeName,[self _defaultParagraphStyle],NSParagraphStyleAttributeName,[self _defaultTextColor],NSForegroundColorAttributeName, nil];
}

+ (NSParagraphStyle *)_defaultParagraphStyle; {
	// Set the width of every tab by first checking the size of the tab in spaces in the current font and then remove all tabs that sets automatically and then set the default tab stop distance
	NSMutableString *sizeString = [NSMutableString string];
	NSInteger numberOfSpaces = [[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesEditorTabWidthKey];
	while (numberOfSpaces--)
		[sizeString appendString:@" "];
	
	NSDictionary *sizeAttribute = [NSDictionary dictionaryWithObjectsAndKeys:[[NSUserDefaults standardUserDefaults] fontForKey:kWCPreferencesEditorFontKey], NSFontAttributeName, nil];
	CGFloat sizeOfTab = [sizeString sizeWithAttributes:sizeAttribute].width;
	
	NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
	
	for (id item in [style tabStops])
		[style removeTabStop:item];
	
	[style setDefaultTabInterval:sizeOfTab];
	
	return [[style copy] autorelease];
}

+ (NSFont *)_defaultFont {
	return [[NSUserDefaults standardUserDefaults] fontForKey:kWCPreferencesEditorFontKey];
}

+ (NSColor *)_defaultTextColor; {
	return [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEditorTextColorKey];
}
+ (NSColor *)_defaultBackgroundColor; {
	return [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEditorBackgroundColorKey];
}

- (void)_privateInit; {
	_lineStartIndexes = [[NSMutableArray alloc] init];
	
	[self setupUserDefaultsObserving];
	
	[self _calculateLineStartIndexes];
}
@end
