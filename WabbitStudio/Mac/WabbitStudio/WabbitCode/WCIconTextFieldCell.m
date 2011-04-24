//
//  WCIconTextFieldCell.m
//  customcelltest
//
//  Created by William Towe on 4/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "WCIconTextFieldCell.h"
#import "WCDefines.h"

static const CGFloat kIconMarginLeft = 2.0;
static const CGFloat kIconMarginRight = 2.0;
static const CGFloat kIconMarginTop = 2.0; // only applies if centerIcon is not set to YES

@interface WCIconTextFieldCell ()
- (void)_privateInit;
@end

@implementation WCIconTextFieldCell

- (id)initTextCell:(NSString *)stringValue {
	if (!(self = [super initTextCell:stringValue]))
		return nil;
	
	[self _privateInit];
	
	return self;
}

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	[self _privateInit];
	
	return self;
}

- (void)dealloc {
	[_icon release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone {
	WCIconTextFieldCell *copy = [super copyWithZone:zone];
	
	copy->_icon = [_icon retain];
	copy->_iconSize = _iconSize;
	copy->_centerIcon = _centerIcon;
	
	return copy;
}

- (NSRect)titleRectForBounds:(NSRect)bounds {	
	NSRect iconRect = [self iconRectForBounds:bounds];
	
	return [super titleRectForBounds:NSMakeRect(NSMaxX(iconRect), NSMinY(bounds), NSWidth(bounds)-NSWidth(iconRect), NSHeight(bounds))];
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView {
	if ([self icon]) {
		NSRect iconRect = [self iconRectForBounds:cellFrame];
		
		[[self icon] setSize:[self iconSize]];
		
		if ([self centerIcon])
			[[self icon] drawInRect:WCCenteredRectWithSize([self iconSize], iconRect) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
		else
			[[self icon] drawInRect:NSMakeRect(NSMinX(iconRect)+kIconMarginLeft, NSMinY(iconRect)+kIconMarginTop, [self iconSize].width, [self iconSize].height) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
	}
	
	[super drawInteriorWithFrame:cellFrame inView:controlView];
}

- (NSRect)iconRectForBounds:(NSRect)bounds; {
	if ([self icon] == nil)
		return NSZeroRect;
	
	NSRect left, right;
	NSDivideRect(bounds, &left, &right, [self iconSize].width+kIconMarginLeft+kIconMarginRight, NSMinXEdge);
	
	return left;
}

@synthesize icon=_icon;
@synthesize iconSize=_iconSize;
@synthesize centerIcon=_centerIcon;

- (void)_privateInit; {
	_iconSize = WCSmallSize;
	_centerIcon = YES;
}
@end
