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

- (NSRect)titleRectForBounds:(NSRect)bounds {
	NSAttributedString *attributedString = [self attributedStringValue];
	NSSize size = [attributedString size];
	
	return WCCenteredRectWithSize(NSMakeSize(NSWidth(bounds), size.height), bounds);
}

- (void)drawInteriorWithFrame:(NSRect)cellFrame inView:(NSView *)controlView {
	[super drawInteriorWithFrame:[self titleRectForBounds:cellFrame] inView:controlView];
}

- (void)editWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject event:(NSEvent *)theEvent {
	[super editWithFrame:[self titleRectForBounds:aRect] inView:controlView editor:textObj delegate:anObject event:theEvent];
}

- (void)selectWithFrame:(NSRect)aRect inView:(NSView *)controlView editor:(NSText *)textObj delegate:(id)anObject start:(NSInteger)selStart length:(NSInteger)selLength {
	[super selectWithFrame:[self titleRectForBounds:aRect] inView:controlView editor:textObj delegate:anObject start:selStart length:selLength];
}
@end
