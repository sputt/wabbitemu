//
//  WCSymbolScanner.m
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSymbolScanner.h"
#import "WCFile.h"
#import "WCTextStorage.h"
#import "WCSymbol.h"
#import "NSObject+WCExtensions.h"
#import "WCProject.h"
#import "WCBuildTarget.h"
#import "WCSyntaxHighlighter.h"

#define restrict
#import <RegexKit/RegexKit.h>

NSString* const kWCSymbolScannerFinishedScanningNotification = @"kWCSymbolScannerFinishedScanningNotification";
NSString* const kWCSymbolScannerFinishedScanningAfterSymbolsCaseSensitiveDidChangeNotification = @"kWCSymbolScannerFinishedScanningAfterSymbolsCaseSensitiveDidChangeNotification";

RKRegex *kWCSyntaxHighlighterMultilineCommentsRegex = nil;

static RKRegex *kWCSymbolScannerLabelsRegex = nil;
static RKRegex *kWCSymbolScannerEquatesRegex = nil;
static RKRegex *kWCSymbolScannerDefinesRegex = nil;
static RKRegex *kWCSymbolScannerMacrosRegex = nil;

@interface WCSymbolScanner (Private)
- (void)_scanSymbols;
- (void)_repopulateSymbolStrings;
@end

@implementation WCSymbolScanner

+ (void)initialize {
	if ([WCSymbolScanner class] != self)
		return;
	
	// general pattern for labels, including checks for some special cases that we don't want treated as labels
	// '^' checks for the beginning of a line, i.e. a label must begin in column 0, otherwise it's not a label
	// '[A-z0-9_!?]+' matches the rest of the label
	// '(?!\\()' is a look ahead assertion that ensures calls to macros are ignored as potential label names; somemacro() is ignored whereas somemacro would be considered a valid label name
	kWCSymbolScannerLabelsRegex = [[RKRegex alloc] initWithRegexString:@"^[A-z0-9_!?]+(?!\\()" options:RKCompileUTF8|RKCompileMultiline];
	// general pattern for equates, which are just a special kind of label
	// '^' checks for the beginning of a line as above
	// '(?<name> ...)' names the capture for the name of the equate so we can extract it later
	// then we skip any whitespace and look for '=', '.equ', '.EQU', 'equ', or 'EQU' which tells us we have an equate
	// again, skip whitespace then look for the value of equate which '(?<value> ...)' captures so we can get at it later
	kWCSymbolScannerEquatesRegex = [[RKRegex alloc] initWithRegexString:@"^(?<name>[A-z0-9_!?]+)\\s*(?:=|\\.equ|\\.EQU|equ|EQU)\\s*(?<value>[-+$._!? ()A-z0-9]+)" options:RKCompileUTF8|RKCompileMultiline];
	// general pattern for defines
	// same as the previous cases, but we have to for '#define' or '#DEFINE' to start off the match
	kWCSymbolScannerDefinesRegex = [[RKRegex alloc] initWithRegexString:@"(?:#define|#DEFINE)\\s+(?<name>[A-z0-9_!?.]+)" options:RKCompileUTF8];
	// general pattern for macros
	// same as above, but we check for '#macro' and '#MACRO' instead
	kWCSymbolScannerMacrosRegex = [[RKRegex alloc] initWithRegexString:@"(?:#macro|#MACRO)\\s+(?<name>[A-z0-9_!?.]+)" options:RKCompileUTF8];
	// pattern for multiline comments
	// the first part of the alteration has the *? qualifier after the dot to ensure it doesn't consume the '#endcomment' tag
	kWCSyntaxHighlighterMultilineCommentsRegex = [[RKRegex alloc] initWithRegexString:@"(?:#comment.*?#endcomment)|(?:#comment.*)" options:RKCompileUTF8|RKCompileDotAll|RKCompileMultiline];
}

- (id)initWithFile:(WCFile *)file; {
	if (!(self = [super init]))
		return nil;
	
	_file = file;
	
	[self setupNotificationObserving];
	
	[self _scanSymbols];
	
	return self;
}

- (void)dealloc {
	/*
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	 */
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	_file = nil;
	[_symbols release];
	[_labelNamesToSymbols release];
	[_equateNamesToSymbols release];
	[_defineNamesToSymbols release];
	[_macroNamesToSymbols release];
    [super dealloc];
}

- (NSArray *)notificationDictionaries {
	return [NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_fileTextDidChange:)),kNSObjectSelectorKey,NSTextStorageDidProcessEditingNotification,kNSObjectNotificationNameKey,[_file textStorage],kNSObjectNotificationObjectKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_activeBuildTargetDidChange:)),kNSObjectSelectorKey,kWCProjectActiveBuildTargetDidChangeNotification,kNSObjectNotificationNameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_labelsAreCaseSensitiveDidChange:)),kNSObjectSelectorKey,kWCBuildTargetLabelsAreCaseSensitiveDidChangeNotification,kNSObjectNotificationNameKey, nil], nil];
}

@synthesize file=_file;
@synthesize isScanning=_isScanning;
@synthesize symbols=_symbols;
@synthesize labelNamesToSymbols=_labelNamesToSymbols;
@synthesize equateNamesToSymbols=_equateNamesToSymbols;
@synthesize defineNamesToSymbols=_defineNamesToSymbols;
@synthesize macroNamesToSymbols=_macroNamesToSymbols;

- (void)scanSymbols; {
	[self _scanSymbols];
}

- (NSArray *)symbolsForSymbolName:(NSString *)name; {
	NSMutableArray *retval = [NSMutableArray array];
	BOOL labelsAreCaseSensitive = [[[[self file] project] activeBuildTarget] symbolsAreCaseSensitive];
	NSString *compareName = (labelsAreCaseSensitive)?name:[name lowercaseString];
	
	for (WCSymbol *symbol in [self symbols]) {
		NSString *sname = (labelsAreCaseSensitive)?[symbol name]:[[symbol name] lowercaseString];
		
		if ([sname isEqualToString:compareName]) {
			[retval addObject:symbol];
		}
	}
	return [[retval copy] autorelease];
}

- (void)_fileTextDidChange:(NSNotification *)note {
	[self _scanSymbols];
}
- (void)_activeBuildTargetDidChange:(NSNotification *)note {
	WCProject *project = [[self file] project];
	
	if (!project)
		return;
	else if ([note object] == project)
		[self _repopulateSymbolStrings];
}
- (void)_labelsAreCaseSensitiveDidChange:(NSNotification *)note {
	WCProject *project = [[self file] project];
	
	if (!project)
		return;
	else if ([[note object] project] == project && [project activeBuildTarget] == [note object])
		[self _repopulateSymbolStrings];
}

- (void)_scanSymbols; {
	if ([self isScanning])
		return;
	
	[self setIsScanning:YES];
	
	// do our scanning in a different thread
	dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		// grab a copy of the string
		NSString *string = [[[self file] textStorage] string];
		BOOL labelsAreCaseSensitive = [[[[self file] project] activeBuildTarget] symbolsAreCaseSensitive];
		NSMutableArray *multilineCommentRanges = [NSMutableArray array];
		NSMutableArray *symbols = [NSMutableArray arrayWithCapacity:[[self symbols] count]];
		NSMutableDictionary *labelNamesToSymbols = [NSMutableDictionary dictionary];
		NSMutableDictionary *equateNamesToSymbols = [NSMutableDictionary dictionary];
		NSMutableDictionary *defineNamesToSymbols = [NSMutableDictionary dictionary];
		NSMutableDictionary *macroNamesToSymbols = [NSMutableDictionary dictionary];
		
		
		// find all multiline comments, ignore symbols found within the found ranges
		RKEnumerator *mCommentEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterMultilineCommentsRegex string:string] autorelease];
		
		while ([mCommentEnum nextRanges] != NULL) {
			NSRangePointer matchRange = [mCommentEnum currentRanges];
			
			[multilineCommentRanges addObject:[NSValue valueWithRange:*matchRange]];
		}
		
		// find equates
		RKEnumerator *equatesEnum = [[[RKEnumerator alloc] initWithRegex:kWCSymbolScannerEquatesRegex string:string] autorelease];
		NSRange equateRange = [equatesEnum nextRangeForCaptureName:@"name"];
		
		while (equateRange.location != NSNotFound) {
			BOOL insideMComment = NO;
			for (NSValue *rValue in multilineCommentRanges) {
				if (NSIntersectionRange(equateRange, [rValue rangeValue]).length) {
					insideMComment = YES;
					break;
				}
			}
			
			if (!insideMComment) {
				NSString *equateName = [string substringWithRange:equateRange];
				NSString *compareName = (labelsAreCaseSensitive)?equateName:[equateName lowercaseString];
				
				WCSymbol *existingSymbol = nil;
				
				existingSymbol = [WCSymbol symbolWithName:equateName ofType:WCSymbolEquateType inFile:_file withRange:equateRange];
				[existingSymbol setSymbolValue:[[string substringWithRange:[equatesEnum currentRangeForCaptureName:@"value"]] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]];
				
				[symbols addObject:existingSymbol];
				
				[equateNamesToSymbols setObject:existingSymbol forKey:compareName];
			}
			
			equateRange = [equatesEnum nextRangeForCaptureName:@"name"];
		}
		
		// find labels
		RKEnumerator *labelsEnum = [[[RKEnumerator alloc] initWithRegex:kWCSymbolScannerLabelsRegex string:string] autorelease];
		//NSUInteger length = [string length];
		
		while ([labelsEnum nextRanges] != NULL) {
			NSRangePointer labelRange = [labelsEnum currentRanges];
			
			BOOL insideMComment = NO;
			for (NSValue *rValue in multilineCommentRanges) {
				if (NSIntersectionRange(*labelRange, [rValue rangeValue]).length) {
					insideMComment = YES;
					break;
				}
			}
			
			if (!insideMComment) {
				NSString *labelName = [string substringWithRange:*labelRange];
				NSString *compareName = (labelsAreCaseSensitive)?labelName:[labelName lowercaseString];
				
				// dont count the special temp labels
				if ([labelName characterAtIndex:0] == '_')
					continue;
				 
				// ignore macro invocations
				/*
				NSUInteger indexOfLastCharacter = NSMaxRange(*labelRange);
				if (indexOfLastCharacter < length && [string characterAtIndex:indexOfLastCharacter] == '(')
					continue;
				*/
				 
				if ([equateNamesToSymbols objectForKey:compareName] == nil) {
					
					WCSymbol *existingSymbol = nil;
					
					existingSymbol = [WCSymbol symbolWithName:labelName ofType:WCSymbolLabelType inFile:_file withRange:*labelRange];
					
					[symbols addObject:existingSymbol];
					
					[labelNamesToSymbols setObject:existingSymbol forKey:compareName];
				}
			}
		}
		
		// find defines
		RKEnumerator *definesEnum = [[[RKEnumerator alloc] initWithRegex:kWCSymbolScannerDefinesRegex string:string] autorelease];
		NSRange defineRange = [definesEnum nextRangeForCaptureName:@"name"];
		
		while (defineRange.location != NSNotFound) {
			BOOL insideMComment = NO;
			for (NSValue *rValue in multilineCommentRanges) {
				if (NSIntersectionRange(defineRange, [rValue rangeValue]).length) {
					insideMComment = YES;
					break;
				}
			}
			
			if (!insideMComment) {
				NSString *defineName = [string substringWithRange:defineRange];
				NSString *compareName = (labelsAreCaseSensitive)?defineName:[defineName lowercaseString];
				
				WCSymbol *existingSymbol = nil;
				
				existingSymbol = [WCSymbol symbolWithName:defineName ofType:WCSymbolDefineType inFile:_file withRange:defineRange];
				
				[symbols addObject:existingSymbol];
				
				[defineNamesToSymbols setObject:existingSymbol forKey:compareName];
			}
			
			defineRange = [definesEnum nextRangeForCaptureName:@"name"];
		}
		
		// find macros
		RKEnumerator *macrosEnum = [[[RKEnumerator alloc] initWithRegex:kWCSymbolScannerMacrosRegex string:string] autorelease];
		NSRange macroRange = [macrosEnum nextRangeForCaptureName:@"name"];
		
		while (macroRange.location != NSNotFound) {
			BOOL insideMComment = NO;
			for (NSValue *rValue in multilineCommentRanges) {
				if (NSIntersectionRange(macroRange, [rValue rangeValue]).length) {
					insideMComment = YES;
					break;
				}
			}
			
			if (!insideMComment) {
				NSString *macroName = [string substringWithRange:macroRange];
				NSString *compareName = (labelsAreCaseSensitive)?macroName:[macroName lowercaseString];
				
				WCSymbol *existingSymbol = nil;
				
				existingSymbol = [WCSymbol symbolWithName:macroName ofType:WCSymbolMacroType inFile:_file withRange:macroRange];
				
				[symbols addObject:existingSymbol];
				
				[macroNamesToSymbols setObject:existingSymbol forKey:compareName];
			}
			
			macroRange = [macrosEnum nextRangeForCaptureName:@"name"];
		}
		
		[self setEquateNamesToSymbols:equateNamesToSymbols];
		[self setLabelNamesToSymbols:labelNamesToSymbols];
		[self setDefineNamesToSymbols:defineNamesToSymbols];
		[self setMacroNamesToSymbols:macroNamesToSymbols];
		
		[symbols sortUsingSelector:@selector(compareUsingSymbolRange:)];
		
		[self setSymbols:symbols];
		
		[pool drain];
		
		// post the notification on the main thread
		dispatch_async(dispatch_get_main_queue(), ^{
			[[NSNotificationCenter defaultCenter] postNotificationName:kWCSymbolScannerFinishedScanningNotification object:self];
			[self setIsScanning:NO];
		});
	});
}

- (void)_repopulateSymbolStrings; {
	dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		NSMutableDictionary *labels = [NSMutableDictionary dictionary];
		NSMutableDictionary *equates = [NSMutableDictionary dictionary];
		NSMutableDictionary *defines = [NSMutableDictionary dictionary];
		NSMutableDictionary *macros = [NSMutableDictionary dictionary];
		NSArray *symbols = [self symbols];
		BOOL symbolsAreCaseSensitive = [[[[self file] project] activeBuildTarget] symbolsAreCaseSensitive];
		
		for (WCSymbol *symbol in symbols) {
			NSString *symbolName = (symbolsAreCaseSensitive)?[symbol name]:[[symbol name] lowercaseString];
			switch ([symbol symbolType]) {
				case WCSymbolLabelType:
					[labels setObject:symbol forKey:symbolName];
					break;
				case WCSymbolEquateType:
					[equates setObject:symbol forKey:symbolName];
					break;
				case WCSymbolDefineType:
					[defines setObject:symbol forKey:symbolName];
					break;
				case WCSymbolMacroType:
					[macros setObject:symbol forKey:symbolName];
					break;
				default:
					break;
			}
		}
		
		[self setLabelNamesToSymbols:labels];
		[self setEquateNamesToSymbols:equates];
		[self setDefineNamesToSymbols:defines];
		[self setMacroNamesToSymbols:macros];
		
		[pool release];
		
		// post the notification on the main thread
		dispatch_async(dispatch_get_main_queue(), ^{
			[[NSNotificationCenter defaultCenter] postNotificationName:kWCSymbolScannerFinishedScanningAfterSymbolsCaseSensitiveDidChangeNotification object:self];
		});
	});
}
@end
