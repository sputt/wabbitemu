//
//  WCIconTextFieldCell.m
//  WabbitStudio
//
//  Created by William Towe on 4/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSIconTextFieldCell.h"
#import "WCDefines.h"

static const CGFloat kIconMarginLeft = 2.0;
static const CGFloat kIconMarginRight = 2.0;
static const CGFloat kIconMarginTop = 2.0; // only applies if centerIcon is not set to YES

@implementation RSIconTextFieldCell

- (void)dealloc {
	[_icon release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone {
	RSIconTextFieldCell *copy = [super copyWithZone:zone];
	
	copy->_icon = [_icon retain];
	copy->_iconSize = _iconSize;
	copy->_centerIcon = _centerIcon;
	
	return copy;
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView {
	if ([self icon]) {
		NSRect iconRect = [self iconRectForBounds:cellFrame remainingRect:&cellFrame];
		
		[[self icon] setSize:[self iconSize]];
		
		if ([self centerIcon])
			[[self icon] drawInRect:WCCenteredRectWithSize([self iconSize], iconRect) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
		else
			[[self icon] drawInRect:NSMakeRect(NSMinX(iconRect)+kIconMarginLeft, NSMinY(iconRect)+kIconMarginTop, [self iconSize].width, [self iconSize].height) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
	}
	
	[super drawInteriorWithFrame:cellFrame inView:controlView];
}

- (void)editWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject event:(NSEvent *)theEvent {
	[self iconRectForBounds:aRect remainingRect:&aRect];
	[super editWithFrame:aRect inView:controlView editor:textObj delegate:anObject event:theEvent];
}

- (void)selectWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject start:(NSInteger)selStart length:(NSInteger)selLength {
	[self iconRectForBounds:aRect remainingRect:&aRect];
	[super selectWithFrame:aRect inView:controlView editor:textObj delegate:anObject start:selStart length:selLength];
}

- (NSRect)expansionFrameWithFrame:(NSRect)cellFrame inView:(NSView *)view {
	if ([self icon] == nil)
		return [super expansionFrameWithFrame:cellFrame inView:view];
	
	NSRect rect = [super expansionFrameWithFrame:cellFrame inView:view];
	rect.size.width += [self iconSize].width+kIconMarginLeft+kIconMarginRight;
	return rect;
}

- (NSUInteger)hitTestForEvent:(NSEvent *)event inRect:(NSRect)cellFrame ofView:(NSView *)controlView {
	[self iconRectForBounds:cellFrame remainingRect:&cellFrame];
	return [super hitTestForEvent:event inRect:cellFrame ofView:controlView];
}

- (NSRect)iconRectForBounds:(NSRect)bounds remainingRect:(NSRectPointer)remainingRect; {
	if ([self icon] == nil)
		return NSZeroRect;
	
	NSRect left, right;
	NSDivideRect(bounds, &left, &right, [self iconSize].width+kIconMarginLeft+kIconMarginRight, NSMinXEdge);
	
	if (remainingRect != NULL)
		*remainingRect = right;
	
	return left;
}

@synthesize icon=_icon;
@synthesize iconSize=_iconSize;
@synthesize centerIcon=_centerIcon;

- (void)commonInit; {
	[super commonInit];
	
	_iconSize = WCSmallSize;
	_centerIcon = YES;
}
@end
