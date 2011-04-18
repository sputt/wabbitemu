//
//  WCFindInProjectViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFindInProjectViewController.h"
#import "WCFile.h"
#import "WCSymbol.h"
#import "WCFindInProjectResult.h"
#import "WCProject.h"
#import "WCTextStorage.h"
#import "NSTreeController+WCExtensions.h"
#import "WCDoEverythingTextFieldCell.h"
#import "WCSymbolScanner.h"
#import "NSAlert-OAExtensions.h"
#import "NSTextView+WCExtensions.h"
#import "WCGeneralPerformer.h"

#define restrict
#import <RegexKit/RegexKit.h>

@implementation WCFindInProjectViewController

#pragma mark *** Subclass Overrides ***
- (void)dealloc {
	[_findResults release];
	[_findString release];
	[_matchesString release];
	[_replaceString release];
	[_replacementRanges release];
    [super dealloc];
}

- (NSString *)viewNibName {
	return @"WCFindInProjectView";
}

- (void)loadView {
	[self setIgnoreCase:YES];
	[self setUseRegularExpression:NO];
	[self setFindScope:WCFindScopeAllFiles];
	[self setFindSubScope:WCFindSubScopeContains];
	
	[super loadView];
	
	[[self outlineView] setDoubleAction:@selector(_findInProjectOutlineViewDoubleAction:)];
	[[self outlineView] setTarget:[self project]];
}

- (NSResponder *)firstResponder {
	return _searchField;
}

- (NSArray *)selectedNodes {
	return [(NSTreeController *)[[self outlineView] dataSource] treeNodesForRepresentedObjects:[self selectedObjects]];
}

- (NSArray *)selectedObjects {
	return [(NSTreeController *)[[self outlineView] dataSource] selectedRepresentedObjects];
}
- (void)setSelectedObjects:(NSArray *)selectedObjects {
	[(NSTreeController *)[[self outlineView] dataSource] setSelectedRepresentedObjects:selectedObjects];
}
#pragma mark *** Protocol Overrides ***
#pragma mark NSOutlineViewDelegate
- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item {	
	WCFindInProjectResult *result = [item representedObject];
	
	[cell setIcon:[result icon]];
	[cell setIconSize:NSMakeSize(16.0, 16.0)];
	[cell setBadgeCount:[[result childNodes] count]];
	
	if ([result parentNode]) {
		NSMutableAttributedString *string = [[[result findString] mutableCopy] autorelease];
		[string addAttribute:NSFontAttributeName value:[cell font] range:NSMakeRange(0, [[string string] length])];
		
		if ([cell isHighlighted])
			[string applyFontTraits:NSBoldFontMask range:NSMakeRange(0, [[string string] length])];
		
		[cell setAttributedStringValue:string];
	}
}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldShowCellExpansionForTableColumn:(NSTableColumn *)tableColumn item:(id)item {
	return YES;
}
#pragma mark NSUserInterfaceValidations
- (BOOL)validateUserInterfaceItem:(id<NSValidatedUserInterfaceItem>)item {
	if ([item action] == @selector(changeFindScope:)) {
		if ([item tag] == [self findScope])
			[(NSMenuItem *)item setState:NSOnState];
		else
			[(NSMenuItem *)item setState:NSOffState];
	}
	else if ([item action] == @selector(changeFindSubScope:)) {
		if ([item tag] == [self findSubScope])
			[(NSMenuItem *)item setState:NSOnState];
		else
			[(NSMenuItem *)item setState:NSOffState];
		
		if ([self useRegularExpression])
			return NO;
	}
	else if ([item action] == @selector(toggleReplaceControls:)) {
		if ([item tag] == 0)
			[(NSMenuItem *)item setState:([self replaceControlsVisible])?NSOffState:NSOnState];
		else
			[(NSMenuItem *)item setState:([self replaceControlsVisible])?NSOnState:NSOffState];
	}
	return YES;
}

@synthesize outlineView=_outlineView;
@synthesize isFinding=_isFinding;
@synthesize findResults=_findResults;
@dynamic mutableFindResults;
- (NSMutableArray *)mutableFindResults {
	if (!_findResults)
		_findResults = [[NSMutableArray alloc] init];
	return [self mutableArrayValueForKey:@"findResults"];
}
@synthesize findString=_findString;
@synthesize matchesString=_matchesString;
@synthesize ignoreCase=_ignoreCase;
@synthesize useRegularExpression=_useRegularExpression;
@synthesize findScope=_findScope;
@synthesize findSubScope=_findSubScope;
@synthesize replaceControlsVisible=_replaceControlsVisible;
@synthesize replaceString=_replaceString;
@synthesize numberOfFindMatches=_numberOfFindMatches;
@synthesize isReplacing=_isReplacing;
@synthesize replacementRanges=_replacementRanges;

- (IBAction)findInProject:(id)sender; {
	if ([self isFinding] || ![self findString] || ![[self findString] length])
		return;
	
	[self setIsFinding:YES];
	// [[self mutableFindResults] removeAllObjects] is pretty slow when we have a ton of results to remove
	[[self mutableFindResults] removeObjectsAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [[self findResults] count])]];
	
	dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),^{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		NSArray *files = ([self findScope] == WCFindScopeAllFiles)?[[self project] textFiles]:[[[self project] openFiles] allObjects];
		NSString *findString = [self findString];
		NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
		[style setLineBreakMode:NSLineBreakByTruncatingTail];
		NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]],NSFontAttributeName,style,NSParagraphStyleAttributeName, nil];
		//NSArray *symbols = [[self project] symbols];
		BOOL useRegularExpression = [self useRegularExpression];
		NSStringCompareOptions options = ([self ignoreCase])?NSCaseInsensitiveSearch:NSLiteralSearch;
		RKCompileOption rOptions = ([self ignoreCase])?(RKCompileUTF8|RKCompileCaseless|RKCompileMultiline):(RKCompileUTF8|RKCompileMultiline);
		WCFindSubScope subscope = [self findSubScope];
		NSUInteger matchedFiles = 0;
		NSUInteger matches = 0;
		NSDictionary *mAttributes = [[WCGeneralPerformer sharedPerformer] findAttributes];
		
		for (WCFile *file in files) {
			NSString *string = [[[[file textStorage] string] copy] autorelease];
			NSRange entireRange = NSMakeRange(0, [string length]);
			NSRange searchRange = entireRange;
			NSRange matchRange = NSMakeRange(NSNotFound, 0);
			
			WCFindInProjectResult *parent = [WCFindInProjectResult findInProjectResultForFile:file inRange:matchRange withString:nil symbol:nil];
			
			if (useRegularExpression) {
				RKRegex *regex = [[[RKRegex alloc] initWithRegexString:findString options:rOptions] autorelease];
				
				RKEnumerator *enumerator = [[[RKEnumerator alloc] initWithRegex:regex string:string inRange:entireRange] autorelease];
				
				while ([enumerator nextRanges] != NULL) {
					matchRange = *[enumerator currentRanges];
					
					matches++;
					
					if (![[parent childNodes] count]) {
						matchedFiles++;
						// im not sure if this is necessary, but it can't hurt to wait until this returns to insert any children into 'parent'
						dispatch_sync(dispatch_get_main_queue(), ^{
							[[self mutableFindResults] addObject:parent];
						});
					}
					
					NSRange lRange = [string lineRangeForRange:matchRange];
					NSMutableAttributedString *lString = [[[NSMutableAttributedString alloc] initWithString:[[string substringWithRange:lRange] stringByTrimmingCharactersInSet:[NSCharacterSet newlineCharacterSet]] attributes:attributes] autorelease];
					[lString addAttributes:mAttributes range:NSMakeRange(matchRange.location-lRange.location, matchRange.length)];
					
					WCFindInProjectResult *result = [WCFindInProjectResult findInProjectResultForFile:file inRange:matchRange withString:lString symbol:nil];
					
					// same as above
					dispatch_sync(dispatch_get_main_queue(), ^{
						[[parent mutableChildNodes] addObject:result];
					});
				}
			}
			else {
				CFLocaleRef locale = CFLocaleCopyCurrent();
				// the CFStringTokenizer documentation says to pass kCFStringTokenizerUnitWordBoundary to do whole word searching
				// which is what we want in this case
				CFStringTokenizerRef tokenizer = CFStringTokenizerCreate(kCFAllocatorDefault, (CFStringRef)string, CFRangeMake(0, (CFIndex)[string length]), kCFStringTokenizerUnitWordBoundary, locale);
				CFRelease(locale);
				
				// search until our starting location is greater than or equal to the length of the string
				while (searchRange.location < entireRange.length) {
					matchRange = [string rangeOfString:findString options:options range:searchRange];
					
					if (matchRange.location == NSNotFound)
						break;
					
					BOOL subscopeMatches = YES;
					if (subscope == WCFindSubScopeWholeWord) {
						CFStringTokenizerGoToTokenAtIndex(tokenizer, (CFIndex)matchRange.location);
						CFRange tRange = CFStringTokenizerGetCurrentTokenRange(tokenizer);
						
						// the ranges must match exactly, otherwise we are lying inside a word boundary
						if (tRange.location == kCFNotFound ||
							tRange.location != matchRange.location ||
							tRange.length != matchRange.length)
							subscopeMatches = NO;
					}
					else if (subscope == WCFindSubScopeStartsWith) {
						CFStringTokenizerGoToTokenAtIndex(tokenizer, (CFIndex)matchRange.location);
						CFRange tRange = CFStringTokenizerGetCurrentTokenRange(tokenizer);
						
						// the beginning of the ranges must match and the match length can't be longer than the word length
						if (tRange.location == kCFNotFound ||
							tRange.location != matchRange.location ||
							matchRange.length > tRange.length)
							subscopeMatches = NO;
					}
					else if (subscope == WCFindSubScopeEndsWith) {
						CFStringTokenizerGoToTokenAtIndex(tokenizer, (CFIndex)matchRange.location);
						CFRange tRange = CFStringTokenizerGetCurrentTokenRange(tokenizer);
						
						// the end of the ranges must line up
						if (tRange.location == kCFNotFound ||
							NSMaxRange(matchRange) != (tRange.location + tRange.length))
							subscopeMatches = NO;
					}
					// NSString already performs contains matching, so there is no need to check for this explicitly
					
					if (subscopeMatches) {
						matches++;
						
						if (![[parent childNodes] count]) {
							matchedFiles++;
							// im not sure if this is necessary, but it can't hurt to wait until this returns to insert any children into 'parent'
							dispatch_sync(dispatch_get_main_queue(), ^{
								[[self mutableFindResults] addObject:parent];
							});
						}
						
						NSRange lRange = [string lineRangeForRange:matchRange];
						NSMutableAttributedString *lString = [[[NSMutableAttributedString alloc] initWithString:[[string substringWithRange:lRange] stringByTrimmingCharactersInSet:[NSCharacterSet newlineCharacterSet]] attributes:attributes] autorelease];
						[lString addAttributes:mAttributes range:NSMakeRange(matchRange.location-lRange.location, matchRange.length)];
						
						WCSymbol *mSymbol = nil;
						for (WCSymbol *symbol in [[file symbolScanner] symbols]) {
							if (NSLocationInRange(matchRange.location, [symbol symbolRange])) {
								mSymbol = symbol;
								break;
							}
						}
						
						WCFindInProjectResult *result = [WCFindInProjectResult findInProjectResultForFile:file inRange:matchRange withString:lString symbol:mSymbol];
						
						// same as above
						dispatch_sync(dispatch_get_main_queue(), ^{
							[[parent mutableChildNodes] addObject:result];
						});
					}
					
					searchRange = NSMakeRange(NSMaxRange(matchRange), entireRange.length-NSMaxRange(matchRange));
				}
				
				CFRelease(tokenizer);
			}
		}
		
		NSString *formatString = nil;
		if (matchedFiles == 1 && matches == 1)
			formatString = NSLocalizedString(@"%u match in %u file", @"single match in single file");
		else if (matchedFiles == 1)
			formatString = NSLocalizedString(@"%u matches in %u file", @"multiple matches in single file");
		else if (matches == 1)
			formatString = NSLocalizedString(@"%u match in %u files", @"single match in multiple files");
		else
			formatString = NSLocalizedString(@"%u matches in %u files", @"multiple matches in multiple files");
			
		NSString *matchesString = [NSString stringWithFormat:formatString,matches,matchedFiles];
		
		dispatch_sync(dispatch_get_main_queue(), ^{
			[self setNumberOfFindMatches:matches];
			[self setMatchesString:matchesString];
			[self setIsFinding:NO];
		});
		
		[pool release];
	});
}

- (IBAction)replaceInProject:(id)sender; {
	if ([self isReplacing]) {
		NSBeep();
		return;
	}
	
	if (!_hasConfirmationForReplace) {
		NSAlert *alert = [NSAlert alertWithMessageText:NSLocalizedString(@"Confirm Replace All", @"Confirm Replace All") defaultButton:NSLocalizedString(@"Replace All", @"Replace All") alternateButton:NS_LOCALIZED_STRING_CANCEL otherButton:nil informativeTextWithFormat:[NSString stringWithFormat:NSLocalizedString(@"Are you sure you want to replace \"%@\" with \"%@\"?", @"replace in project alert informative text"),[self findString],[self replaceString]]];
		
		[alert beginSheetModalForWindow:[[self view] window] completionHandler:^(NSAlert *rAlert, NSInteger code) {
			if (code != NSAlertDefaultReturn)
				return;
			
			_hasConfirmationForReplace = YES;
			[self replaceInProject:nil];
		}];
		return;
	}
	
	[self setIsFinding:YES];
	[self setIsReplacing:YES];
	// [[self mutableFindResults] removeAllObjects] is pretty slow when we have a ton of results to remove
	[[self mutableFindResults] removeObjectsAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [[self findResults] count])]];
	
	dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),^{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		NSArray *files = ([self findScope] == WCFindScopeAllFiles)?[[self project] textFiles]:[[[self project] openFiles] allObjects];
		NSString *findString = [self findString];
		NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
		[style setLineBreakMode:NSLineBreakByTruncatingTail];
		NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]],NSFontAttributeName,style,NSParagraphStyleAttributeName, nil];
		//NSArray *symbols = [[self project] symbols];
		BOOL useRegularExpression = [self useRegularExpression];
		NSStringCompareOptions options = ([self ignoreCase])?NSCaseInsensitiveSearch:NSLiteralSearch;
		RKCompileOption rOptions = ([self ignoreCase])?(RKCompileUTF8|RKCompileCaseless|RKCompileMultiline):(RKCompileUTF8|RKCompileMultiline);
		WCFindSubScope subscope = [self findSubScope];
		NSString *replaceString = [self replaceString];
		NSUInteger matchedFiles = 0;
		NSUInteger matches = 0;
		NSDictionary *mAttributes = [[WCGeneralPerformer sharedPerformer] findAttributes];
		
		for (WCFile *file in files) {
			NSMutableString *mString = [[[[file textStorage] string] mutableCopy] autorelease];
			
			if (useRegularExpression) {
				// replace everything first, then update our matches string
				// its too complicated to show all the affected matches, because the replaceString can have capture references
				RKRegex *regex = [[[RKRegex alloc] initWithRegexString:findString options:rOptions] autorelease];
				NSUInteger replacements = [mString match:regex replace:RKReplaceAll withString:replaceString];
				
				if (!replacements)
					continue;
				
				matchedFiles++;
				matches += replacements;
				
				dispatch_sync(dispatch_get_main_queue(), ^{
					NSTextView *textView = [file textViewForFindInProjectReplace];
					if ([textView shouldChangeTextInRange:NSMakeRange(0, [[textView string] length]) replacementString:mString]) {
						[[textView textStorage] replaceCharactersInRange:NSMakeRange(0, [[textView string] length]) withString:mString];
					}
				});
			}
			else {
				// search the string first and keep track of the ranges that match
				NSString *string = [[file textStorage] string];
				NSMutableArray *replaceRanges = [NSMutableArray array];
				NSRange matchRange = NSMakeRange(NSNotFound, 0);
				NSRange entireRange = NSMakeRange(0, [string length]);
				NSRange searchRange = entireRange;
				
				// the CFStringTokenizer documentation says to pass kCFStringTokenizerUnitWordBoundary to do whole word searching
				// which is what we want in this case
				CFLocaleRef locale = CFLocaleCopyCurrent();
				CFStringTokenizerRef tokenizer = CFStringTokenizerCreate(kCFAllocatorDefault, (CFStringRef)string, CFRangeMake(0, (CFIndex)[string length]), kCFStringTokenizerUnitWordBoundary, locale);
				CFRelease(locale);
				
				// search until our starting location is greater than or equal to the length of the string
				while (searchRange.location < entireRange.length) {
					matchRange = [string rangeOfString:findString options:options range:searchRange];
					
					if (matchRange.location == NSNotFound)
						break;
					
					BOOL subscopeMatches = YES;
					if (subscope == WCFindSubScopeWholeWord) {
						CFStringTokenizerGoToTokenAtIndex(tokenizer, (CFIndex)matchRange.location);
						CFRange tRange = CFStringTokenizerGetCurrentTokenRange(tokenizer);
						
						// the ranges must match exactly, otherwise we are lying inside a word boundary
						if (tRange.location == kCFNotFound ||
							tRange.location != matchRange.location ||
							tRange.length != matchRange.length)
							subscopeMatches = NO;
					}
					else if (subscope == WCFindSubScopeStartsWith) {
						CFStringTokenizerGoToTokenAtIndex(tokenizer, (CFIndex)matchRange.location);
						CFRange tRange = CFStringTokenizerGetCurrentTokenRange(tokenizer);
						
						// the beginning of the ranges must match and the match length can't be longer than the word length
						if (tRange.location == kCFNotFound ||
							tRange.location != matchRange.location ||
							matchRange.length > tRange.length)
							subscopeMatches = NO;
					}
					else if (subscope == WCFindSubScopeEndsWith) {
						CFStringTokenizerGoToTokenAtIndex(tokenizer, (CFIndex)matchRange.location);
						CFRange tRange = CFStringTokenizerGetCurrentTokenRange(tokenizer);
						
						// the end of the ranges must line up
						if (tRange.location == kCFNotFound ||
							NSMaxRange(matchRange) != (tRange.location + tRange.length))
							subscopeMatches = NO;
					}
					// NSString already performs contains matching, so there is no need to check for this explicitly
					
					if (subscopeMatches)
						[replaceRanges addObject:[NSValue valueWithRange:matchRange]];
					
					searchRange = NSMakeRange(NSMaxRange(matchRange), entireRange.length-NSMaxRange(matchRange));
				}
				
				CFRelease(tokenizer);
				
				if (![replaceRanges count])
					continue;
				
				matchedFiles++;
				matches += [replaceRanges count];
				
				// perform the replacements
				dispatch_sync(dispatch_get_main_queue(), ^{
					// wait until this finishes on the main thread, then set our new ranges
					// i don't know of a better way to do this, not sure how to pass around values in blocks yet
					NSArray *newRanges = [[file textViewForFindInProjectReplace] replaceCharactersInRanges:[[replaceRanges copy] autorelease] withString:[self replaceString]];
					[self setReplacementRanges:newRanges];
				});
				
				// grab the string again, since we just performed the replacements
				string = [[file textStorage] string];
				// create find results for the new ranges
				WCFindInProjectResult *parent = [WCFindInProjectResult findInProjectResultForFile:file inRange:matchRange withString:nil symbol:nil];
				
				for (NSValue *rValue in [self replacementRanges]) {
					NSRange matchRange = [rValue rangeValue];
					NSRange lRange = [string lineRangeForRange:matchRange];
					NSMutableAttributedString *lString = [[[NSMutableAttributedString alloc] initWithString:[[string substringWithRange:lRange] stringByTrimmingCharactersInSet:[NSCharacterSet newlineCharacterSet]] attributes:attributes] autorelease];
					[lString addAttributes:mAttributes range:NSMakeRange(matchRange.location-lRange.location, matchRange.length)];
					
					WCFindInProjectResult *result = [WCFindInProjectResult findInProjectResultForFile:file inRange:matchRange withString:lString symbol:nil];
					
					[[parent mutableChildNodes] addObject:result];
				}
				
				dispatch_sync(dispatch_get_main_queue(), ^{
					[[self mutableFindResults] addObject:parent];
				});
			}
		}
		
		NSString *formatString = nil;
		if (matchedFiles == 1 && matches == 1)
			formatString = NSLocalizedString(@"%u replacement in %u file", @"single replacement in single file");
		else if (matchedFiles == 1)
			formatString = NSLocalizedString(@"%u replacements in %u file", @"multiple replacements in single file");
		else if (matches == 1)
			formatString = NSLocalizedString(@"%u replacement in %u files", @"single replacement in multiple files");
		else
			formatString = NSLocalizedString(@"%u replacements in %u files", @"multiple replacements in multiple files");
		
		NSString *matchesString = [NSString stringWithFormat:formatString,matches,matchedFiles];
		
		dispatch_sync(dispatch_get_main_queue(), ^{
			[self setNumberOfFindMatches:matches];
			[self setMatchesString:matchesString];
			[self setIsFinding:NO];
			[self setIsReplacing:NO];
			_hasConfirmationForReplace = NO;
		});
		
		[pool release];
	});
}

- (IBAction)toggleReplaceControls:(id)sender; {
	if (([sender tag] == 0 && ![self replaceControlsVisible]) ||
		([sender tag] == 1 && [self replaceControlsVisible]))
		return;
	
	_toggleReplaceControlsAnimation = [[NSViewAnimation alloc] initWithDuration:0.25 animationCurve:NSAnimationEaseIn];
	NSView *topView = [_searchField superview];
	NSRect frame = [topView frame];
	NSView *bottomView = [_outlineView enclosingScrollView];
	NSRect scrollFrame = [bottomView frame];
	static const CGFloat delta = 45.0;
	
	[_toggleReplaceControlsAnimation setDelegate:self];
	
	if (_replaceControlsVisible) {
		[_toggleReplaceControlsAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:topView,NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(frame.origin.x, frame.origin.y+delta, NSWidth(frame), NSHeight(frame)-delta)],NSViewAnimationEndFrameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:bottomView,NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(scrollFrame.origin.x, scrollFrame.origin.y, NSWidth(scrollFrame), NSHeight(scrollFrame)+delta)],NSViewAnimationEndFrameKey, nil], nil]];
	}
	else {
		[_toggleReplaceControlsAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:topView,NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(frame.origin.x, frame.origin.y-delta, NSWidth(frame), NSHeight(frame)+delta)],NSViewAnimationEndFrameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:bottomView,NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(scrollFrame.origin.x, scrollFrame.origin.y, NSWidth(scrollFrame), NSHeight(scrollFrame)-delta)],NSViewAnimationEndFrameKey, nil], nil]];
	}
	
	[_toggleReplaceControlsAnimation startAnimation];
}

- (IBAction)changeFindScope:(NSMenuItem *)sender; {
	[self setFindScope:[sender tag]];
}
- (IBAction)changeFindSubScope:(NSMenuItem *)sender; {
	[self setFindSubScope:[sender tag]];
}

- (NSUInteger)countOfFindResults {
	return [_findResults count];
}
- (id)objectInFindResultsAtIndex:(NSUInteger)index {
	return [_findResults objectAtIndex:index];
}
- (void)insertObject:(id)object inFindResultsAtIndex:(NSUInteger)index; {
	if (!_findResults)
		_findResults = [[NSMutableArray alloc] init];
	[_findResults insertObject:object atIndex:index];
}
- (void)removeObjectFromFindResultsAtIndex:(NSUInteger)index; {	
	[_findResults removeObjectAtIndex:index];
}
- (void)removeFindResultsAtIndexes:(NSIndexSet *)indexes {
	[_findResults removeObjectsAtIndexes:indexes];
}

- (void)animationDidEnd:(NSAnimation *)animation {
	[self setReplaceControlsVisible:![self replaceControlsVisible]];
	[_toggleReplaceControlsAnimation release];
	_toggleReplaceControlsAnimation = nil;
	
	if ([self replaceControlsVisible]) {
		[_searchField setNextKeyView:_replaceField];
		[_replaceField setNextKeyView:_outlineView];
	}
	else {
		[_searchField setNextKeyView:_outlineView];
		if ([[[self view] window] firstResponder] == _replaceField)
			[[[self view] window] makeFirstResponder:_searchField];
	}
}
@end
