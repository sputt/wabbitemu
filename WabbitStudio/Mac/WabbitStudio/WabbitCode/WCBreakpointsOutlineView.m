//
//  WCBreakpointsOutlineView.m
//  WabbitStudio
//
//  Created by William Towe on 4/19/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBreakpointsOutlineView.h"
#import "NSView+WCExtensions.h"


@implementation WCBreakpointsOutlineView

- (BOOL)shouldDrawBackgroundString {
	return ([[[self itemAtRow:0] childNodes] count] == 0);
}

- (NSString *)backgroundString {
	return NSLocalizedString(@"No Breakpoints", @"No Breakpoints");
}

- (void)drawBackgroundInClipRect:(NSRect)clipRect {
	[super drawBackgroundInClipRect:clipRect];
	
	[self drawBackgroundString];
}

- (BOOL)drawsGroupBackgroundForRootItems {
	return NO;
}
@end
