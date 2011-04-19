//
//  WCSyntaxHighlighter.m
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSyntaxHighlighter.h"
#import "WCTextView.h"
#import "WCTextStorage.h"
#import "WCSymbolScanner.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "WCFile.h"
#import "WCProject.h"
#import "WCSymbol.h"
#import "NSObject+WCExtensions.h"
#import "WCBuildTarget.h"

#define restrict
#import <RegexKit/RegexKit.h>

RKRegex *kWCSyntaxHighlighterDirectivesRegex = nil;
RKRegex *kWCSyntaxHighlighterOpCodesRegex = nil;
RKRegex *kWCSyntaxHighlighterConditionalsRegex = nil;
RKRegex *kWCSyntaxHighlighterRegistersRegex = nil;
RKRegex *kWCSyntaxHighlighterPreOpsRegex = nil;
RKRegex *kWCSyntaxHighlighterStringsRegex = nil;
RKRegex *kWCSyntaxHighlighterNumbersRegex = nil;
RKRegex *kWCSyntaxHighlighterHexadeicmalsRegex = nil;
RKRegex *kWCSyntaxHighlighterBinariesRegex = nil;
RKRegex *kWCSyntaxHighlighterCommentsRegex = nil;

RKRegex *kWCSyntaxHighlighterSymbolsRegex = nil;

@implementation WCSyntaxHighlighter

+ (void)initialize {
	if ([WCSyntaxHighlighter class] != self)
		return;
	
	kWCSyntaxHighlighterDirectivesRegex = [[RKRegex alloc] initWithRegexString:@"\\.(?:db|dw|end|org|byte|word|fill|block|addinstr|echo|error|list|nolist|equ|show|option|seek)\\b" options:RKCompileUTF8];
	kWCSyntaxHighlighterOpCodesRegex = [[RKRegex alloc] initWithRegexString:@"\\b(?:adc|add|and|bit|call|ccf|cpdr|cpd|cpir|cpi|cpl|cp|daa|dec|di|djnz|ei|exx|ex|halt|im|inc|indr|ind|inir|ini|in|jp|jr|lddr|ldd|ldir|ldi|ld|neg|nop|or|otdr|otir|outd|outi|out|pop|push|res|reti|retn|ret|rla|rlca|rlc|rld|rl|rra|rrca|rrc|rrd|rr|rst|sbc|scf|set|sla|sll|sra|srl|sub|xor)\\b" options:RKCompileUTF8];
	kWCSyntaxHighlighterConditionalsRegex = [[RKRegex alloc] initWithRegexString:@"\\b(?:nz|nv|nc|po|pe|c|p|m|n|z|v)\\b" options:RKCompileUTF8];
	kWCSyntaxHighlighterRegistersRegex = [[RKRegex alloc] initWithRegexString:@"\\b(?:ixh|iyh|ixl|iyl|sp|af|pc|bc|de|hl|ix|iy|a|f|b|c|d|e|h|l|r|i)\\b" options:RKCompileUTF8];
	kWCSyntaxHighlighterPreOpsRegex = [[RKRegex alloc] initWithRegexString:@"#\\w+" options:RKCompileUTF8];
	kWCSyntaxHighlighterStringsRegex = [[RKRegex alloc] initWithRegexString:@"\".*?\"" options:RKCompileUTF8];
	kWCSyntaxHighlighterNumbersRegex = [[RKRegex alloc] initWithRegexString:@"\\b[0-9]+\\b" options:RKCompileUTF8];
	kWCSyntaxHighlighterHexadeicmalsRegex = [[RKRegex alloc] initWithRegexString:@"(?:\\$[0-9a-fA-F]+\\b)|(?:(?<=[^$%]\\b)[0-9a-fA-F]+h)" options:RKCompileUTF8];
	kWCSyntaxHighlighterBinariesRegex = [[RKRegex alloc] initWithRegexString:@"(?:%[01]+\\b)|(?:(?<=[^$%]\\b)[01]+b)" options:RKCompileUTF8];
	kWCSyntaxHighlighterCommentsRegex = [[RKRegex alloc] initWithRegexString:@";+.*$" options:RKCompileUTF8|RKCompileMultiline];
	
	kWCSyntaxHighlighterSymbolsRegex = [[RKRegex alloc] initWithRegexString:@"[A-z0-9_!?.]+" options:RKCompileUTF8];
}

- (id)initWithTextView:(WCTextView *)textView; {
	if (!(self = [super init]))
		return nil;
	
	_textView = textView;
	
	[self setupNotificationObserving];
	
	if ([[_textView file] project])
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_textViewBoundsDidChange:) name:kWCProjectNumberOfFilesDidChangeNotification object:[[_textView file] project]];
	
	[self setupUserDefaultsObserving];
	
	[self performSyntaxHighlighting];
	
	return self;
}

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[self cleanupUserDefaultsObserving];
	[NSObject cancelPreviousPerformRequestsWithTarget:self];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	_textView = nil;
    [super dealloc];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	
	if ([(NSString *)context isEqualToString:kWCPreferencesBinariesKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesBinariesColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesConditionalsKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesConditionalsColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesCommentsKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesCommentsColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesDefinesKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesDefinesColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesDirectivesKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesDirectivesColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesEquatesKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesEquatesColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesHexadecimalsKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesHexadecimalsColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesLabelsKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesLabelsColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesMacrosKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesMacrosColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesNumbersKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesNumbersColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesOpCodesKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesOpCodesColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesPreOpsKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesPreOpsColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesRegistersKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesRegistersColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesStringsKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesStringsColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesUseSyntaxHighlightingKey])
		[self performSyntaxHighlighting];
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (NSArray *)notificationDictionaries {
	return [NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_textViewBoundsDidChange:)),kNSObjectSelectorKey,NSViewBoundsDidChangeNotification,kNSObjectNotificationNameKey,[[_textView enclosingScrollView] contentView],kNSObjectNotificationObjectKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_textViewFrameDidChange:)),kNSObjectSelectorKey,NSViewFrameDidChangeNotification,kNSObjectNotificationNameKey,[[_textView enclosingScrollView] contentView],kNSObjectNotificationObjectKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_symbolScannerFinishedScanning:)),kNSObjectSelectorKey,kWCSymbolScannerFinishedScanningNotification,kNSObjectNotificationNameKey,[[_textView file] symbolScanner],kNSObjectNotificationObjectKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_symbolScannerFinishedScanningHighlightImmediately:)),kNSObjectSelectorKey,kWCSymbolScannerFinishedScanningAfterSymbolsCaseSensitiveDidChangeNotification,kNSObjectNotificationNameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_fileTextDidChange:)),kNSObjectSelectorKey,NSTextStorageDidProcessEditingNotification,kNSObjectNotificationNameKey,[[_textView file] textStorage],kNSObjectNotificationObjectKey, nil], nil];
}

- (NSArray *)userDefaultsKeys {
	return [NSArray arrayWithObjects:kWCPreferencesBinariesKey,kWCPreferencesBinariesColorKey,kWCPreferencesConditionalsKey,kWCPreferencesConditionalsColorKey,kWCPreferencesCommentsKey,kWCPreferencesCommentsColorKey,kWCPreferencesDefinesKey,kWCPreferencesDefinesColorKey,kWCPreferencesDirectivesKey,kWCPreferencesDirectivesColorKey,kWCPreferencesEquatesKey,kWCPreferencesEquatesColorKey,kWCPreferencesHexadecimalsKey,kWCPreferencesHexadecimalsColorKey,kWCPreferencesLabelsKey,kWCPreferencesLabelsColorKey,kWCPreferencesMacrosKey,kWCPreferencesMacrosColorKey,kWCPreferencesNumbersKey,kWCPreferencesNumbersColorKey,kWCPreferencesOpCodesKey,kWCPreferencesOpCodesColorKey,kWCPreferencesPreOpsKey,kWCPreferencesPreOpsColorKey,kWCPreferencesRegistersKey,kWCPreferencesRegistersColorKey,kWCPreferencesStringsKey,kWCPreferencesStringsColorKey,kWCPreferencesUseSyntaxHighlightingKey, nil];
}

@synthesize isHighlighting=_isHighlighting;

- (void)performSyntaxHighlighting; {
	NSString *string = [_textView string];
	
	if (![string length])
		return;
	else if ([self isHighlighting])
		return;
	else if (![[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesUseSyntaxHighlightingKey]) {
		[[_textView layoutManager] removeTemporaryAttribute:NSForegroundColorAttributeName forCharacterRange:NSMakeRange(0, [[_textView string] length])];
		return;
	}
	
	[self setIsHighlighting:YES];
	
	NSRect visibleRect = [[[_textView enclosingScrollView] contentView] documentVisibleRect];
	NSRange visibleRange = [[_textView layoutManager] glyphRangeForBoundingRect:visibleRect inTextContainer:[_textView textContainer]];
	NSRange charRange = [[_textView layoutManager] characterRangeForGlyphRange:visibleRange actualGlyphRange:NULL];
	NSUInteger firstChar = [string lineRangeForRange:NSMakeRange(charRange.location, 0)].location;
	NSUInteger lastChar = NSMaxRange([string lineRangeForRange:NSMakeRange(NSMaxRange(charRange), 0)]);
	NSRange searchRange = NSMakeRange(firstChar, lastChar-firstChar);
	NSString *searchString = [string substringWithRange:searchRange];
	
	[[_textView layoutManager] removeTemporaryAttribute:NSForegroundColorAttributeName forCharacterRange:visibleRange];
	[[_textView layoutManager] removeTemporaryAttribute:NSToolTipAttributeName forCharacterRange:visibleRange];
	
	// symbols
	RKEnumerator *symbolsEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterSymbolsRegex string:searchString] autorelease];
	NSColor *labelsColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesLabelsColorKey];
	NSColor *equatesColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEquatesColorKey];
	NSColor *definesColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesDefinesColorKey];
	NSColor *macrosColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesMacrosColorKey];
	NSDictionary *labelNamesToSymbols = nil;
	NSDictionary *equateNamesToSymbols = nil;
	NSDictionary *defineNamesToSymbols = nil;
	NSDictionary *macroNamesToSymbols = nil;
	
	if ([[_textView file] project]) {
		labelNamesToSymbols = [[[_textView file] project] labelNamesToSymbols];
		equateNamesToSymbols = [[[_textView file] project] equateNamesToSymbols];
		defineNamesToSymbols = [[[_textView file] project] defineNamesToSymbols];
		macroNamesToSymbols = [[[_textView file] project] macroNamesToSymbols];
	}
	else {
		labelNamesToSymbols = [[[_textView file] symbolScanner] labelNamesToSymbols];
		equateNamesToSymbols = [[[_textView file] symbolScanner] equateNamesToSymbols];
		defineNamesToSymbols = [[[_textView file] symbolScanner] defineNamesToSymbols];
		macroNamesToSymbols = [[[_textView file] symbolScanner] macroNamesToSymbols];
	}
	
	BOOL highlightLabels = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesLabelsKey];
	BOOL highlightEquates = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEquatesKey];
	BOOL highlightDefines = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesDefinesKey];
	BOOL highlightMacros = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesMacrosKey];
	BOOL showEquateValues = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorShowEquateValueTooltipsKey];
	BOOL symbolsAreCaseSensitive = [[[[_textView file] project] activeBuildTarget] symbolsAreCaseSensitive];
	
	while ([symbolsEnum nextRanges] != NULL) {
		NSRangePointer matchRange = [symbolsEnum currentRanges];
		NSRange actualRange = NSMakeRange(searchRange.location+matchRange->location, matchRange->length);
		NSString *symbolName = (symbolsAreCaseSensitive)?[string substringWithRange:actualRange]:[[string substringWithRange:actualRange] lowercaseString];
		
		if (highlightEquates && [equateNamesToSymbols objectForKey:symbolName]) {
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:equatesColor forCharacterRange:actualRange];
			
			if (showEquateValues)
				[[_textView layoutManager] addTemporaryAttribute:NSToolTipAttributeName value:[[equateNamesToSymbols objectForKey:symbolName] symbolValue] forCharacterRange:actualRange];
		}
		else if (highlightLabels && [labelNamesToSymbols objectForKey:symbolName])
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:labelsColor forCharacterRange:actualRange];
		else if (highlightDefines && [defineNamesToSymbols objectForKey:symbolName])
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:definesColor forCharacterRange:actualRange];
		else if (highlightMacros && [macroNamesToSymbols objectForKey:symbolName])
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:macrosColor forCharacterRange:actualRange];
	}
	
	// directives
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesDirectivesKey]) {
		RKEnumerator *directivesEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterDirectivesRegex string:searchString] autorelease];
		NSColor *directivesColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesDirectivesColorKey];
		
		while ([directivesEnum nextRanges] != NULL) {
			NSRangePointer matchRange = [directivesEnum currentRanges];
			
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:directivesColor forCharacterRange:NSMakeRange(searchRange.location+matchRange->location, matchRange->length)];
		}
	}
	
	// op codes
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesOpCodesKey]) {
		RKEnumerator *opCodesEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterOpCodesRegex string:searchString] autorelease];
		NSColor *opCodesColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesOpCodesColorKey];
		
		while ([opCodesEnum nextRanges] != NULL) {
			NSRangePointer matchRange = [opCodesEnum currentRanges];
			
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:opCodesColor forCharacterRange:NSMakeRange(searchRange.location+matchRange->location, matchRange->length)];
		}
	}
	
	// conditionals
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesConditionalsKey]) {
		RKEnumerator *conditionalsEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterConditionalsRegex string:searchString] autorelease];
		NSColor *conditionalsColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesConditionalsColorKey];
		
		while ([conditionalsEnum nextRanges] != NULL) {
			NSRangePointer matchRange = [conditionalsEnum currentRanges];
			
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:conditionalsColor forCharacterRange:NSMakeRange(searchRange.location+matchRange->location, matchRange->length)];
		}
	}
	
	// registers
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesRegistersKey]) {
		RKEnumerator *registersEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterRegistersRegex string:searchString] autorelease];
		NSColor *registersColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesRegistersColorKey];
		
		while ([registersEnum nextRanges] != NULL) {
			NSRangePointer matchRange = [registersEnum currentRanges];
			
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:registersColor forCharacterRange:NSMakeRange(searchRange.location+matchRange->location, matchRange->length)];
		}
	}
	
	// pre ops
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesPreOpsKey]) {
		RKEnumerator *preOpsEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterPreOpsRegex string:searchString] autorelease];
		NSColor *preOpsColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesPreOpsColorKey];
		
		while ([preOpsEnum nextRanges] != NULL) {
			NSRangePointer matchRange = [preOpsEnum currentRanges];
			
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:preOpsColor forCharacterRange:NSMakeRange(searchRange.location+matchRange->location, matchRange->length)];
		}
	}
	
	// numbers
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesNumbersKey]) {
		RKEnumerator *numbersEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterNumbersRegex string:searchString] autorelease];
		NSColor *numbersColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesNumbersColorKey];
		
		while ([numbersEnum nextRanges] != NULL) {
			NSRangePointer matchRange = [numbersEnum currentRanges];
			
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:numbersColor forCharacterRange:NSMakeRange(searchRange.location+matchRange->location, matchRange->length)];
		}
	}
	
	// hexadecimals
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesHexadecimalsKey]) {
		RKEnumerator *hexadecimalsEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterHexadeicmalsRegex string:searchString] autorelease];
		NSColor *hexadecimalsColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesHexadecimalsColorKey];
		
		while ([hexadecimalsEnum nextRanges] != NULL) {
			NSRangePointer matchRange = [hexadecimalsEnum currentRanges];
			
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:hexadecimalsColor forCharacterRange:NSMakeRange(searchRange.location+matchRange->location, matchRange->length)];
		}
	}
	
	// binaries
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesBinariesKey]) {
		RKEnumerator *binariesEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterBinariesRegex string:searchString] autorelease];
		NSColor *binariesColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesBinariesColorKey];
		
		while ([binariesEnum nextRanges] != NULL) {
			NSRangePointer matchRange = [binariesEnum currentRanges];
			
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:binariesColor forCharacterRange:NSMakeRange(searchRange.location+matchRange->location, matchRange->length)];
		}
	}
	
	// strings
	RKEnumerator *stringsEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterStringsRegex string:searchString] autorelease];
	NSColor *stringsColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesStringsColorKey];
	
	while ([stringsEnum nextRanges] != NULL) {
		NSRangePointer matchRange = [stringsEnum currentRanges];
		
		[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:stringsColor forCharacterRange:NSMakeRange(searchRange.location+matchRange->location, matchRange->length)];
	}
	
	// comments
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesCommentsKey]) {
		RKEnumerator *commentsEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterCommentsRegex string:searchString] autorelease];
		NSColor *commentsColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesCommentsColorKey];
		
		while ([commentsEnum nextRanges] != NULL) {
			NSRangePointer matchRange = [commentsEnum currentRanges];
			
			[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:commentsColor forCharacterRange:NSMakeRange(searchRange.location+matchRange->location, matchRange->length)];
		}
		
		// multiline comments
		RKEnumerator *multilineCommentsEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterMultilineCommentsRegex string:string] autorelease];
		
		while ([multilineCommentsEnum nextRanges] != NULL) {
			NSRangePointer matchRange = [multilineCommentsEnum currentRanges];
			
			if (NSIntersectionRange(*matchRange, searchRange).length)
				[[_textView layoutManager] addTemporaryAttribute:NSForegroundColorAttributeName value:commentsColor forCharacterRange:*matchRange];
			else if (matchRange->location > NSMaxRange(searchRange))
				break;
		}
	}
	
	[self setIsHighlighting:NO];
}

- (void)_textViewBoundsDidChange:(NSNotification *)note {
	[self performSyntaxHighlighting];
}

- (void)_textViewFrameDidChange:(NSNotification *)note {
	[self performSyntaxHighlighting];
}

- (void)_fileTextDidChange:(NSNotification *)note {

}

- (void)_symbolScannerFinishedScanning:(NSNotification *)note {
	[self performSelector:@selector(performSyntaxHighlighting) withObject:nil afterDelay:0.0];
}

- (void)_symbolScannerFinishedScanningHighlightImmediately:(NSNotification *)note {
	[self performSyntaxHighlighting];
}
@end
