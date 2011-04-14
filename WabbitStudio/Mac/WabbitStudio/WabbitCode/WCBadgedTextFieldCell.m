//
//  WCBadgedTextFieldCell.m
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBadgedTextFieldCell.h"
#import "WCDefines.h"
#import "WCGeneralPerformer.h"

#define kWCBadgedIconAndTextFieldCellMarginRight 6.0
#define kWCBadgedIconAndTextFieldCellMarginLeft 6.0
#define kWCBadgedIconAndTextFieldCellMarginTop 1.0
#define kWCBadgedIconAndTextFieldCellMarginBottom 1.0

@interface WCBadgedTextFieldCell (Private)
- (NSRect)badgeRectForBounds:(NSRect)bounds;
- (NSDictionary *)attributesForBadgeCountString;
- (NSColor *)badgeFillColor;
@end

@implementation WCBadgedTextFieldCell

- (id)copyWithZone:(NSZone *)zone {
	WCBadgedTextFieldCell *copy = [super copyWithZone:zone];
	
	copy->_badgeCount = _badgeCount;
	
	return copy;
}

- (NSRect)titleRectForBounds:(NSRect)bounds {	
	if (!_badgeCount)
		return [super titleRectForBounds:bounds];
	
	NSRect badgeRect = [self badgeRectForBounds:bounds];
	NSRect titleRect = [super titleRectForBounds:bounds];
	
	titleRect.size.width -= NSWidth(badgeRect)+kWCBadgedIconAndTextFieldCellMarginLeft;
	return titleRect;
}

- (void)drawInteriorWithFrame:(NSRect)frame inView:(NSView *)view {
	if (_badgeCount) {
		NSRect badgeRect = [self badgeRectForBounds:frame];
		
		//fillRect.origin.y = frame.origin.y+kWCBadgedIconAndTextFieldCellMarginTop+kWCBadgedIconAndTextFieldCellMarginBottom;
		//fillRect.size.height = frame.size.height-(kWCBadgedIconAndTextFieldCellMarginTop+kWCBadgedIconAndTextFieldCellMarginBottom)*2.0;
		
		[[self badgeFillColor] setFill];
		[[NSBezierPath bezierPathWithRoundedRect:badgeRect xRadius:8.0 yRadius:8.0] fill];
		
		NSAttributedString *string = [[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"%u", _badgeCount] attributes:[self attributesForBadgeCountString]] autorelease];
		NSRect titleRect = [string boundingRectWithSize:badgeRect.size options:NSStringDrawingOneShot];
		
		[string drawInRect:WCCenteredRect(titleRect, badgeRect)];
	}
	
	[super drawInteriorWithFrame:frame inView:view];
}

@synthesize badgeCount=_badgeCount;

- (NSRect)badgeRectForBounds:(NSRect)bounds; {
	if (!_badgeCount)
		return NSZeroRect;
	
	static NSDictionary *attributes = nil;
	if (!attributes) {
		NSMutableParagraphStyle *paragraphStyle = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
		[paragraphStyle setAlignment:NSCenterTextAlignment];
		attributes = [[NSDictionary alloc] initWithObjectsAndKeys:paragraphStyle, NSParagraphStyleAttributeName, [NSFont fontWithName:@"Helvetica-Bold" size:11.0], NSFontAttributeName, [NSColor textColor], NSForegroundColorAttributeName, nil]; 
	}
	
	NSSize size = [[NSString stringWithFormat:@"%u", _badgeCount] boundingRectWithSize:bounds.size options:NSStringDrawingOneShot attributes:attributes].size;
	
	return NSMakeRect(NSMaxX(bounds)-(size.width+kWCBadgedIconAndTextFieldCellMarginLeft+kWCBadgedIconAndTextFieldCellMarginRight), bounds.origin.y+(floor(bounds.size.height/2.0)-floor((size.height+kWCBadgedIconAndTextFieldCellMarginTop+kWCBadgedIconAndTextFieldCellMarginBottom)/2.0)), size.width+kWCBadgedIconAndTextFieldCellMarginLeft+kWCBadgedIconAndTextFieldCellMarginRight, size.height+kWCBadgedIconAndTextFieldCellMarginBottom+kWCBadgedIconAndTextFieldCellMarginTop);
}

- (NSDictionary *)attributesForBadgeCountString; {
	static NSParagraphStyle *style = nil;
	static NSDictionary *defaultAttributes = nil;
	static NSDictionary *selectedFirstResponderAttributes = nil;
	static NSDictionary *selectedAttributes = nil;
	static NSDictionary *selectedNonKeyAttributes = nil;
	if (!defaultAttributes) {
		NSFont *font = [NSFont fontWithName:@"Helvetica-Bold" size:11.0];
		NSMutableParagraphStyle *tStyle = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
		[tStyle setAlignment:NSCenterTextAlignment];
		style = [tStyle copy];
		
		// text is white when we aren't selected
		defaultAttributes = [[NSDictionary alloc] initWithObjectsAndKeys:font, NSFontAttributeName, style, NSParagraphStyleAttributeName, [NSColor whiteColor], NSForegroundColorAttributeName, nil];
		// text is blue when we are selected and our control view is first responder
		selectedFirstResponderAttributes = [[NSDictionary alloc] initWithObjectsAndKeys:font, NSFontAttributeName, style, NSParagraphStyleAttributeName, [NSColor colorWithCalibratedRed:58.0/255.0 green:117.0/255.0 blue:193.0/255.0 alpha:1.0], NSForegroundColorAttributeName, nil];
		// text is light purple if we are selected but our control view is not first responder
		selectedAttributes = [[NSDictionary alloc] initWithObjectsAndKeys:font, NSFontAttributeName, style, NSParagraphStyleAttributeName, [NSColor colorWithCalibratedRed:138.0/255.0 green:153.0/255.0 blue:188.0/255.0 alpha:1.0], NSForegroundColorAttributeName, nil];
		// text is light gray if we are selected and our window isn't the key window
		selectedNonKeyAttributes = [[NSDictionary alloc] initWithObjectsAndKeys:font, NSFontAttributeName, style, NSParagraphStyleAttributeName, [NSColor colorWithCalibratedWhite:154.0/255.0 alpha:1.0], NSForegroundColorAttributeName, nil];
	}
	
	if ([[self controlView] respondsToSelector:@selector(selectionHighlightStyle)] &&
		[(NSTableView *)[self controlView] selectionHighlightStyle] == NSTableViewSelectionHighlightStyleSourceList) {
		
		BOOL isHighlighted = [self isHighlighted];
		BOOL isFirstResponder = [[[[self controlView] window] firstResponder] isEqualTo:[self controlView]];
		BOOL isKeyWindow = [[NSApp keyWindow] isEqualTo:[[self controlView] window]];
		
		if (isHighlighted && isFirstResponder && isKeyWindow)
			return selectedFirstResponderAttributes;
		else if (isHighlighted && isKeyWindow)
			return selectedAttributes;
		else if (isHighlighted)
			return selectedNonKeyAttributes;
		else
			return defaultAttributes;
	}
	else {
		BOOL isHighlighted = [self isHighlighted];
		BOOL isWindowKey = ([[NSApp keyWindow] isEqualTo:[[self controlView] window]] && [[[[self controlView] window] firstResponder] isEqualTo:[self controlView]]);
		
		if (isHighlighted && isWindowKey)
			return [NSDictionary dictionaryWithObjectsAndKeys:[self font], NSFontAttributeName, style, NSParagraphStyleAttributeName, [NSColor alternateSelectedControlColor], NSForegroundColorAttributeName, nil];
		else if (isHighlighted)
			return [NSDictionary dictionaryWithObjectsAndKeys:[self font], NSFontAttributeName, style, NSParagraphStyleAttributeName, [NSColor textColor], NSForegroundColorAttributeName, nil];
		else
			return [NSDictionary dictionaryWithObjectsAndKeys:[self font], NSFontAttributeName, style, NSParagraphStyleAttributeName, [NSColor textColor], NSForegroundColorAttributeName, nil];
	}
}

- (NSColor *)badgeFillColor; {
	if ([[self controlView] respondsToSelector:@selector(selectionHighlightStyle)] &&
		[(NSTableView *)[self controlView] selectionHighlightStyle] == NSTableViewSelectionHighlightStyleSourceList) {
		static NSColor *defaultColor = nil;
		static NSColor *selectedColor = nil;
		static NSColor *nonKeyColor = nil;
		if (!defaultColor) {
			// light purple fill color when we are not selected and our window is the key window
			defaultColor = [[NSColor colorWithCalibratedRed:138.0/255.0 green:153.0/255.0 blue:188.0/255.0 alpha:1.0] retain];
			// white fill when we are selected
			selectedColor = [[NSColor whiteColor] retain];
			// light gray fill color when we are not selected and our window is not the key window
			nonKeyColor = [[NSColor colorWithCalibratedWhite:154.0/255.0 alpha:1.0] retain];
		}
		
		BOOL isHighlighted = [self isHighlighted];
		BOOL isKeyWindow = [[NSApp keyWindow] isEqualTo:[[self controlView] window]];
		
		if (isHighlighted)
			return selectedColor;
		else if (isKeyWindow)
			return defaultColor;
		else
			return nonKeyColor;
	}
	else {
		BOOL isHighlighted = [self isHighlighted];
		BOOL isWindowKey = ([[NSApp keyWindow] isEqualTo:[[self controlView] window]] && [[[[self controlView] window] firstResponder] isEqualTo:[self controlView]]);
		
		if (isHighlighted && isWindowKey)
			return [NSColor alternateSelectedControlTextColor];
		else if (isHighlighted)
			return [NSColor alternateSelectedControlTextColor];
		else
			return [NSColor secondarySelectedControlColor]; 
	}
}

@end
