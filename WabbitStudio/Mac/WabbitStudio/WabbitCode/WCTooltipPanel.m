//
//  WCTooltipPanel.m
//  WabbitStudio
//
//  Created by William Towe on 5/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTooltipPanel.h"


@implementation WCTooltipPanel

- (id)initWithContentRect:(NSRect)contentRect styleMask:(NSUInteger)aStyle backing:(NSBackingStoreType)bufferingType defer:(BOOL)flag {
	if (!(self = [super initWithContentRect:contentRect styleMask:NSBorderlessWindowMask backing:bufferingType defer:flag]))
		return nil;
	
	[self setBackgroundColor:[NSColor clearColor]];
	
	return self;
}

@end
