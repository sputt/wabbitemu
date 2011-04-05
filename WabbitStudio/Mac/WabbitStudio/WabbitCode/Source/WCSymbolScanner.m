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
	
	kWCSymbolScannerLabelsRegex = [[RKRegex alloc] initWithRegexString:@"^[A-z0-9_!?]+" options:RKCompileUTF8|RKCompileMultiline];
	kWCSymbolScannerEquatesRegex = [[RKRegex alloc] initWithRegexString:@"^(?<name>[A-z0-9_!?]+)\\s+(?:=|\\.equ|\\.EQU|equ|EQU)\\s+(?<value>[$%A-z0-9_!?]+)" options:RKCompileUTF8|RKCompileMultiline];
	kWCSymbolScannerDefinesRegex = [[RKRegex alloc] initWithRegexString:@"(?:#define|#DEFINE)\\s+(?<name>[A-z0-9_!?.]+)" options:RKCompileUTF8|RKCompileMultiline];
	kWCSymbolScannerMacrosRegex = [[RKRegex alloc] initWithRegexString:@"(?:#macro|#MACRO)\\s+(?<name>[A-z0-9_!?]+)" options:RKCompileUTF8|RKCompileMultiline];
	
	kWCSyntaxHighlighterMultilineCommentsRegex = [[RKRegex alloc] initWithRegexString:@"(?:#comment.*?#endcomment)|(?:#comment.*)" options:RKCompileUTF8|RKCompileDotAll];
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
	[_labelStrings release];
	[_equateStrings release];
	[_defineStrings release];
	[_macroStrings release];
	[_symbols release];
	_file = nil;
    [super dealloc];
}

- (NSArray *)notificationDictionaries {
	return [NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_fileTextDidChange:)),kNSObjectSelectorKey,NSTextStorageDidProcessEditingNotification,kNSObjectNotificationNameKey,[_file textStorage],kNSObjectNotificationObjectKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_activeBuildTargetDidChange:)),kNSObjectSelectorKey,kWCProjectActiveBuildTargetDidChangeNotification,kNSObjectNotificationNameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_labelsAreCaseSensitiveDidChange:)),kNSObjectSelectorKey,kWCBuildTargetLabelsAreCaseSensitiveDidChangeNotification,kNSObjectNotificationNameKey, nil], nil];
}

@synthesize file=_file;
@synthesize isScanning=_isScanning;
@synthesize labelStrings=_labelStrings;
@synthesize equateStrings=_equateStrings;
@synthesize defineStrings=_defineStrings;
@synthesize macroStrings=_macroStrings;
@synthesize symbols=_symbols;
@dynamic equateSymbols;
- (NSArray *)equateSymbols {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCSymbol *symbol in [self symbols]) {
		if ([symbol symbolType] == WCSymbolEquateType)
			[retval addObject:symbol];
	}
	return [[retval copy] autorelease];
}

- (void)scanSymbols; {
	[self _scanSymbols];
}

- (NSArray *)symbolsForSymbolName:(NSString *)name; {
	NSMutableArray *retval = [NSMutableArray array];
	BOOL labelsAreCaseSensitive = [[[[self file] project] activeBuildTarget] labelsAreCaseSensitive];
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
		NSString *string = [[[[[self file] textStorage] string] copy] autorelease];
		BOOL labelsAreCaseSensitive = [[[[self file] project] activeBuildTarget] labelsAreCaseSensitive];
		NSMutableArray *multilineCommentRanges = [NSMutableArray array];
		NSMutableSet *labelStrings = [NSMutableSet set];
		NSMutableSet *equateStrings = [NSMutableSet set];
		NSMutableSet *defineStrings = [NSMutableSet set];
		NSMutableSet *macroStrings = [NSMutableSet set];
		NSArray *currentSymbols = [self symbols];
		NSMutableArray *symbols = [NSMutableArray arrayWithCapacity:[currentSymbols count]];
		
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
				
				[equateStrings addObject:compareName];
				
				WCSymbol *existingSymbol = nil;
				
				for (WCSymbol *symbol in currentSymbols) {
					NSString *symbolName = (labelsAreCaseSensitive)?[symbol name]:[[symbol name] lowercaseString];
					if ([symbol symbolType] == WCSymbolEquateType && [symbolName isEqualToString:compareName]) {
						existingSymbol = symbol;
						[symbol setName:equateName];
						[symbol setSymbolRange:equateRange];
						[symbol setSymbolValue:[string substringWithRange:[equatesEnum currentRangeForCaptureName:@"value"]]];
						break;
					}
				}
				
				if (!existingSymbol) {
					existingSymbol = [WCSymbol symbolWithName:equateName ofType:WCSymbolEquateType inFile:_file withRange:equateRange];
					[existingSymbol setSymbolValue:[string substringWithRange:[equatesEnum currentRangeForCaptureName:@"value"]]];
				}
				
				[symbols addObject:existingSymbol];
			}
			
			equateRange = [equatesEnum nextRangeForCaptureName:@"name"];
		}
		
		// find labels
		RKEnumerator *labelsEnum = [[[RKEnumerator alloc] initWithRegex:kWCSymbolScannerLabelsRegex string:string] autorelease];
		NSUInteger length = [string length];
		
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
				if ([labelName isEqualToString:@"_"])
					continue;
				
				// ignore macro invocations
				NSUInteger indexOfLastCharacter = NSMaxRange(*labelRange);
				if (indexOfLastCharacter < length && [string characterAtIndex:indexOfLastCharacter] == '(')
					continue;
				
				if (![equateStrings containsObject:compareName]) {
					[labelStrings addObject:compareName];
					
					WCSymbol *existingSymbol = nil;
					
					for (WCSymbol *symbol in currentSymbols) {
						NSString *symbolName = (labelsAreCaseSensitive)?[symbol name]:[[symbol name] lowercaseString];
						if ([symbol symbolType] == WCSymbolLabelType && [symbolName isEqualToString:compareName]) {
							existingSymbol = symbol;
							[symbol setName:labelName];
							[symbol setSymbolRange:*labelRange];
							break;
						}
					}
					
					if (!existingSymbol) {
						existingSymbol = [WCSymbol symbolWithName:labelName ofType:WCSymbolLabelType inFile:_file withRange:*labelRange];
					}
					
					[symbols addObject:existingSymbol];
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
				
				[defineStrings addObject:compareName];
				
				WCSymbol *existingSymbol = nil;
				
				for (WCSymbol *symbol in currentSymbols) {
					NSString *symbolName = (labelsAreCaseSensitive)?[symbol name]:[[symbol name] lowercaseString];
					if ([symbol symbolType] == WCSymbolDefineType && [symbolName isEqualToString:compareName]) {
						existingSymbol = symbol;
						[symbol setName:defineName];
						[symbol setSymbolRange:defineRange];
						break;
					}
				}
				
				if (!existingSymbol) {
					existingSymbol = [WCSymbol symbolWithName:defineName ofType:WCSymbolDefineType inFile:_file withRange:defineRange];
				}
				
				[symbols addObject:existingSymbol];
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
				
				[macroStrings addObject:compareName];
				
				WCSymbol *existingSymbol = nil;
				
				for (WCSymbol *symbol in currentSymbols) {
					NSString *symbolName = (labelsAreCaseSensitive)?[symbol name]:[[symbol name] lowercaseString];
					if ([symbol symbolType] == WCSymbolMacroType && [symbolName isEqualToString:compareName]) {
						existingSymbol = symbol;
						[symbol setName:macroName];
						[symbol setSymbolRange:macroRange];
						break;
					}
				}
				
				if (!existingSymbol) {
					existingSymbol = [WCSymbol symbolWithName:macroName ofType:WCSymbolMacroType inFile:_file withRange:macroRange];
				}
				
				[symbols addObject:existingSymbol];
			}
			
			macroRange = [macrosEnum nextRangeForCaptureName:@"name"];
		}
		
		[self setLabelStrings:labelStrings];
		[self setEquateStrings:equateStrings];
		[self setDefineStrings:defineStrings];
		[self setMacroStrings:macroStrings];
		
		[symbols sortUsingSelector:@selector(compareUsingSymbolRange:)];
		
		[self setSymbols:symbols];
		
		[pool release];
		
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
		NSMutableSet *labels = [NSMutableSet set];
		NSMutableSet *equates = [NSMutableSet set];
		NSMutableSet *defines = [NSMutableSet set];
		NSMutableSet *macros = [NSMutableSet set];
		NSArray *symbols = [self symbols];
		BOOL symbolsAreCaseSensitive = [[[[self file] project] activeBuildTarget] labelsAreCaseSensitive];
		
		for (WCSymbol *symbol in symbols) {
			NSString *symbolName = (symbolsAreCaseSensitive)?[symbol name]:[[symbol name] lowercaseString];
			switch ([symbol symbolType]) {
				case WCSymbolLabelType:
					[labels addObject:symbolName];
					break;
				case WCSymbolEquateType:
					[equates addObject:symbolName];
					break;
				case WCSymbolDefineType:
					[defines addObject:symbolName];
					break;
				case WCSymbolMacroType:
					[macros addObject:symbolName];
					break;
				default:
					break;
			}
		}
		
		[self setLabelStrings:labels];
		[self setEquateStrings:equates];
		[self setDefineStrings:defines];
		[self setMacroStrings:macros];
		
		[pool release];
		
		// post the notification on the main thread
		dispatch_async(dispatch_get_main_queue(), ^{
			[[NSNotificationCenter defaultCenter] postNotificationName:kWCSymbolScannerFinishedScanningAfterSymbolsCaseSensitiveDidChangeNotification object:self];
		});
	});
}
@end
