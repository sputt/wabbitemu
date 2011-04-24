//
//  WCBadgedTextFieldCell.m
//  WabbitStudio
//
//  Created by William Towe on 4/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBadgedTextFieldCell.h"
#import "WCDefines.h"


static const CGFloat kBadgeMarginLeft = 2.0;
static const CGFloat kBadgeMarginRight = 2.0;
static const CGFloat kBadgeMarginTop = 1.0;
static const CGFloat kBadgeInsetLeft = 5.0;
static const CGFloat kBadgeInsetRight = 4.0;
static const CGFloat kBadgeInsetTop = 1.0;
static NSColor *kBadgeTextColor = nil;
static NSColor *kBadgeSelectedKeyTextColor = nil;
static NSColor *kBadgeSelectedNonKeyTextColor = nil;
static NSColor *kBadgeSelectedTextColor = nil;
static NSColor *kBadgeSelectedBackgroundColor = nil;
static NSColor *kBadgeKeyBackgroundColor = nil;
static NSColor *kBadgeBackgroundColor = nil;

@interface WCBadgedTextFieldCell ()
- (NSAttributedString *)_attributedStringForBadgeCount;
- (NSColor *)_textColorForBadgeString;
- (NSColor *)_backgroundColorForBadgeString;
@end

@implementation WCBadgedTextFieldCell

+ (void)initialize {
	if ([WCBadgedTextFieldCell class] != self)
		return;
	
	kBadgeTextColor = [[NSColor alternateSelectedControlTextColor] retain];
	kBadgeSelectedKeyTextColor = [[NSColor keyboardFocusIndicatorColor] retain];
	kBadgeSelectedNonKeyTextColor = [[NSColor colorWithCalibratedRed:(152/255.0) green:(168/255.0) blue:(202/255.0) alpha:1.0] retain];
	kBadgeSelectedTextColor = [[NSColor colorWithDeviceWhite:(180/255.0) alpha:1.0] retain];
	kBadgeSelectedBackgroundColor = [[NSColor alternateSelectedControlTextColor] retain];
	kBadgeKeyBackgroundColor = [[NSColor colorWithCalibratedRed:(152/255.0) green:(168/255.0) blue:(202/255.0) alpha:1.0] retain];
	kBadgeBackgroundColor = [[NSColor colorWithDeviceWhite:(180/255.0) alpha:1.0] retain];
}

- (id)copyWithZone:(NSZone *)zone {
	WCBadgedTextFieldCell *copy = [super copyWithZone:zone];
	
	copy->_badgeCount = _badgeCount;
	
	return copy;
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView {
	if ([self badgeCount] > 0) {
		NSRect badgeRect = [self badgeRectForBounds:cellFrame remainingRect:&cellFrame];
		NSAttributedString *attributedString = [self _attributedStringForBadgeCount];
		NSSize size = [attributedString size];
		NSRect fillRect = WCCenteredRect(NSMakeRect(NSMinX(badgeRect)+kBadgeMarginLeft, NSMinY(badgeRect)+kBadgeMarginTop, NSWidth(badgeRect)-kBadgeMarginLeft-kBadgeMarginRight, size.height+kBadgeInsetTop+kBadgeInsetTop), badgeRect);
		NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:fillRect xRadius:8.0 yRadius:8.0];
		
		[[self _backgroundColorForBadgeString] setFill];
		[path fill];
		
		[attributedString drawInRect:NSMakeRect(NSMinX(fillRect)+kBadgeInsetLeft, NSMinY(fillRect)+kBadgeInsetTop, NSWidth(fillRect)-kBadgeInsetLeft-kBadgeInsetRight, NSHeight(fillRect)-kBadgeMarginTop-kBadgeMarginTop)];
	}
	
	[super drawInteriorWithFrame:cellFrame inView:controlView];
}

- (NSRect)badgeRectForBounds:(NSRect)bounds remainingRect:(NSRectPointer)remainingRect; {
	if ([self badgeCount] == 0)
		return NSZeroRect;
	
	NSAttributedString *badgeString = [self _attributedStringForBadgeCount];
	NSSize size = [badgeString size];
	
	NSRect left, right;
	NSDivideRect(bounds, &right, &left, size.width+kBadgeMarginLeft+kBadgeMarginRight+kBadgeInsetLeft+kBadgeInsetRight, NSMaxXEdge);
	
	if (remainingRect != NULL)
		*remainingRect = left;
	
	return right;
}

@synthesize badgeCount=_badgeCount;

- (NSColor *)_textColorForBadgeString; {
	if ([self isHighlighted] &&
		[[[self controlView] window] firstResponder] == [self controlView] &&
		[[NSApplication sharedApplication] keyWindow] == [[self controlView] window])
		return kBadgeSelectedKeyTextColor;
	else if ([self isHighlighted] &&
			 [[NSApplication sharedApplication] keyWindow] == [[self controlView] window])
		return kBadgeSelectedNonKeyTextColor;
	else if ([self isHighlighted])
		return kBadgeSelectedTextColor;
	return kBadgeTextColor;
}

- (NSColor *)_backgroundColorForBadgeString; {
	if ([self isHighlighted])
		return kBadgeSelectedBackgroundColor;
	else if ([[NSApplication sharedApplication] keyWindow] == [[self controlView] window])
		return kBadgeKeyBackgroundColor;
	return kBadgeBackgroundColor;
}

- (NSAttributedString *)_attributedStringForBadgeCount; {
	NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
	[style setAlignment:NSCenterTextAlignment];
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont boldSystemFontOfSize:[NSFont systemFontSizeForControlSize:[self controlSize]]],NSFontAttributeName,[self _textColorForBadgeString],NSForegroundColorAttributeName,style,NSParagraphStyleAttributeName, nil];
	NSAttributedString *attributedString = [[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"%lu",[self badgeCount]] attributes:attributes] autorelease];
	
	return attributedString;
}

@end
