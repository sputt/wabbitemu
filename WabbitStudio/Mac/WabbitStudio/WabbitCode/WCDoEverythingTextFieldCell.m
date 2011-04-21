//
//  WCMultiLineTextFieldCell.m
//  customcelltest
//
//  Created by William Towe on 4/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WCDoEverythingTextFieldCell.h"
#import "WCDefines.h"

static const CGFloat kIconMarginLeftRight = 2.0;
static const CGFloat kSecondaryTitleMarginTopBottom = 1.0;
static const CGFloat kSecondaryTitleMarginLeft = 4.0;
static const CGFloat kBadgeMarginLeftRight = 2.0;
static const CGFloat kBadgeMarginTopBottom = 2.0;
static const CGFloat kBadgeInsetLeftRight = 4.0;
static const CGFloat kBadgeInsetTopBottom = 1.0;
static const CGFloat kBadgeMinWidth = 22.0;

@implementation WCDoEverythingTextFieldCell

- (void)dealloc {
	[_secondaryTitle release];
	[_icon release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone {
	WCDoEverythingTextFieldCell *copy = [super copyWithZone:zone];
	
	copy->_secondaryTitle = [_secondaryTitle copy];
	copy->_icon = [_icon retain];
	copy->_iconSize = _iconSize;
	copy->_badgeCount = _badgeCount;
	
	return copy;
}

- (void)drawInteriorWithFrame:(NSRect)frame inView:(NSView *)view {
	if ([self icon]) {
		NSRect left, right;
		NSDivideRect(frame, &left, &right, [self iconSize].width+kIconMarginLeftRight*2, NSMinXEdge);
		
		[[self icon] setSize:[self iconSize]];
		[[self icon] drawInRect:WCCenteredRectWithSize([self iconSize], left) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
		
		frame = right;
	}
	
	if ([self badgeCount] > 0) {
		NSColor *textColor = [self badgeTextColor];
		NSColor *backgroundColor = [self badgeBackgroundColor];
		
		NSString *badgeString = [NSString stringWithFormat:@"%lu",[self badgeCount]];
		NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont boldSystemFontOfSize:[NSFont systemFontSizeForControlSize:[self controlSize]]],NSFontAttributeName,textColor,NSForegroundColorAttributeName, nil];
		NSAttributedString *attributedString = [[[NSAttributedString alloc] initWithString:badgeString attributes:attributes] autorelease];
		NSSize size = [attributedString size];
		NSRect left,right;
		CGFloat width = (size.width+kBadgeInsetLeftRight*2);
		if (width < kBadgeMinWidth)
			width = kBadgeMinWidth;
		NSDivideRect(frame, &right, &left, width+kBadgeMarginLeftRight*2, NSMaxXEdge);
		
		right = WCCenteredRectWithSize(NSMakeSize(width, size.height+kBadgeInsetTopBottom*2), right);
		
		NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:right xRadius:8.0 yRadius:8.0];
		
		[backgroundColor setFill];
		[path fill];
		
		right = WCCenteredRectWithSize(size, right);
		
		[attributedString drawInRect:right];
		
		frame = left;
	}
	
	if ([[self secondaryTitle] length] > 0) {
		NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
		[style setLineBreakMode:NSLineBreakByTruncatingTail];
		[style setAlignment:NSLeftTextAlignment];
		NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]],NSFontAttributeName,([self isHighlighted])?[NSColor alternateSelectedControlTextColor]:[NSColor disabledControlTextColor],NSForegroundColorAttributeName,style,NSParagraphStyleAttributeName, nil];
		NSAttributedString *attributedString = [[[NSAttributedString alloc] initWithString:[self secondaryTitle] attributes:attributes] autorelease];
		NSSize size = [attributedString size];
		
		NSRect top, bottom;
		NSDivideRect(frame, &top, &bottom, floor(NSHeight(frame)/2.0), NSMinYEdge);
		
		bottom = WCCenteredRect(NSMakeRect(NSMinX(bottom)+kSecondaryTitleMarginLeft, NSMinY(bottom), NSWidth(bottom)-kSecondaryTitleMarginLeft, size.height), bottom);
		
		[attributedString drawInRect:bottom];
		
		frame = top;
	}
	
	
	NSAttributedString *attributedString = [self attributedStringValue];
	NSSize size = [attributedString size];
	
	frame = WCCenteredRectWithSize(NSMakeSize(NSWidth(frame), size.height), frame);
	
	 
	[super drawInteriorWithFrame:frame inView:view];
}

- (void)editWithFrame:(NSRect)rect inView:(NSView *)view editor:(NSText *)editor delegate:(id)object event:(NSEvent *)event {
	[super editWithFrame:[self titleRectForBounds:rect] inView:view editor:editor delegate:object event:event];
}

- (void)selectWithFrame:(NSRect)rect inView:(NSView *)view editor:(NSText *)editor delegate:(id)object start:(NSInteger)start length:(NSInteger)length {
	[super selectWithFrame:[self titleRectForBounds:rect] inView:view editor:editor delegate:object start:start length:length];
}

- (NSUInteger)hitTestForEvent:(NSEvent *)event inRect:(NSRect)frame ofView:(NSView *)view {
	if ([view mouse:[event locationInWindow] inRect:[view convertRectToBase:[self titleRectForBounds:frame]]])
		return NSCellHitEditableTextArea;
	return NSCellHitContentArea;
}

- (NSRect)titleRectForBounds:(NSRect)frame {
	if ([self icon]) {
		NSRect left, right;
		NSDivideRect(frame, &left, &right, [self iconSize].width+kIconMarginLeftRight*2, NSMinXEdge);
		
		frame = right;
	}
	
	if ([self badgeCount] > 0) {
		NSString *badgeString = [NSString stringWithFormat:@"%lu",[self badgeCount]];
		NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont boldSystemFontOfSize:[NSFont systemFontSizeForControlSize:[self controlSize]]],NSFontAttributeName, nil];
		NSAttributedString *attributedString = [[[NSAttributedString alloc] initWithString:badgeString attributes:attributes] autorelease];
		NSSize size = [attributedString size];
		NSRect left,right;
		NSDivideRect(frame, &right, &left, size.width+(kBadgeMarginLeftRight*2)+(kBadgeInsetLeftRight*2), NSMaxXEdge);
		
		frame = left;
	}
	
	if ([[self secondaryTitle] length] > 0) {
		NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]],NSFontAttributeName, nil];
		NSAttributedString *attributedString = [[[NSAttributedString alloc] initWithString:[self secondaryTitle] attributes:attributes] autorelease];
		NSSize size = [attributedString size];
		
		NSRect top, bottom;
		NSDivideRect(frame, &top, &bottom, size.height+kSecondaryTitleMarginTopBottom*2, NSMinYEdge);
		
		return WCCenteredRect(NSMakeRect(NSMinX(top), NSMinY(top), NSWidth(top), size.height), top);
	}
	
	NSAttributedString *attributedString = [self attributedStringValue];
	NSSize size = [attributedString size];
	
	frame = WCCenteredRect(NSMakeRect(NSMinX(frame), NSMinY(frame), NSWidth(frame), size.height), frame);
	
	return frame;
}

@synthesize secondaryTitle=_secondaryTitle;
@synthesize icon=_icon;
@synthesize iconSize=_iconSize;
@synthesize badgeCount=_badgeCount;

@dynamic badgeTextColor;
- (NSColor *)badgeTextColor {
	NSColor *textColor = nil;
	if ([self isHighlighted] &&
		[[[self controlView] window] firstResponder] == [self controlView] &&
		[[NSApplication sharedApplication] keyWindow] == [[self controlView] window])
		textColor = [NSColor keyboardFocusIndicatorColor];
	else if ([self isHighlighted] &&
			 [[NSApplication sharedApplication] keyWindow] == [[self controlView] window])
		textColor = [NSColor colorWithCalibratedRed:(152/255.0) green:(168/255.0) blue:(202/255.0) alpha:1.0];
	else if ([self isHighlighted])
		textColor = [NSColor colorWithDeviceWhite:(180/255.0) alpha:1.0];
	else
		textColor = [NSColor alternateSelectedControlTextColor];
	
	return textColor;
}
@dynamic badgeBackgroundColor;
- (NSColor *)badgeBackgroundColor {
	NSColor *backgroundColor = nil;
	if ([self isHighlighted])
		backgroundColor = [NSColor alternateSelectedControlTextColor];
	else if ([[NSApplication sharedApplication] keyWindow] == [[self controlView] window])
		backgroundColor = [NSColor colorWithCalibratedRed:(152/255.0) green:(168/255.0) blue:(202/255.0) alpha:1.0];
	else
		backgroundColor = [NSColor colorWithDeviceWhite:(180/255.0) alpha:1];
	
	return backgroundColor;
}
@end
