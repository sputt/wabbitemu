//
//  WCGradientView.m
//  WabbitStudio
//
//  Created by William Towe on 4/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCGradientView.h"


@implementation WCGradientView
- (id)initWithCoder:(NSCoder *)aDecoder {
	if (!(self = [super initWithCoder:aDecoder]))
		return nil;
	
	[self commonInit];
	
	return self;
}

- (id)initWithFrame:(NSRect)frameRect {
	if (!(self = [super initWithFrame:frameRect]))
		return nil;
	
	[self commonInit];
	
	return self;
}

- (void)dealloc {
	[_gradient release];
	[_topLineColor release];
	[_bottomLineColor release];
    [super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect {
    NSRect bounds = [self bounds];
	
	[[self gradient] drawInRect:bounds angle:[self angle]];
	
	if ([self drawsTopLine]) {
		[[self topLineColor] setFill];
		NSRectFill(NSMakeRect(bounds.origin.x, bounds.origin.y+NSHeight(bounds)-1.0, bounds.size.width, 1.0));
	}
	
	if ([self drawsBottomLine]) {
		[[self bottomLineColor] setFill];
		NSRectFill(NSMakeRect(bounds.origin.x, bounds.origin.y, bounds.size.width, 1.0));
	}
}

- (void)commonInit; {
	
}

@synthesize gradient=_gradient;
@synthesize topLineColor=_topLineColor;
@synthesize bottomLineColor=_bottomLineColor;
@synthesize drawsTopLine=_drawsTopLine;
@synthesize drawsBottomLine=_drawsBottomLine;
@synthesize angle=_angle;
@end
