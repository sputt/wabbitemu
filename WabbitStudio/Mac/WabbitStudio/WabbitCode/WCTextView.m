//
//  WCTextView.m
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTextView.h"
#import "WCFile.h"
#import "WCSyntaxHighlighter.h"
#import "WCSymbolScanner.h"
#import "WCLineHighlighter.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "WCSymbol.h"
#import "WCProject.h"
#import "WCFileViewController.h"
#import "WCBuildMessage.h"
#import "WCTextStorage.h"
#import "NSObject+WCExtensions.h"
#import "WCBuildTarget.h"
#import "WCFindBarViewController.h"
#import "WCFileViewController.h"
#import "WCFindInProjectViewController.h"
#import "WCGotoLineSheetController.h"
#import "NSTextView+WCExtensions.h"
#import "WCBreakpoint.h"
#import "WCFileWindowController.h"

// without this xcode complains about the restrict qualifiers in the regexkit header
#define restrict
#import <RegexKit/RegexKit.h>


@implementation WCTextView
#pragma mark *** Subclass Overrides ***
- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[self cleanupUserDefaultsObserving];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_syntaxHighlighter release];
	[_lineHighlighter release];
	_file = nil;
    [super dealloc];
}

- (id)initWithCoder:(NSCoder *)decoder {
	if (!(self = [super initWithCoder:decoder]))
		return nil;
	
	[[self layoutManager] setAllowsNonContiguousLayout:NO];
	
	_lineHighlighter = [[WCLineHighlighter alloc] initWithTextView:self];
	
	[self setupUserDefaultsObserving];
	
	return self;
}

- (void)awakeFromNib {
	[super awakeFromNib];
	
	[self toggleWrapLines:nil];
}

- (void)mouseDown:(NSEvent *)event {
	[super mouseDown:event];
	
	if ([event type] == NSLeftMouseDown &&
		[event clickCount] == 2 &&
		([event modifierFlags] & NSCommandKeyMask))
		[self jumpToDefinition:nil];
}

- (void)drawViewBackgroundInRect:(NSRect)rect {
	[super drawViewBackgroundInRect:rect];
	
	NSRect visibleRect = [self visibleRect];
	
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorDisplayErrorBadgesKey] &&
		[[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorErrorLineHighlightKey]) {
		for (WCBuildMessage *error in [_file allErrorMessages]) {
			NSRect lineRect = [[self layoutManager] lineFragmentRectForGlyphAtIndex:[_file lineStartForBuildMessage:error] effectiveRange:NULL];
			
			if (NSIntersectsRect(lineRect, visibleRect) && [self needsToDrawRect:lineRect]) {
				NSColor *baseColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEditorErrorLineHighlightColorKey];
				
				[[baseColor colorWithAlphaComponent:0.25] setFill];
				NSRectFillUsingOperation(lineRect, NSCompositeSourceOver);
				[[baseColor colorWithAlphaComponent:0.5] setFill];
				NSRectFillUsingOperation(NSMakeRect(lineRect.origin.x, lineRect.origin.y, lineRect.size.width, 1.0), NSCompositeSourceOver);
				NSRectFillUsingOperation(NSMakeRect(lineRect.origin.x, lineRect.origin.y+lineRect.size.height - 1.0, lineRect.size.width, 1.0), NSCompositeSourceOver);
			}
		}
	}
	
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorDisplayWarningBadgesKey] &&
		[[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorWarningLineHighlightKey]) {
		for (WCBuildMessage *warning in [_file allWarningMessages]) {
			NSRect lineRect = [[self layoutManager] lineFragmentRectForGlyphAtIndex:[_file lineStartForBuildMessage:warning] effectiveRange:NULL];
			
			if (NSIntersectsRect(lineRect, visibleRect) && [self needsToDrawRect:lineRect]) {
				NSColor *baseColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEditorWarningLineHighlightColorKey];
				
				[[baseColor colorWithAlphaComponent:0.25] setFill];
				NSRectFillUsingOperation(lineRect, NSCompositeSourceOver);
				[[baseColor colorWithAlphaComponent:0.5] setFill];
				NSRectFillUsingOperation(NSMakeRect(lineRect.origin.x, lineRect.origin.y, lineRect.size.width, 1.0), NSCompositeSourceOver);
				NSRectFillUsingOperation(NSMakeRect(lineRect.origin.x, lineRect.origin.y+lineRect.size.height - 1.0, lineRect.size.width, 1.0), NSCompositeSourceOver);
			}
		}
	}
	
	if (![[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesCurrentLineHighlightKey])
		return;
	
	NSRange range = [self selectedRange];
	
	if (range.length)
		return;
	else if (range.location >= [[self string] length])
		range.location = [[self string] length] - 1;
	
	NSUInteger rectCount = 0;
	NSRectArray newRects = [[self layoutManager] rectArrayForCharacterRange:[[self string] lineRangeForRange:range] withinSelectedCharacterRange:NSMakeRange(NSNotFound, 0) inTextContainer:[self textContainer] rectCount:&rectCount];
	NSRect newRect = NSZeroRect;
	
	if (rectCount != 0) {
		newRect = newRects[0];
		
		newRect.origin.x = NSMinX([self bounds]);
		newRect.size.width = NSWidth([self bounds]);
		
		[[[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesCurrentLineHighlightColorKey] setFill];
		NSRectFillUsingOperation(newRect, NSCompositeSourceOver);
	}
}

- (NSArray *)completionsForPartialWordRange:(NSRange)charRange indexOfSelectedItem:(NSInteger *)index {
	NSArray *symbols = ([[self file] project] == nil)?[[[self file] symbolScanner] symbols]:[[[self file] project] symbols];
	NSMutableArray *csymbols = [NSMutableArray array];
	NSString *prefix = [[self string] substringWithRange:charRange];
	NSString *lowercasePrefix = [prefix lowercaseString];
	BOOL symbolsAreCaseSensitive = [[[[self file] project] activeBuildTarget] symbolsAreCaseSensitive];
	
	for (WCSymbol *symbol in symbols) {
		if (symbolsAreCaseSensitive && [[symbol name] hasPrefix:prefix])
			[csymbols addObject:[symbol name]];
		else if ([[[symbol name] lowercaseString] hasPrefix:lowercasePrefix])
			[csymbols addObject:[symbol name]];
	}
	
	[csymbols sortUsingSelector:@selector(compare:)];
	
	return [[csymbols copy] autorelease];
}
/*
- (void)insertCompletion:(NSString *)word forPartialWordRange:(NSRange)charRange movement:(NSInteger)movement isFinal:(BOOL)flag {
	if (flag && (movement == NSReturnTextMovement || movement == NSTabTextMovement))
		[super insertCompletion:word forPartialWordRange:charRange movement:movement isFinal:flag];
}
 */
 
#pragma mark IBActions
- (IBAction)insertNewline:(id)sender {
	[super insertNewline:sender];
	
	if (![[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorAutomaticallyIndentNewLinesKey])
		return;
	
	NSString *previousLineWhitespaceString = nil;
	NSScanner *previousLineScanner = [[[NSScanner alloc] initWithString:[[self string] substringWithRange:[[self string] lineRangeForRange:NSMakeRange([self selectedRange].location - 1, 0)]]] autorelease];
	[previousLineScanner setCharactersToBeSkipped:nil];
	
	if ([previousLineScanner scanCharactersFromSet:[NSCharacterSet whitespaceCharacterSet] intoString:&previousLineWhitespaceString]) {
		[self insertText:previousLineWhitespaceString];
	}
}

- (IBAction)insertTab:(id)sender {
	[super insertTab:sender];
}

- (IBAction)performFindPanelAction:(id)sender {
	[WCFindBarViewController presentFindBarForTextView:self];
}

- (void)paste:(id)sender {
	[self pasteAsPlainText:sender];
}

- (void)pasteAsRichText:(id)sender {
	[self pasteAsPlainText:sender];
}

#pragma mark *** Protocol Overrides ***
#pragma mark NSKeyValueObserving
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	
	if ([(NSString *)context isEqualToString:kWCPreferencesEditorFontKey]) {
		
	}
	else if ([(NSString *)context isEqualToString:kWCPreferencesEditorTextColorKey])
		[self setTextColor:[[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEditorTextColorKey]];
	else if ([(NSString *)context isEqualToString:kWCPreferencesEditorBackgroundColorKey])
		[self setBackgroundColor:[[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEditorBackgroundColorKey]];
	else if ([(NSString *)context isEqualToString:kWCPreferencesEditorErrorLineHighlightKey] ||
			 [(NSString *)context isEqualToString:kWCPreferencesEditorErrorLineHighlightColorKey] ||
			 [(NSString *)context isEqualToString:kWCPreferencesEditorWarningLineHighlightKey] ||
			 [(NSString *)context isEqualToString:kWCPreferencesEditorWarningLineHighlightColorKey] ||
			 [(NSString *)context isEqualToString:kWCPreferencesEditorDisplayErrorBadgesKey] ||
			 [(NSString *)context isEqualToString:kWCPreferencesEditorDisplayWarningBadgesKey])
		[self setNeedsDisplayInRect:[self visibleRect]];
	else if ([(NSString *)context isEqualToString:kWCPreferencesEditorWrapLinesKey])
		[self toggleWrapLines:nil];
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}
#pragma mark *** Category Overrides ***
- (NSArray *)userDefaultsKeys {
	return [NSArray arrayWithObjects:kWCPreferencesEditorTextColorKey,kWCPreferencesEditorBackgroundColorKey,kWCPreferencesEditorErrorLineHighlightKey,kWCPreferencesEditorErrorLineHighlightColorKey,kWCPreferencesEditorWarningLineHighlightKey,kWCPreferencesEditorWarningLineHighlightColorKey,kWCPreferencesEditorWrapLinesKey,kWCPreferencesEditorDisplayErrorBadgesKey,kWCPreferencesEditorDisplayWarningBadgesKey, nil];
}
#pragma mark *** Public Methods ***
- (void)jumpToSymbol:(WCSymbol *)symbol; {
	WCFileViewController *controller = [[[self file] project] addFileViewControllerForFile:[symbol file] inTabViewContext:[[[self file] project] currentTabViewContext]];
	WCTextView *textView = [controller textView];
	
	if (textView == nil)
		textView = self;
	
#ifdef DEBUG
	NSAssert(textView != nil, @"cannot jump to a symbol without a text view!");
#endif
	
	[textView setSelectedRangeSafely:[symbol symbolRange] scrollRangeToVisible:YES];
}
#pragma mark Accessors
@dynamic file;
- (WCFile *)file {
	return _file;
}
- (void)setFile:(WCFile *)file {
	if (_file == file)
		return;
	
	[_syntaxHighlighter release];
	
	_file = file;
	
	_syntaxHighlighter = [[WCSyntaxHighlighter alloc] initWithTextView:self];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileNumberOfBuildMessagesChanged:) name:kWCFileNumberOfErrorMessagesChangedNotification object:_file];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileNumberOfBuildMessagesChanged:) name:kWCFileNumberOfWarningMessagesChangedNotification object:_file];
}
@dynamic currentSymbolString;
- (NSString *)currentSymbolString {
	NSString *string = [self string];
	NSRange selectedRange = [self selectedRange];
	NSRange lineRange = [string lineRangeForRange:selectedRange];
	NSString *lineString = [string substringWithRange:lineRange];
	
	// search the line string for anything that looks like a symbol name
	RKEnumerator *lineEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterSymbolsRegex string:lineString] autorelease];
	
	while ([lineEnum nextRanges] != NULL) {
		NSRangePointer matchRange = [lineEnum currentRanges];

		if (NSLocationInRange(selectedRange.location, NSMakeRange(lineRange.location + matchRange->location, matchRange->length)))
			return [lineString substringWithRange:*matchRange];
	}
	return nil;
}
@synthesize fileViewController=_fileViewController;
@synthesize syntaxHighlighter=_syntaxHighlighter;
#pragma mark IBActions
- (IBAction)jumpToDefinition:(id)sender; {
	NSString *symbolString = [[self currentSymbolString] lowercaseString];
	
	if (!symbolString) {
		NSBeep();
		return;
	}
	
	NSArray *symbols = ([[self file] project] == nil)?[[[self file] symbolScanner] symbolsForSymbolName:symbolString]:[[[self file] project] symbolsForSymbolName:symbolString];
	
	if (![symbols count]) {
		NSBeep();
		return;
	}
	else if ([symbols count] == 1)
		[self jumpToSymbol:[symbols lastObject]];
	else {
		NSMenu *menu = [[[NSMenu alloc] initWithTitle:@""] autorelease];
		
		[menu setShowsStateColumn:NO];
		[menu setFont:[NSFont menuFontOfSize:11.0]];
		
		for (WCSymbol *symbol in symbols) {
			NSMenuItem *item = [menu addItemWithTitle:[NSString stringWithFormat:@"%@ - %@:%lu",[symbol name],[[symbol file] name],[[[symbol file] textStorage] lineNumberForCharacterIndex:[symbol symbolRange].location]+1] action:@selector(_jumpToSymbolFromMenu:) keyEquivalent:@""];
			[item setImage:[symbol iconForContextualMenu]];
			[item setRepresentedObject:symbol];
		}
		
		NSRect lineRect = [[self layoutManager] lineFragmentRectForGlyphAtIndex:[self selectedRange].location effectiveRange:NULL];
		NSPoint selectedPoint = [[self layoutManager] locationForGlyphAtIndex:[self selectedRange].location];
		
		lineRect.origin.y += lineRect.size.height;
		lineRect.origin.x += selectedPoint.x;
		
		[menu popUpMenuPositioningItem:nil atLocation:lineRect.origin inView:self];
	}
}

- (IBAction)commentOrUncomment:(id)sender; {
	NSRange range = [self selectedRange];
	NSRange lineRange = [[self string] lineRangeForRange:range];
	
	if (!range.length) {
		NSString *string = [[self string] substringWithRange:lineRange];
		
		if ([string rangeOfString:@";" options:NSLiteralSearch].location == NSNotFound) {
			NSString *replacementString = [@";;" stringByAppendingString:string];
			
			if ([self shouldChangeTextInRange:lineRange replacementString:replacementString]) {
				[self replaceCharactersInRange:lineRange withString:replacementString];
				[self setSelectedRange:NSMakeRange(range.location + 2, range.length)];
			}
		}
		else {
			NSMutableString *replacementString = [NSMutableString stringWithString:string];
			
			[replacementString replaceOccurrencesOfString:@";" withString:@"" options:NSLiteralSearch range:NSMakeRange(0, [replacementString length])];
			
			if ([self shouldChangeTextInRange:lineRange replacementString:replacementString]) {
				[self replaceCharactersInRange:lineRange withString:replacementString];
				[self didChangeText];
				[self setSelectedRange:NSMakeRange(range.location-([string length] - [replacementString length]), range.length)];
			}
		}
	}
	else {
		NSString *string = [[self string] substringWithRange:lineRange];
		
		RKRegex *mRegex = [[[RKRegex alloc] initWithRegexString:@"^\\s*;+.*" options:RKCompileUTF8|RKCompileMultiline] autorelease];
		if ([string rangeOfRegex:mRegex].location == NSNotFound) {
			RKRegex *regex = [[[RKRegex alloc] initWithRegexString:@"^(?<first>\\s*)(?<second>.*)" options:RKCompileUTF8|RKCompileMultiline] autorelease];
			NSMutableString *newString = [NSMutableString stringWithString:string];
			[newString match:regex replace:RKReplaceAll withString:@"${first};;${second}"];
			
			if ([self shouldChangeTextInRange:lineRange replacementString:newString]) {
				[self replaceCharactersInRange:lineRange withString:newString];
				[self didChangeText];
				[self setSelectedRange:NSMakeRange(lineRange.location, [newString length])];
			}
		}
		else {
			RKRegex *regex = [[[RKRegex alloc] initWithRegexString:@"^(?<first>\\s*)(?<comment>;+)(?<second>.*)" options:RKCompileUTF8|RKCompileMultiline] autorelease];
			NSMutableString *newString = [NSMutableString stringWithString:string];
			[newString match:regex replace:RKReplaceAll withString:@"${first}${second}"];
			
			if ([self shouldChangeTextInRange:lineRange replacementString:newString]) {
				[self replaceCharactersInRange:lineRange withString:newString];
				[self didChangeText];
				[self setSelectedRange:NSMakeRange(lineRange.location, [newString length])];
			}
		}
	}
}
- (IBAction)blockCommentOrUncomment:(id)sender; {
	NSRange range = [[self string] lineRangeForRange:[self selectedRange]];
	NSString *string = [[self string] substringWithRange:range];
	
	// the selection does not have a matching block comment
	if ([string rangeOfString:@"#comment" options:NSLiteralSearch].location == NSNotFound && [string rangeOfString:@"#endcomment" options:NSLiteralSearch].location == NSNotFound) {
		NSMutableString *newString = [NSMutableString stringWithString:@"#comment\n"];
		[newString appendString:string];
		[newString appendString:@"#endcomment\n"];
		
		if ([self shouldChangeTextInRange:range replacementString:newString]) {
			[self replaceCharactersInRange:range withString:newString];
			[self didChangeText];
			[self setSelectedRange:NSMakeRange(range.location, [newString length])];
		}
	}
	else {
		NSMutableString *newString = [NSMutableString stringWithString:string];
		[newString replaceOccurrencesOfString:@"#comment\n" withString:@"" options:NSLiteralSearch range:NSMakeRange(0, [newString length])];
		[newString replaceOccurrencesOfString:@"#endcomment\n" withString:@"" options:NSLiteralSearch range:NSMakeRange(0, [newString length])];
		[newString replaceOccurrencesOfString:@"#comment" withString:@"" options:NSLiteralSearch range:NSMakeRange(0, [newString length])];
		[newString replaceOccurrencesOfString:@"#endcomment" withString:@"" options:NSLiteralSearch range:NSMakeRange(0, [newString length])];
		
		if ([self shouldChangeTextInRange:range replacementString:newString]) {
			[self replaceCharactersInRange:range withString:newString];
			[self didChangeText];
			[self setSelectedRange:NSMakeRange(range.location, [newString length])];
		}
	}
}

- (IBAction)shiftLeft:(id)sender; {
	NSRange range = [self selectedRange];
	NSRange lineRange = [[self string] lineRangeForRange:range];
	NSString *string = [[self string] substringWithRange:lineRange];
	NSMutableString *newString = [NSMutableString stringWithString:string];
	
	if (!range.length) {
		RKRegex *regex = [[[RKRegex alloc] initWithRegexString:@"^(\t)(.*)" options:RKCompileNoOptions] autorelease];
		
		[newString match:regex replace:RKReplaceAll withString:@"$2"];
		
		if ([self shouldChangeTextInRange:lineRange replacementString:newString]) {
			[self replaceCharactersInRange:lineRange withString:newString];
			[self didChangeText];
			[self setSelectedRange:NSMakeRange(range.location-1, 0)];
		}
	}
	else {
		RKRegex *regex = [[[RKRegex alloc] initWithRegexString:@"^(\t)(.*)" options:RKCompileMultiline] autorelease];
		
		[newString match:regex replace:RKReplaceAll withString:@"$2"];
		
		if ([self shouldChangeTextInRange:lineRange replacementString:newString]) {
			[self replaceCharactersInRange:lineRange withString:newString];
			[self didChangeText];
			[self setSelectedRange:NSMakeRange(lineRange.location, [newString length])];
		}
	}
}

- (IBAction)shiftRight:(id)sender; {
	NSRange range = [self selectedRange];
	NSRange lineRange = [[self string] lineRangeForRange:range];
	NSString *string = [[self string] substringWithRange:lineRange];
	
	if (!range.length) {
		NSMutableString *newString = [NSMutableString stringWithString:@"\t"];
		[newString appendString:string];
		
		if ([self shouldChangeTextInRange:lineRange replacementString:newString]) {
			[self replaceCharactersInRange:lineRange withString:newString];
			[self didChangeText];
			[self setSelectedRange:NSMakeRange(range.location+1, 0)];
		}
	}
	else {
		NSMutableString *newString = [NSMutableString stringWithString:string];
		RKRegex *regex = [[[RKRegex alloc] initWithRegexString:@"^(.*)" options:RKCompileMultiline] autorelease];
		
		[newString match:regex replace:RKReplaceAll withString:@"\t$1"];
		
		if ([self shouldChangeTextInRange:lineRange replacementString:newString]) {
			[self replaceCharactersInRange:lineRange withString:newString];
			[self didChangeText];
			[self setSelectedRange:NSMakeRange(lineRange.location, [newString length])];
		}
	}
}
- (IBAction)useSelectionForFindInProject:(id)sender; {
	if (![[self file] project] || ![self selectedRange].length) {
		NSBeep();
		return;
	}
	
	WCProject *project = [[self file] project];
	
	[project viewSearch:nil];
	[[project findInProjectViewController] setFindString:[[self string] substringWithRange:[self selectedRange]]];
	[[project findInProjectViewController] findInProject:nil];
}
- (IBAction)toggleWrapLines:(id)sender; {
	NSTextView *textView = self;
	NSScrollView *textScrollView = [self enclosingScrollView];
	NSSize contentSize = [textScrollView contentSize];
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorWrapLinesKey]) {
		[textView setMinSize:contentSize];
		[textScrollView setHasHorizontalScroller:NO];
		[textView setHorizontallyResizable:YES];
		[[textView textContainer] setWidthTracksTextView:YES];
		[[textView textContainer] setContainerSize:NSMakeSize(contentSize.width, CGFLOAT_MAX)];
	}
	else {
		[textView setMinSize:contentSize];
		[textScrollView setHasHorizontalScroller:YES];
		[textView setHorizontallyResizable:YES];
		[[textView textContainer] setContainerSize:NSMakeSize(CGFLOAT_MAX, CGFLOAT_MAX)];
		[[textView textContainer] setWidthTracksTextView:NO];
	}
}
- (IBAction)gotoLine:(id)sender; {
	[WCGotoLineSheetController presentGotoLineSheetForTextView:self];
}

- (IBAction)openInSeparateEditor:(id)sender; {
	if ([_file project] == nil || [[[self window] windowController] isKindOfClass:[WCFileWindowController class]]) {
		NSBeep();
		return;
	}
	
	WCFileWindowController *controller = [WCFileWindowController fileWindowControllerWithFile:_file];
	
	[[_file project] addWindowController:controller];
	
	[controller showWindow:nil];
}
#pragma mark *** Private Methods ***
#pragma mark IBActions
- (IBAction)_jumpToSymbolFromMenu:(id)sender {
	[self jumpToSymbol:[sender representedObject]];
}
#pragma mark Notifications
- (void)_fileNumberOfBuildMessagesChanged:(NSNotification *)note {
	[self setNeedsDisplay:YES];
}

@end
