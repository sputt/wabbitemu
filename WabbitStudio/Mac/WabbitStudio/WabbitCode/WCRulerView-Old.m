//
//  WCRulerView.m
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCRulerView-Old.h"
#import "WCFile.h"
#import "WCTextStorage.h"
#import "WCBuildMessage.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "NSObject+WCExtensions.h"


const CGFloat WCRulerViewDefaultThickness = 25.0;
const CGFloat WCRulerViewRulerMargin = 5.0;
const CGFloat WCRulerViewBuildMessageWidth = 14.0;

static NSDictionary *_textAttributes = nil;

@implementation WCRulerView

+ (void)initialize {
	if ([WCRulerView class] != self)
		return;
	
	_textAttributes = [[NSDictionary alloc] initWithObjectsAndKeys:[NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSMiniControlSize]],NSFontAttributeName,[NSColor colorWithCalibratedWhite:0.42 alpha:1.0],NSForegroundColorAttributeName, nil];
}

- (id)initWithScrollView:(NSScrollView *)scrollView forFile:(WCFile *)file; {
	if (!(self = [super initWithScrollView:scrollView orientation:NSVerticalRuler]))
		return nil;
	
	_file = file;
	
	[self setClientView:[scrollView documentView]];
	
	[self setupNotificationObserving];
	[self setupUserDefaultsObserving];
	
	[self adjustRequiredThickness];
	
	return self;
}

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[self cleanupUserDefaultsObserving];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	_file = nil;
    [super dealloc];
}

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

- (NSArray *)userDefaultsKeys {
	return [NSArray arrayWithObjects:kWCPreferencesEditorErrorLineHighlightKey,kWCPreferencesEditorErrorLineHighlightColorKey,kWCPreferencesEditorDisplayErrorBadgesKey,kWCPreferencesEditorWarningLineHighlightKey,kWCPreferencesEditorWarningLineHighlightColorKey,kWCPreferencesEditorDisplayWarningBadgesKey, nil];
}

- (NSArray *)notificationDictionaries {
	return [NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_fileTextDidChange:)),kNSObjectSelectorKey,NSTextStorageDidProcessEditingNotification,kNSObjectNotificationNameKey,[_file textStorage],kNSObjectNotificationObjectKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_fileNumberOfBuildMessagesChanged:)),kNSObjectSelectorKey,kWCFileNumberOfErrorMessagesChangedNotification,kNSObjectNotificationNameKey,_file,kNSObjectNotificationObjectKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:NSStringFromSelector(@selector(_fileNumberOfBuildMessagesChanged:)),kNSObjectSelectorKey,kWCFileNumberOfWarningMessagesChangedNotification,kNSObjectNotificationNameKey,_file,kNSObjectNotificationObjectKey, nil], nil];
}

- (CGFloat)requiredThickness {
    NSUInteger lineCount = [[_file textStorage] numberOfLines];
	NSUInteger digits = (NSUInteger)log10(lineCount) + 1;
    NSMutableString *sampleString = [NSMutableString string];

    for (NSUInteger i = 0; i < digits; i++)
		[sampleString appendString:@"8"];
    
    NSSize stringSize = [sampleString sizeWithAttributes:_textAttributes];
	
	CGFloat thickness = ceilf(MAX(WCRulerViewDefaultThickness, WCRulerViewBuildMessageWidth + stringSize.width + (WCRulerViewRulerMargin * 2.0)));
	
	
    return thickness;
}
	 
- (void)drawHashMarksAndLabelsInRect:(NSRect)rect {
	NSRect bounds = [self bounds];
	NSLayoutManager *layoutManager = [(NSTextView *)[self clientView] layoutManager];
	NSTextContainer *textContainer = [(NSTextView *)[self clientView] textContainer];
	NSRange nullRange = NSMakeRange(NSNotFound, 0);
	CGFloat yinset = [(NSTextView *)[self clientView] textContainerInset].height;
	NSRect visibleRect = [[[self scrollView] contentView] bounds];
	// Find the characters that are currently visible
	NSRange glyphRange = [layoutManager glyphRangeForBoundingRect:visibleRect inTextContainer:textContainer];
	NSRange range = [layoutManager characterRangeForGlyphRange:glyphRange actualGlyphRange:NULL];
	NSUInteger line, index = 0, count = [[_file textStorage] numberOfLines];
	BOOL showErrorBadges = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorDisplayErrorBadgesKey];
	BOOL errorLineHighlight = ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorErrorLineHighlightKey] && showErrorBadges);
	BOOL showWarningBadges = [[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorDisplayWarningBadgesKey];
	BOOL warningLineHighlight = ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesEditorWarningLineHighlightKey] && showErrorBadges);
	
	range.length++;
	
	for (line = [[_file textStorage] lineNumberForCharacterIndex:range.location]; line < count; line++)
	{
		index = [[_file textStorage] lineStartIndexForLineNumber:line];
		
		if (NSLocationInRange(index, range)) {
			NSUInteger rectCount = 0;
			NSRectArray rects = [layoutManager rectArrayForCharacterRange:NSMakeRange(index, 0)withinSelectedCharacterRange:nullRange inTextContainer:textContainer rectCount:&rectCount];
			
			if (rectCount > 0) {
				// Note that the ruler view is only as tall as the visible
				// portion. Need to compensate for the clipview's coordinates.
				CGFloat ypos = yinset + NSMinY(rects[0]) - NSMinY(visibleRect);
				// Line numbers are internally stored starting at 0

				NSString *labelText = [NSString stringWithFormat:@"%u", line + 1];
				NSSize stringSize = [labelText sizeWithAttributes:_textAttributes];
				
				// Draw string flush right, centered vertically within the line
				[labelText drawInRect:NSMakeRect(NSWidth(bounds) - stringSize.width - WCRulerViewRulerMargin,ypos + (NSHeight(rects[0]) - stringSize.height) / 2.0,NSWidth(bounds) - WCRulerViewRulerMargin * 2.0, NSHeight(rects[0])) withAttributes:_textAttributes];
				
				if (showErrorBadges) {
					WCBuildMessage *error = [_file errorMessageAtLineNumber:line];
					
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
						
						NSRect drawRect = NSMakeRect(bounds.origin.x,ypos + (NSHeight(rects[0]) - 12.0)/2.0,12.0, 12.0);
						drawRect.size.width = drawRect.size.height = 12.0;
						
						NSImage *icon = [error icon];
						[icon setSize:NSMakeSize(12.0, 12.0)];
						
						[icon drawInRect:drawRect fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
					}
				}
				
				if (showWarningBadges) {
					WCBuildMessage *warning = [_file warningMessageAtLineNumber:line];
					
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
						
						NSRect drawRect = NSMakeRect(bounds.origin.x,ypos + (NSHeight(rects[0]) - 12.0)/2.0,12.0, 12.0);
						drawRect.size.width = drawRect.size.height = 12.0;
						
						NSImage *icon = [warning icon];
						[icon setSize:NSMakeSize(12.0, 12.0)];
						
						[icon drawInRect:drawRect fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
					}
				}
			}
		}
		
		if (index > NSMaxRange(range))
			break;
	}
}

- (void)adjustRequiredThickness; {
	CGFloat oldThickness = [self ruleThickness];
	CGFloat newThickness = [self requiredThickness];
	
	if (fabs(oldThickness - newThickness) > 1) {
		NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:[self methodSignatureForSelector:@selector(setRuleThickness:)]];
		
		[invocation setSelector:@selector(setRuleThickness:)];
		[invocation setTarget:self];
		[invocation setArgument:&newThickness atIndex:2];
		
		[invocation performSelector:@selector(invoke) withObject:nil afterDelay:0.0];
	}
	
    [self setNeedsDisplay:YES];
}

- (void)_fileTextDidChange:(NSNotification *)note {
	[self adjustRequiredThickness];
}

- (void)_fileNumberOfBuildMessagesChanged:(NSNotification *)note {
	if ([note object] == _file)
		[self setNeedsDisplay:YES];
}
@end
