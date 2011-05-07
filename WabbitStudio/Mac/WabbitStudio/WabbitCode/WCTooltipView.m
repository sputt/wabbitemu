//
//  WCTooltipView.m
//  WabbitStudio
//
//  Created by William Towe on 5/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTooltipView.h"
#import "WCTooltip.h"
#import "WCDefines.h"
#import "NSBezierPath+StrokeExtensions.h"


const CGFloat kTooltipLeftMargin = 2.0;
const CGFloat kTooltipRightMargin = 2.0;
const CGFloat kTooltipTopMargin = 2.0;
const CGFloat kTooltipBottomMargin = 2.0;

@implementation WCTooltipView

- (void)drawRect:(NSRect)dirtyRect {
    if ([self tooltip] == nil)
		return;
	
	[[NSColor colorWithCalibratedRed:0.996 green:0.996 blue:0.788 alpha:1.0] setFill];
	NSRectFill([self bounds]);
	[[NSColor colorWithCalibratedRed:0.506 green:0.518 blue:0.525 alpha:1.0] setStroke];
	[[NSBezierPath bezierPathWithRect:[self bounds]] strokeInside];
	
	NSDictionary *attributes = [[self tooltip] attributesForTooltip];
	NSAttributedString *attributedString = [[[NSAttributedString alloc] initWithString:[[self tooltip] string] attributes:attributes] autorelease];
	
	[attributedString drawInRect:WCCenteredRectWithSize(NSMakeSize(NSWidth([self bounds])-kTooltipLeftMargin-kTooltipRightMargin, [attributedString size].height), [self bounds])];
}

@dynamic tooltip;
- (WCTooltip *)tooltip {
	return _tooltip;
}
- (void)setTooltip:(WCTooltip *)tooltip {
	[_tooltip release];
	_tooltip = [tooltip retain];
	
	[self setNeedsDisplay:YES];
}

@end
