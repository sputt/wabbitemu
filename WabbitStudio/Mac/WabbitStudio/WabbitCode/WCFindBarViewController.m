//
//  WCFindBarViewController.m
//  WabbitStudio
//
//  Created by William Towe on 3/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFindBarViewController.h"
#import "WCTextView.h"
#import "WCFileViewController.h"
#import "WCGeneralPerformer.h"
#import "NSArray+WCExtensions.h"
#import "WCMutableRangeArray.h"

#define restrict
#import <RegexKit/RegexKit.h>

@interface WCFindBarViewController (Private)
- (id)_initWithTextView:(WCTextView *)textView;

- (void)_showFindBar;
- (void)_removeEverything;
@end

@implementation WCFindBarViewController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[NSObject cancelPreviousPerformRequestsWithTarget:self];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_findRanges release];
	[_findString release];
	[_matchesString release];
	_textView = nil;
    [super dealloc];
}

- (void)loadView {
	[super loadView];
	
	if (![self findString] ||
		[[self findString] length] == 0) {
		NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSFindPboard];
		NSString *string = [pboard stringForType:NSStringPboardType];
		
		if ([string length]) {
			[self setFindString:string];
			[self find:nil];
		}
	}
}

#pragma mark NSControlTextEditingDelegate
- (BOOL)control:(NSControl *)control textView:(NSTextView *)textView doCommandBySelector:(SEL)commandSelector; {
	if (commandSelector == @selector(cancelOperation:)) {
		[self closeFindBar:nil];
		return YES;
	}
	else if (commandSelector == @selector(insertNewline:)) {
		[self findNext:nil];
		return YES;
	}
	return NO;
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
@synthesize textView=_textView;
@synthesize searchField=_searchField;
@synthesize findString=_findString;
@synthesize matchesString=_matchesString;
@synthesize ignoreCase=_ignoreCase;
@synthesize useRegularExpression=_useRegularExpression;
@synthesize findScope=_findScope;
@synthesize findSubScope=_findSubScope;
@synthesize replaceControlsVisible=_replaceControlsVisible;
@synthesize replaceString=_replaceString;

+ (void)presentFindBarForTextView:(WCTextView *)textView; {
	WCFindBarViewController *controller = [[[self class] alloc] _initWithTextView:textView];
	
	[controller _showFindBar];
}

- (IBAction)closeFindBar:(id)sender; {
	[[[self view] window] makeFirstResponder:_textView];
	[[_textView layoutManager] removeTemporaryAttribute:NSBackgroundColorAttributeName forCharacterRange:NSMakeRange(0, [[_textView string] length])];
	
	NSRect frame = [[self view] frame];
	_closeAnimation = [[NSViewAnimation alloc] initWithDuration:0.25 animationCurve:NSAnimationEaseIn];
	NSRect scrollFrame = [[_textView enclosingScrollView] frame];
	
	[_closeAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:[self view],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(frame.origin.x, frame.origin.y+frame.size.height, frame.size.width, frame.size.height)],NSViewAnimationEndFrameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:[_textView enclosingScrollView],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(scrollFrame.origin.x, scrollFrame.origin.y, scrollFrame.size.width, scrollFrame.size.height+frame.size.height)],NSViewAnimationEndFrameKey, nil], nil]];
	
	[_closeAnimation setDelegate:self];
	
	[_closeAnimation startAnimation];
}

- (IBAction)toggleReplaceControls:(id)sender; {
	if (([sender tag] == 0 && ![self replaceControlsVisible]) ||
		([sender tag] == 1 && [self replaceControlsVisible]))
		return;
	
	_toggleReplaceControlsAnimation = [[NSViewAnimation alloc] initWithDuration:0.25 animationCurve:NSAnimationEaseIn];
	NSRect frame = [[self view] frame];
	NSRect scrollFrame = [[_textView enclosingScrollView] frame];
	[_toggleReplaceControlsAnimation setDelegate:self];
	
	if ([self replaceControlsVisible]) {
		[_toggleReplaceControlsAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:[self view],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(frame.origin.x, frame.origin.y+floor(frame.size.height/2.0), NSWidth(frame), floor(NSHeight(frame)/2.0))],NSViewAnimationEndFrameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:[_textView enclosingScrollView],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(scrollFrame.origin.x, scrollFrame.origin.y, NSWidth(scrollFrame), NSHeight(scrollFrame)+floor(NSHeight(frame)/2.0))],NSViewAnimationEndFrameKey, nil], nil]];
	}
	else {
		[_toggleReplaceControlsAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:[self view],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(frame.origin.x, frame.origin.y-frame.size.height, NSWidth(frame), NSHeight(frame)*2.0)],NSViewAnimationEndFrameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:[_textView enclosingScrollView],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(scrollFrame.origin.x, scrollFrame.origin.y, NSWidth(scrollFrame), NSHeight(scrollFrame)-NSHeight(frame))],NSViewAnimationEndFrameKey, nil], nil]];
	}
	
	[_toggleReplaceControlsAnimation startAnimation];
}

- (IBAction)find:(id)sender; {
	NSString *string = [_textView string];
	NSRange entireRange = NSMakeRange(0, [string length]);
	
	if (!string || ![string length])
		return;
	
	[[_textView layoutManager] removeTemporaryAttribute:NSBackgroundColorAttributeName forCharacterRange:entireRange];
	[[_textView layoutManager] removeTemporaryAttribute:NSUnderlineStyleAttributeName forCharacterRange:entireRange];
	[[_textView layoutManager] removeTemporaryAttribute:NSUnderlineColorAttributeName forCharacterRange:entireRange];
	
	if (![self findString] ||
		![[self findString] length] ||
		([self useRegularExpression] && ![RKRegex isValidRegexString:[self findString] options:([self ignoreCase])?(RKCompileUTF8|RKCompileMultiline):(RKCompileUTF8|RKCompileMultiline|RKCompileCaseless)])) {
		
		[_findRanges removeAllRanges];
		_numberOfMatches = 0;
		[self setMatchesString:NSLocalizedString(@"No matches", @"No matches")];
		NSBeep();
		return;
	}
	
	NSString *searchString = [self findString];
	NSRange searchRange = entireRange;
	NSStringCompareOptions options = NSCaseInsensitiveSearch;
	RKCompileOption rOptions = ([self ignoreCase])?(RKCompileUTF8|RKCompileMultiline):(RKCompileUTF8|RKCompileMultiline|RKCompileCaseless);
	NSRange matchRange;
	NSUInteger matches = 0;
	NSDictionary *mAttributes = [[WCGeneralPerformer sharedPerformer] findAttributes];
	
	if ([self useRegularExpression]) {
		RKRegex *regex = [[[RKRegex alloc] initWithRegexString:searchString options:rOptions] autorelease];
		RKEnumerator *enumerator = [[[RKEnumerator alloc] initWithRegex:regex string:string] autorelease];
		
		while ([enumerator nextRanges] != NULL) {
			matchRange = *[enumerator currentRanges];
			matches++;
			
			[[_textView layoutManager] addTemporaryAttributes:mAttributes forCharacterRange:matchRange];
		}
	}
	else {
		WCFindSubScope subscope = [self findSubScope];
		CFLocaleRef locale = CFLocaleCopyCurrent();
		// the CFStringTokenizer documentation says to pass kCFStringTokenizerUnitWordBoundary to do whole word searching
		// which is what we want in this case
		CFStringTokenizerRef tokenizer = CFStringTokenizerCreate(kCFAllocatorDefault, (CFStringRef)string, CFRangeMake(0, (CFIndex)[string length]), kCFStringTokenizerUnitWordBoundary, locale);
		CFRelease(locale);
		
		while (searchRange.location < entireRange.length) {
			matchRange = [string rangeOfString:searchString options:options range:searchRange];
			
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
				[_findRanges addRange:matchRange];
				[[_textView layoutManager] addTemporaryAttributes:mAttributes forCharacterRange:matchRange];
			}
			
			searchRange = NSMakeRange(NSMaxRange(matchRange), entireRange.length-NSMaxRange(matchRange));
		}
		
		CFRelease(tokenizer);
	}
	
	_numberOfMatches = matches;
	[self setMatchesString:[NSString stringWithFormat:(matches == 1)?NSLocalizedString(@"%u match", @"single find match"):NSLocalizedString(@"%u matches", @"multiple find matches"),matches]];
}
- (IBAction)findNextOrPrevious:(id)sender; {
	if (![sender selectedSegment])
		[self findPrevious:nil];
	else
		[self findNext:nil];
}
- (IBAction)findNext:(id)sender; {
	if (!_numberOfMatches) {
		NSBeep();
		return;
	}
	
	// TODO: add regex case
	if ([self useRegularExpression]) {
		
	}
	else {
		NSRange eRange = NSMakeRange(0, [[_textView string] length]);
		NSRange sRange = [_textView selectedRange];
		NSRange mRange = [[_textView string] rangeOfString:[self findString] options:([self ignoreCase])?(NSCaseInsensitiveSearch):(NSLiteralSearch) range:NSMakeRange(NSMaxRange(sRange), eRange.length-NSMaxRange(sRange))];
		
		if (mRange.location == NSNotFound) {
			NSBeep();
			return;
		}
		
		[_textView setSelectedRange:mRange];
		[_textView scrollRangeToVisible:mRange];
		[_textView showFindIndicatorForRange:mRange];
	}
}
- (IBAction)findPrevious:(id)sender; {
	if (!_numberOfMatches) {
		NSBeep();
		return;
	}
	
	// TODO: add regex case
	if ([self useRegularExpression]) {
		
	}
	else {
		NSRange sRange = [_textView selectedRange];
		NSRange mRange = [[_textView string] rangeOfString:[self findString] options:([self ignoreCase])?(NSCaseInsensitiveSearch|NSBackwardsSearch):(NSLiteralSearch|NSBackwardsSearch) range:NSMakeRange(0, sRange.location)];
		
		if (mRange.location == NSNotFound) {
			NSBeep();
			return;
		}
		
		[_textView setSelectedRange:mRange];
		[_textView scrollRangeToVisible:mRange];
		[_textView showFindIndicatorForRange:mRange];
	}
}

- (IBAction)replace:(id)sender; {
	if (![self findString] ||
		[[self findString] length] == 0) {
		NSBeep();
		return;
	}
	
	if (![self replaceString])
		[self setReplaceString:@""];
	
	NSString *compareString = ([self ignoreCase])?[[self findString] lowercaseString]:[self findString];
	NSString *selectedString = ([self ignoreCase])?[[[[self textView] string] substringWithRange:[[self textView] selectedRange]] lowercaseString]:[[[self textView] string] substringWithRange:[[self textView] selectedRange]];
	if (![compareString isEqualToString:selectedString]) {
		NSBeep();
		return;
	}
	
	if ([[self textView] shouldChangeTextInRange:[[self textView] selectedRange] replacementString:[self replaceString]]) {
		[[self textView] replaceCharactersInRange:[[self textView] selectedRange] withString:[self replaceString]];
		[[self textView] didChangeText];
	}
	
	[self find:nil];
}
- (IBAction)replaceAndFind:(id)sender; {
	[self replace:nil];
	[self findNext:nil];
}
- (IBAction)replaceAll:(id)sender; {
	if (![self findString] || ![[self findString] length] || ![self replaceString]) {
		NSBeep();
		return;
	}
	
	NSArray *ranges = [_findRanges allRangesAsObjects];
	NSMutableArray *strings = [NSMutableArray arrayWithCapacity:[_findRanges count]];
	for (NSUInteger index = 0; index < [_findRanges count]; index++) {
		[strings addObject:[self replaceString]];
	}
	
	NSString *replaceString = [self replaceString];
	NSInteger locationShift = 0;
	if ([_textView shouldChangeTextInRanges:ranges replacementStrings:strings]) {
		[[_textView textStorage] beginEditing];
		for (NSValue *rValue in ranges) {
			NSRange oRange = [rValue rangeValue];
			
			[[_textView textStorage] replaceCharactersInRange:NSMakeRange(oRange.location + locationShift, oRange.length) withString:replaceString];
			
			locationShift += ([[self replaceString] length] - oRange.length);
		}
		[[_textView textStorage] endEditing];
	}
}

- (IBAction)changeFindScope:(NSMenuItem *)sender; {
	[self setFindScope:[sender tag]];
}
- (IBAction)changeFindSubScope:(NSMenuItem *)sender; {
	[self setFindSubScope:[sender tag]];
}

- (id)_initWithTextView:(WCTextView *)textView; {
	if (!(self = [super initWithNibName:@"WCFindBarView" bundle:nil]))
		return nil;
	
	_textView = textView;
	_ignoreCase = YES;
	_useRegularExpression = NO;
	_findScope = WCFindScopeFile;
	_findSubScope = WCFindSubScopeContains;
	_findRanges = [[WCMutableRangeArray alloc] init];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_textStorageDidProcessEditing:) name:NSTextStorageDidProcessEditingNotification object:[_textView textStorage]];
	
	return self;
}

- (void)_showFindBar; {
	[[[_textView fileViewController] view] addSubview:[self view] positioned:NSWindowBelow relativeTo:[[_textView fileViewController] topBarView]];
	NSRect topFrame = [[[_textView fileViewController] topBarView] frame];
	[[self view] setFrame:NSMakeRect(topFrame.origin.x, topFrame.origin.y, NSWidth(topFrame), NSHeight([[self view] frame]))];
	NSRect frame = [[self view] frame];
	_showAnimation = [[NSViewAnimation alloc] initWithDuration:0.25 animationCurve:NSAnimationEaseIn];
	NSRect scrollFrame = [[_textView enclosingScrollView] frame];
	
	[_showAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:[self view],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(frame.origin.x, frame.origin.y-frame.size.height, frame.size.width, frame.size.height)],NSViewAnimationEndFrameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:[_textView enclosingScrollView],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(scrollFrame.origin.x, scrollFrame.origin.y, scrollFrame.size.width, scrollFrame.size.height-frame.size.height)],NSViewAnimationEndFrameKey, nil], nil]];
	 
	[_showAnimation setDelegate:self];
	
	[_showAnimation startAnimation];
}

- (void)animationDidEnd:(NSAnimation *)animation {
	if (animation == _closeAnimation) {
		[self autorelease];
		[_closeAnimation release];
		_closeAnimation = nil;
		[[self view] removeFromSuperviewWithoutNeedingDisplay];
		[self _removeEverything];
		[_textView setFindBarViewController:nil];
		
		if ([[self findString] length] > 0) {
			NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSFindPboard];
			
			[pboard declareTypes:[NSArray arrayWithObjects:NSStringPboardType, nil] owner:nil];
			
			[pboard setString:[self findString] forType:NSStringPboardType];
		}
	}
	else if (animation == _showAnimation) {
		[[[self view] window] makeFirstResponder:_searchField];
		[_searchField setNextKeyView:_textView];
		[_showAnimation release];
		_showAnimation = nil;
		[_textView setFindBarViewController:self];
	}
	else {
		[self setReplaceControlsVisible:![self replaceControlsVisible]];
		[_toggleReplaceControlsAnimation release];
		_toggleReplaceControlsAnimation = nil;
		
		if ([self replaceControlsVisible]) {
			[_searchField setNextKeyView:_replaceTextField];
			[_replaceTextField setNextKeyView:_textView];
		}
		else {
			[_searchField setNextKeyView:_replaceTextField];
			if ([[[self view] window] firstResponder] == _replaceTextField)
				[[[self view] window] makeFirstResponder:_searchField];
		}
	}
}

- (void)_textStorageDidProcessEditing:(NSNotification *)note {
	[self performSelector:@selector(_removeEverything) withObject:nil afterDelay:0.0];
}

- (void)_removeEverything; {
	
	NSString *string = [_textView string];
	NSRange entireRange = NSMakeRange(0, [string length]);
	
	[[_textView layoutManager] removeTemporaryAttribute:NSBackgroundColorAttributeName forCharacterRange:entireRange];
	[[_textView layoutManager] removeTemporaryAttribute:NSUnderlineStyleAttributeName forCharacterRange:entireRange];
	[[_textView layoutManager] removeTemporaryAttribute:NSUnderlineColorAttributeName forCharacterRange:entireRange];
	
	[_findRanges removeAllRanges];
	_numberOfMatches = 0;
	[self setMatchesString:NSLocalizedString(@"No matches", @"No matches")];
}
@end
