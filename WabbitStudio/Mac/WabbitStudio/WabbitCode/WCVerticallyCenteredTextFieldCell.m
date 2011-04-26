//
//  WCVerticallyCenteredTextFieldCell.m
//  WabbitStudio
//
//  Created by William Towe on 4/21/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCVerticallyCenteredTextFieldCell.h"
#import "WCDefines.h"


@implementation WCVerticallyCenteredTextFieldCell
- (id)init {
	if (!(self = [super init]))
		return nil;
	
	[self commonInit];
	
	return self;
}

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	[self commonInit];
	
	return self;
}

- (id)copyWithZone:(NSZone *)zone {
	WCVerticallyCenteredTextFieldCell *copy = [super copyWithZone:zone];
	
	copy->_excludeFileExtensionWhenSelecting = _excludeFileExtensionWhenSelecting;
	
	return copy;
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView {	
	[super drawInteriorWithFrame:[self centeredTitleRectForBounds:cellFrame] inView:controlView];
}

- (void)editWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject event:(NSEvent *)theEvent {
	[super editWithFrame:[self centeredTitleRectForBounds:aRect] inView:controlView editor:textObj delegate:anObject event:theEvent];
}

- (void)selectWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject start:(NSInteger)selStart length:(NSInteger)selLength {
	[super selectWithFrame:[self centeredTitleRectForBounds:aRect] inView:controlView editor:textObj delegate:anObject start:selStart length:([self excludeFileExtensionWhenSelecting])?[[[self stringValue] stringByDeletingPathExtension] length]:selLength];
}

- (NSRect)centeredTitleRectForBounds:(NSRect)bounds; {
	NSAttributedString *attributedString = [self attributedStringValue];
	NSSize size = [attributedString size];
	
	return WCCenteredRectWithSize(NSMakeSize(NSWidth(bounds), size.height), bounds);
}

- (void)commonInit; {
	_excludeFileExtensionWhenSelecting = YES;
}

@synthesize excludeFileExtensionWhenSelecting=_excludeFileExtensionWhenSelecting;
@end
