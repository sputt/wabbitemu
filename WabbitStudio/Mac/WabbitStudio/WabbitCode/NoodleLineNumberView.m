//
//  NoodleLineNumberView.m
//  Line View Test
//
//  Created by Paul Kim on 9/28/08.
//  Copyright (c) 2008 Noodlesoft, LLC. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//

#import "NoodleLineNumberView.h"

#import "WCFile.h"
#import "WCTextView.h"
#import "WCTextStorage.h"
#import "WCBuildMessage.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "NSObject+WCExtensions.h"
#import "WCBreakpoint.h"
#import "WCGeneralPerformer.h"
#import "WCProject.h"
#import "WCBreakpointsViewController.h"
#import "WCDefines.h"
#import "NSAlert-OAExtensions.h"
#import "MAAttachedWindow.h"
#import "WCBreakpointEditViewController.h"

#define DEFAULT_THICKNESS 22.0
#define RULER_MARGIN 3.0
#define BADGE_THICKNESS 12.0
#define CORNER_RADIUS 3.0
#define BREAKPOINT_HEIGHT 12.0

@interface NoodleLineNumberView (Private)

- (NSArray *)lineIndices;
- (NSUInteger)lineNumberForCharacterIndex:(NSUInteger)index inText:(NSString *)text;
- (NSDictionary *)textAttributes;
- (NSDictionary *)markerTextAttributes;
- (NSUInteger)lineNumberForLocation:(CGFloat)location;

@end

@implementation NoodleLineNumberView
#pragma mark *** Subclass Overrides ***
- (id)initWithScrollView:(NSScrollView *)aScrollView
{
    if ((self = [super initWithScrollView:aScrollView orientation:NSVerticalRuler]) != nil)
    {
		//linesToMarkers = [[NSMutableDictionary alloc] init];
		
        [self setClientView:[aScrollView documentView]];
		
		[self setupNotificationObserving];
		[self setupUserDefaultsObserving];
    }
    return self;
}

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[self cleanupUserDefaultsObserving];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
	_currentEditViewController = nil;
    [super dealloc];
}

- (NSArray *)userDefaultsKeys {
	return [NSArray arrayWithObjects:kWCPreferencesEditorErrorLineHighlightKey,kWCPreferencesEditorErrorLineHighlightColorKey,kWCPreferencesEditorDisplayErrorBadgesKey,kWCPreferencesEditorWarningLineHighlightKey,kWCPreferencesEditorWarningLineHighlightColorKey,kWCPreferencesEditorDisplayWarningBadgesKey, nil];
}

- (NSArray *)notificationDictionaries {
	return [NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_fileNumberOfBuildMessagesChanged:)),kNSObjectSelectorKey,kWCFileNumberOfErrorMessagesChangedNotification,kNSObjectNotificationNameKey,[(WCTextView *)[self clientView] file],kNSObjectNotificationObjectKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_fileNumberOfBuildMessagesChanged:)),kNSObjectSelectorKey,kWCFileNumberOfWarningMessagesChangedNotification,kNSObjectNotificationNameKey,[(WCTextView *)[self clientView] file],kNSObjectNotificationObjectKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_fileNumberOfBreakpointsDidChange:)),kNSObjectSelectorKey,kWCFileNumberOfBreakpointsDidChangeNotification,kNSObjectNotificationNameKey,[(WCTextView *)[self clientView] file],kNSObjectNotificationObjectKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_breakpointIsActiveDidChange:)),kNSObjectSelectorKey,kWCBreakpointIsActiveDidChangeNotification,kNSObjectNotificationNameKey, nil], nil];
}

- (void)setClientView:(NSView *)aView {
	id		oldClientView = [self clientView];
	
    if ((oldClientView != aView) && [oldClientView isKindOfClass:[NSTextView class]]) {
		[[NSNotificationCenter defaultCenter] removeObserver:self name:NSTextStorageDidProcessEditingNotification object:[(NSTextView *)oldClientView textStorage]];
    }
    [super setClientView:aView];
    if ((aView != nil) && [aView isKindOfClass:[NSTextView class]]) {
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textDidChange:) name:NSTextStorageDidProcessEditingNotification object:[(NSTextView *)aView textStorage]];

    }
}

- (CGFloat)requiredThickness {
    NSUInteger			lineCount, digits, i;
    NSMutableString     *sampleString;
    NSSize              stringSize;
    
    lineCount = [[self lineIndices] count];
    digits = (NSUInteger)log10(lineCount) + 1;
	sampleString = [NSMutableString string];
    for (i = 0; i < digits; i++)
    {
        // Use "8" since it is one of the fatter numbers. Anything but "1"
        // will probably be ok here. I could be pedantic and actually find the fattest
		// number for the current font but nah.
        [sampleString appendString:@"8"];
    }
    
    stringSize = [sampleString sizeWithAttributes:[self textAttributes]];
	
	//CGFloat badgeThickness = ([[(WCTextView *)[self clientView] file] project] == nil)?0.0:BADGE_THICKNESS;
	CGFloat badgeThickness = BADGE_THICKNESS;
	CGFloat defaultThickness = DEFAULT_THICKNESS + badgeThickness;
	CGFloat reqThickness = stringSize.width + (RULER_MARGIN * 2) + badgeThickness;
	
	// Round up the value. There is a bug on 10.4 where the display gets all wonky when scrolling if you don't
	// return an integral value here.
    return ceilf(MAX(defaultThickness, reqThickness));
}
#pragma mark Tooltips
- (NSString *)view:(NSView *)view stringForToolTip:(NSToolTipTag)tag point:(NSPoint)point userData:(void *)data {
	WCBuildMessage *message = (WCBuildMessage *)data;
	
	return [message name];
}
#pragma mark Mouse Handling
- (void)mouseDown:(NSEvent *)theEvent {
	if ([[(WCTextView *)[self clientView] file] project] == nil)
		return;
	
	NSUInteger startLineNumber = [self lineNumberForLocation:[self convertPointFromBase:[theEvent locationInWindow]].y];
	
	if (startLineNumber == NSNotFound)
		return;
	
	WCFile *file = [(WCTextView *)[self clientView] file];
	WCBreakpoint *startBreakpoint = [file breakpointAtLineNumber:startLineNumber];
	if (startBreakpoint == nil) {
		startBreakpoint = [WCBreakpoint breakpointWithLineNumber:startLineNumber inFile:file];
		[file addBreakpoint:startBreakpoint];
		return;
	}
	
	// create a pool to flush each time through the cycle
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	// track!
	NSEvent *event = nil;
	BOOL didChangeLineNumber = NO;
	while([event type] != NSLeftMouseUp) {
		[pool drain];
		pool = [[NSAutoreleasePool alloc] init];
		
		event = [[self window] nextEventMatchingMask: NSLeftMouseDraggedMask | NSLeftMouseUpMask];
		
		NSPoint currentPoint = [self convertPointFromBase:[event locationInWindow]];
		NSUInteger currentLineNumber = [self lineNumberForLocation:currentPoint.y];
		WCBreakpoint *currentBreakpoint = [[(WCTextView *)[self clientView] file] breakpointAtLineNumber:currentLineNumber];
		
		if (!didChangeLineNumber && startLineNumber != currentLineNumber)
			didChangeLineNumber = YES;
		
		if (NSMouseInRect(currentPoint, [self bounds], [self isFlipped])) {
			if (currentBreakpoint == nil) {
				[startBreakpoint retain];
				[file removeBreakpoint:startBreakpoint];
				[startBreakpoint setLineNumber:currentLineNumber];
				[file addBreakpoint:startBreakpoint];
				[startBreakpoint release];
			}
			else if ([event type] == NSLeftMouseUp && !didChangeLineNumber)
				[startBreakpoint setIsActive:![startBreakpoint isActive]];
			
			[[NSCursor arrowCursor] set];
		}
		else if ([event type] == NSLeftMouseUp && startBreakpoint != nil) {
			[file removeBreakpoint:startBreakpoint];
			NSShowAnimationEffect(NSAnimationEffectDisappearingItemDefault, [NSEvent mouseLocation], NSZeroSize, NULL, NULL, NULL);
		}
		else
			[[NSCursor disappearingItemCursor] set];
	}
	
	[pool drain];
}
#pragma mark Drawing
- (void)drawHashMarksAndLabelsInRect:(NSRect)aRect {
	NSRect bounds = [self bounds];
	
	[[self backgroundColor] set];
	NSRectFill(bounds);
	
	[[NSColor colorWithCalibratedWhite:0.58 alpha:1.0] set];
	[NSBezierPath strokeLineFromPoint:NSMakePoint(NSMaxX(bounds) - 0.5, NSMinY(bounds)) toPoint:NSMakePoint(NSMaxX(bounds) - 0.5, NSMaxY(bounds))];
	
    id view = [self clientView];
	
	if (view == nil)
		return;
	
	[self removeAllToolTips];
	
	NSLayoutManager *layoutManager = [view layoutManager];
	NSTextContainer	*container = [view textContainer];
	NSRect visibleRect = [[[self scrollView] contentView] bounds];
	NSRange	glyphRange = [layoutManager glyphRangeForBoundingRect:visibleRect inTextContainer:container], nullRange = NSMakeRange(NSNotFound, 0);
	NSRange range = [layoutManager characterRangeForGlyphRange:glyphRange actualGlyphRange:NULL];
	CGFloat	yinset = [view textContainerInset].height;
	NSDictionary *textAttributes = [self textAttributes], *currentTextAttributes;
	NSArray	*lines = [self lineIndices];
	NSUInteger index = 0, count = [lines count];
	
	// Fudge the range a tad in case there is an extra new line at end.
	// It doesn't show up in the glyphs so would not be accounted for.
	range.length++;
	
	BOOL showErrorBadges = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorDisplayErrorBadgesKey];
	BOOL errorLineHighlight = ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorErrorLineHighlightKey] && showErrorBadges);
	BOOL showWarningBadges = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorDisplayWarningBadgesKey];
	BOOL warningLineHighlight = ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorWarningLineHighlightKey] && showWarningBadges);
	
	for (NSUInteger line = [self lineNumberForCharacterIndex:range.location inText:nil]; line < count; line++) {
		index = [[lines objectAtIndex:line] unsignedIntegerValue];
		
		if (NSLocationInRange(index, range)) {
			NSUInteger rectCount = 0;
			NSRectArray rects = [layoutManager rectArrayForCharacterRange:NSMakeRange(index, 0)
											 withinSelectedCharacterRange:nullRange
														  inTextContainer:container
																rectCount:&rectCount];
			
			if (rectCount > 0) {
				// Note that the ruler view is only as tall as the visible
				// portion. Need to compensate for the clipview's coordinates.
				CGFloat ypos = yinset + NSMinY(rects[0]) - NSMinY(visibleRect);
				// Line numbers are internally stored starting at 0
				NSString *labelText = [NSString stringWithFormat:@"%d", line + 1];
				
				NSSize stringSize = [labelText sizeWithAttributes:textAttributes];
				
				WCBreakpoint *breakpoint = [[(WCTextView *)[self clientView] file] breakpointAtLineNumber:line];
				if (breakpoint != nil) {
					if ([breakpoint isActive])
						currentTextAttributes = [self markerTextAttributes];
					else
						currentTextAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[self font],NSFontAttributeName,[NSColor textColor],NSForegroundColorAttributeName, nil];
					
					NSRect bRect = NSMakeRect(NSMinX(bounds), ypos, NSWidth(bounds), NSHeight(rects[0]));
					
					[[WCGeneralPerformer sharedPerformer] drawBreakpoint:breakpoint inRect:bRect];
				}
				else
					currentTextAttributes = textAttributes;
				
				if (showErrorBadges) {
					WCBuildMessage *error = [[(WCTextView *)[self clientView] file] errorMessageAtLineNumber:line];
					
					if (error) {
						if (errorLineHighlight) {
							NSRect lineRect = NSMakeRect(bounds.origin.x,ypos,NSWidth(bounds),NSHeight(rects[0]));
							NSColor *baseColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEditorErrorLineHighlightColorKey];
							
							[[baseColor colorWithAlphaComponent:0.25] setFill];
							NSRectFillUsingOperation(lineRect, NSCompositeSourceOver);
							[[baseColor colorWithAlphaComponent:0.5] setFill];
							NSRectFillUsingOperation(NSMakeRect(lineRect.origin.x, lineRect.origin.y, lineRect.size.width, 1.0), NSCompositeSourceOver);
							NSRectFillUsingOperation(NSMakeRect(lineRect.origin.x, lineRect.origin.y+lineRect.size.height - 1.0, lineRect.size.width, 1.0), NSCompositeSourceOver);
						}
						
						NSRect drawRect = NSMakeRect(bounds.origin.x,ypos + (NSHeight(rects[0]) - BADGE_THICKNESS)/2.0,BADGE_THICKNESS, BADGE_THICKNESS);
						drawRect.size.width = drawRect.size.height = BADGE_THICKNESS;
						
						NSImage *icon = [error icon];
						[icon setSize:NSMakeSize(BADGE_THICKNESS, BADGE_THICKNESS)];
						
						[icon drawInRect:drawRect fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
						
						[self addToolTipRect:drawRect owner:self userData:(void *)error];
					}
				}
				
				if (showWarningBadges) {
					WCBuildMessage *warning = [[(WCTextView *)[self clientView] file] warningMessageAtLineNumber:line];
					
					if (warning) {
						if (warningLineHighlight) {
							NSRect lineRect = NSMakeRect(bounds.origin.x,ypos,NSWidth(bounds),NSHeight(rects[0]));
							NSColor *baseColor = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEditorWarningLineHighlightColorKey];
							
							[[baseColor colorWithAlphaComponent:0.25] setFill];
							NSRectFillUsingOperation(lineRect, NSCompositeSourceOver);
							[[baseColor colorWithAlphaComponent:0.5] setFill];
							NSRectFillUsingOperation(NSMakeRect(lineRect.origin.x, lineRect.origin.y, lineRect.size.width, 1.0), NSCompositeSourceOver);
							NSRectFillUsingOperation(NSMakeRect(lineRect.origin.x, lineRect.origin.y+lineRect.size.height - 1.0, lineRect.size.width, 1.0), NSCompositeSourceOver);
						}
						
						NSRect drawRect = NSMakeRect(bounds.origin.x,ypos + (NSHeight(rects[0]) - BADGE_THICKNESS)/2.0,BADGE_THICKNESS, BADGE_THICKNESS);
						drawRect.size.width = drawRect.size.height = BADGE_THICKNESS;
						
						NSImage *icon = [warning icon];
						[icon setSize:NSMakeSize(BADGE_THICKNESS, BADGE_THICKNESS)];
						
						[icon drawInRect:drawRect fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
						
						[self addToolTipRect:drawRect owner:self userData:(void *)warning];
					}
				}
				
				// Draw string flush right, centered vertically within the line
				[labelText drawInRect:
				 NSMakeRect(NSWidth(bounds) - stringSize.width - RULER_MARGIN,
							ypos + floor(NSHeight(rects[0])/2.0) - floor(stringSize.height / 2.0),
							NSWidth(bounds) - RULER_MARGIN * 2.0, NSHeight(rects[0]))
					   withAttributes:currentTextAttributes];
			}
		}
		
		if (index > NSMaxRange(range))
			break;
	}
}
#pragma mark Menus
- (NSMenu *)menu {
	if ([[(WCTextView *)[self clientView] file] project] == nil)
		return nil;
	
	NSMenu *mMenu = [[[NSMenu alloc] initWithTitle:@""] autorelease];
	
	[mMenu addItemWithTitle:NSLocalizedString(@"Edit Breakpoint\u2026", @"Edit Breakpoint with ellipsis") action:@selector(_editBreakpoint:) keyEquivalent:@""];
	[mMenu addItemWithTitle:NSLocalizedString(@"Disable Breakpoint", @"Disable Breakpoint") action:@selector(_toggleBreakpoint:) keyEquivalent:@""];
	[mMenu addItem:[NSMenuItem separatorItem]];
	[mMenu addItemWithTitle:NSLocalizedString(@"Delete Breakpoint", @"Delete Breakpoint") action:@selector(_deleteBreakpoint:) keyEquivalent:@""];
	[mMenu addItemWithTitle:[NSString stringWithFormat:NSLocalizedString(@"Delete All Breakpoints in \"%@\"\u2026", @"Delete All Breakpoints in File with ellipsis"),[[(WCTextView *)[self clientView] file] name]] action:@selector(_deleteAllBreakpoints:) keyEquivalent:@""];
	[mMenu addItem:[NSMenuItem separatorItem]];
	[mMenu addItemWithTitle:NSLocalizedString(@"Reveal in Breakpoints View", @"Reveal in Breakpoints View") action:@selector(_revealInBreakpointsView:) keyEquivalent:@""];
	
	return mMenu;
}

- (NSMenu *)menuForEvent:(NSEvent *)event {
	NSMenu *menu = [super menuForEvent:event];
	
	if (menu != nil) {
		NSPoint p = [self convertPointFromBase:[event locationInWindow]];
		NSUInteger line = [self lineNumberForLocation:p.y];
		_locationForContextualMenu = p.y;
		_lineNumberForContextualMenu = line;
		_breakpointForContextualMenu = [[(WCTextView *)[self clientView] file] breakpointAtLineNumber:line];
	}
	else
		_breakpointForContextualMenu = nil;
	
	return menu;
}
#pragma mark NSMenuValidation
- (BOOL)validateMenuItem:(NSMenuItem *)item {
	if ([item action] == @selector(_deleteBreakpoint:)) {
		if (_breakpointForContextualMenu == nil)
			return NO;
	}
	else if ([item action] == @selector(_deleteAllBreakpoints:)) {
		if ([[[(WCTextView *)[self clientView] file] allBreakpoints] count] == 0)
			return NO;
	}
	else if ([item action] == @selector(_toggleBreakpoint:)) {
		if (_breakpointForContextualMenu == nil)
			return NO;
		
		if ([_breakpointForContextualMenu isActive])
			[item setTitle:NSLocalizedString(@"Disable Breakpoint", @"Disable Breakpoint")];
		else
			[item setTitle:NSLocalizedString(@"Enable Breakpoint", @"Enable Breakpoint")];
	}
	else if ([item action] == @selector(_editBreakpoint:)) {
		if (_breakpointForContextualMenu == nil)
			[item setTitle:NSLocalizedString(@"Add Breakpoint", @"Add Breakpoint")];
		else {
			[item setTitle:NSLocalizedString(@"Edit Breakpoint\u2026", @"Edit Breakpoint with ellipsis")];
			
			if ([self currentEditViewController] != nil)
				return NO;
		}
	}
	else if ([item action] == @selector(_revealInBreakpointsView:)) {
		if (_breakpointForContextualMenu == nil)
			return NO;
	}
	return YES;
}
#pragma mark NSKeyValueObserving
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	
	if ([(NSString *)context isEqualToString:kWCPreferencesEditorErrorLineHighlightKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesEditorErrorLineHighlightColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesEditorDisplayErrorBadgesKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesEditorWarningLineHighlightKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesEditorWarningLineHighlightColorKey] ||
		[(NSString *)context isEqualToString:kWCPreferencesEditorDisplayWarningBadgesKey])
		[self setNeedsDisplay:YES];
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}
#pragma mark *** Public Methods ***
#pragma mark Accessors
@synthesize currentEditViewController=_currentEditViewController;

- (NSFont *)font {
	return [NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSMiniControlSize]];
}

- (NSColor *)textColor {
	return [NSColor disabledControlTextColor];
}

- (NSColor *)alternateTextColor {
	return [NSColor alternateSelectedControlTextColor];
}

- (NSColor *)backgroundColor {
	return [NSColor colorWithCalibratedWhite:0.945 alpha:1.0];
}

- (NSArray *)lineIndices {
	if (!hasPerformedSetup) {
		hasPerformedSetup = YES;
		
		CGFloat oldThickness = [self ruleThickness];
        CGFloat newThickness = [self requiredThickness];
		
		if (fabs(oldThickness - newThickness) > 1)
			[self setRuleThickness:newThickness];
	}
	return [[[(WCTextView *)[self clientView] file] textStorage] lineStartIndexes];
}
#pragma mark *** Private Methods ***
- (NSUInteger)lineNumberForLocation:(CGFloat)location {
	NSUInteger		line, count, index, rectCount, i;
	NSRectArray		rects;
	NSRect			visibleRect;
	NSLayoutManager	*layoutManager;
	NSTextContainer	*container;
	NSRange			nullRange;
	NSArray	*lines;
	id				view;
	
	view = [self clientView];
	visibleRect = [[[self scrollView] contentView] bounds];
	
	lines = [self lineIndices];
	
	location += NSMinY(visibleRect);
	
	if ([view isKindOfClass:[NSTextView class]])
	{
		nullRange = NSMakeRange(NSNotFound, 0);
		layoutManager = [view layoutManager];
		container = [view textContainer];
		count = [lines count];
		
		for (line = 0; line < count; line++)
		{
			index = [[lines objectAtIndex:line] unsignedIntegerValue];
			
			rects = [layoutManager rectArrayForCharacterRange:NSMakeRange(index, 0)
								 withinSelectedCharacterRange:nullRange
											  inTextContainer:container
													rectCount:&rectCount];
			
			for (i = 0; i < rectCount; i++)
			{
				if ((location >= NSMinY(rects[i])) && (location < NSMaxY(rects[i])))
				{
					return line;
				}
			}
		}	
	}
	return NSNotFound;
}

- (NSPoint)centerPointForLocation:(CGFloat)location; {
	NSUInteger		line, count, index, rectCount, i;
	NSRectArray		rects;
	NSRect			visibleRect;
	NSLayoutManager	*layoutManager;
	NSTextContainer	*container;
	NSRange			nullRange;
	NSArray	*lines;
	id				view;
	
	view = [self clientView];
	visibleRect = [[[self scrollView] contentView] bounds];
	
	lines = [self lineIndices];
	
	location += NSMinY(visibleRect);
	
	if ([view isKindOfClass:[NSTextView class]])
	{
		nullRange = NSMakeRange(NSNotFound, 0);
		layoutManager = [view layoutManager];
		container = [view textContainer];
		count = [lines count];
		
		for (line = 0; line < count; line++)
		{
			index = [[lines objectAtIndex:line] unsignedIntegerValue];
			
			rects = [layoutManager rectArrayForCharacterRange:NSMakeRange(index, 0)
								 withinSelectedCharacterRange:nullRange
											  inTextContainer:container
													rectCount:&rectCount];
			
			for (i = 0; i < rectCount; i++)
			{
				if ((location >= NSMinY(rects[i])) && (location < NSMaxY(rects[i])))
				{
					return NSMakePoint(NSMinX(rects[i]), NSMaxY(rects[i])-floor(NSHeight(rects[i])/2.0));
				}
			}
		}	
	}
	return NSZeroPoint;
}

- (NSUInteger)lineNumberForCharacterIndex:(NSUInteger)index inText:(NSString *)text {
	return [(WCTextStorage *)[(WCTextView *)[self clientView] textStorage] lineNumberForCharacterIndex:index];
}

- (NSDictionary *)textAttributes {
    return [NSDictionary dictionaryWithObjectsAndKeys:
            [self font], NSFontAttributeName, 
            [self textColor], NSForegroundColorAttributeName,
            nil];
}

- (NSDictionary *)markerTextAttributes {
	return [NSDictionary dictionaryWithObjectsAndKeys:
            [self font], NSFontAttributeName, 
            [self alternateTextColor], NSForegroundColorAttributeName,
			nil];
}
#pragma mark IBActions
- (IBAction)_editBreakpoint:(id)sender {
	if (_breakpointForContextualMenu == nil)
		[[(WCTextView *)[self clientView] file] addBreakpoint:[WCBreakpoint breakpointWithLineNumber:_lineNumberForContextualMenu inFile:[(WCTextView *)[self clientView] file]]];
	else if ([self currentEditViewController] == nil) {
		WCBreakpointEditViewController *controller = [[WCBreakpointEditViewController alloc] initWithBreakpoint:_breakpointForContextualMenu rulerView:self];
		MAAttachedWindow *window = [[MAAttachedWindow alloc] initWithView:[controller view] attachedToPoint:[self convertPointToBase:[self convertPoint:[self centerPointForLocation:_locationForContextualMenu] fromView:[self clientView]]] inWindow:[self window] onSide:MAPositionRight atDistance:0.0];
		
		[self setCurrentEditViewController:controller];
		
		[window setBackgroundColor:[NSColor colorWithCalibratedWhite:0.9 alpha:1.0]];
		
		[controller showEditViewController:nil];
	}
}
- (IBAction)_toggleBreakpoint:(id)sender {
	[_breakpointForContextualMenu setIsActive:![_breakpointForContextualMenu isActive]];
}
- (IBAction)_deleteBreakpoint:(id)sender {
	[[(WCTextView *)[self clientView] file] removeBreakpoint:_breakpointForContextualMenu];
}
- (IBAction)_deleteAllBreakpoints:(id)sender {
	NSAlert *alert = [NSAlert alertWithMessageText:[NSString stringWithFormat:NSLocalizedString(@"Delete All Breakpoints in \"%@?\"", @"Delete All Breakpoints in File alert message"),[[(WCTextView *)[self clientView] file] name]] defaultButton:NS_LOCALIZED_STRING_DELETE_ALL alternateButton:NS_LOCALIZED_STRING_CANCEL otherButton:nil informativeTextWithFormat:NSLocalizedString(@"This operation cannot be undone.", @"This operation cannot be undone.")];
	
	[alert beginSheetModalForWindow:[self window] completionHandler:^(NSAlert *mAlert,NSInteger result) {
		if (result != NSAlertDefaultReturn)
			return;
		
		for (WCBreakpoint *bp in [[(WCTextView *)[self clientView] file] allBreakpoints])
			[[(WCTextView *)[self clientView] file] removeBreakpoint:bp];
	}];
}
- (IBAction)_revealInBreakpointsView:(id)sender {
	[[[(WCTextView *)[self clientView] file] project] viewBreakpoints:nil];
	[[[[(WCTextView *)[self clientView] file] project] breakpointsViewController] setSelectedObject:_breakpointForContextualMenu];
}
#pragma mark Notifications
- (void)textDidChange:(NSNotification *)notification {
	// Invalidate the line indices. They will be recalculated and recached on demand.
	//[self invalidateLineIndices];
	
	CGFloat oldThickness = [self ruleThickness];
	CGFloat newThickness = [self requiredThickness];
	if (fabs(oldThickness - newThickness) > 1)
	{
		NSInvocation			*invocation;
		
		// Not a good idea to resize the view during calculations (which can happen during
		// display). Do a delayed perform (using NSInvocation since arg is a float).
		invocation = [NSInvocation invocationWithMethodSignature:[self methodSignatureForSelector:@selector(setRuleThickness:)]];
		[invocation setSelector:@selector(setRuleThickness:)];
		[invocation setTarget:self];
		[invocation setArgument:&newThickness atIndex:2];
		
		[invocation performSelector:@selector(invoke) withObject:nil afterDelay:0.0];
	}
	
    [self setNeedsDisplay:YES];
}

- (void)_fileNumberOfBuildMessagesChanged:(NSNotification *)note {
	[self setNeedsDisplay:YES];
}

- (void)_fileNumberOfBreakpointsDidChange:(NSNotification *)note {	
	[self setNeedsDisplay:YES];
}

- (void)_breakpointIsActiveDidChange:(NSNotification *)note {
	if ([[note object] file] != [(WCTextView *)[self clientView] file])
		return;
	
	[self setNeedsDisplay:YES];
}
@end
