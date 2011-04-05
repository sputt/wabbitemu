//
//  WCIconTextFieldCell.m
//  WabbitStudio
//
//  Created by William Towe on 3/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCIconTextFieldCell.h"


@interface WCIconTextFieldCell (Private)
- (NSRect)_iconRectForBounds:(NSRect)bounds;
@end

@implementation WCIconTextFieldCell

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	_iconSize = (NSSize){16.0,16.0};
	
	return self;
}

- (id)initTextCell:(NSString *)string {
	if (!(self = [super initTextCell:string]))
		return nil;
	
	_iconSize = (NSSize){16.0,16.0};
	
	return self;
}

- (void)dealloc {
	[_icon release];
    [super dealloc];
}

@synthesize icon=_icon;
@synthesize iconSize=_iconSize;

#define kWCIconTextFieldCellMarginRight 2.0
#define kWCIconTextFieldCellMarginLeft 2.0

- (NSRect)titleRectForBounds:(NSRect)bounds {
	if (!_icon)
		return [super titleRectForBounds:bounds];
	
	NSRect icon, title;
	NSDivideRect(bounds, &icon, &title, [self iconSize].width+kWCIconTextFieldCellMarginRight+kWCIconTextFieldCellMarginRight, NSMinXEdge);
	return title;
}

- (void)selectWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject start:(NSInteger)selStart length:(NSInteger)selLength {
	// make sure we select file names differently, ignore the path extension if there is one
	NSString *string = [self stringValue];
	if (![[string pathExtension] length])
		[super selectWithFrame:[self titleRectForBounds:aRect] inView:controlView editor:textObj delegate:anObject start:selStart length:selLength];
	else
		[super selectWithFrame:[self titleRectForBounds:aRect] inView:controlView editor:textObj delegate:anObject start:selStart length:[string length] - ([[string pathExtension] length] + 1)];
}

- (void)editWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject event:(NSEvent *)theEvent {
	[super editWithFrame:[self titleRectForBounds:aRect] inView:controlView editor:textObj delegate:anObject event:theEvent];
}

- (NSUInteger)hitTestForEvent:(NSEvent *)event inRect:(NSRect)cellFrame ofView:(NSView *)controlView {
	return [super hitTestForEvent:event inRect:[self titleRectForBounds:cellFrame] ofView:controlView];
}

- (id)copyWithZone:(NSZone *)zone {
	WCIconTextFieldCell *copy = [super copyWithZone:zone];
	
	copy->_icon = nil;
	copy->_icon = [_icon retain];
	copy->_iconSize = _iconSize;
	
	return copy;
}

- (NSRect)_iconRectForBounds:(NSRect)bounds; {
	if (![self icon])
		return NSZeroRect;
	
	NSSize iconSize = [self iconSize];
	
	return NSMakeRect(bounds.origin.x+kWCIconTextFieldCellMarginLeft, bounds.origin.y+floor((bounds.size.height-iconSize.height)/2.0), iconSize.width, iconSize.height);
}

- (void)drawInteriorWithFrame:(NSRect)frame inView:(NSView *)view {
	if ([self icon]) {
		NSRect iconRect = [self _iconRectForBounds:frame];
		
		[[self icon] setSize:[self iconSize]];
		[[self icon] drawInRect:iconRect fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
	}
	[super drawInteriorWithFrame:[self titleRectForBounds:frame] inView:view];
}
@end
