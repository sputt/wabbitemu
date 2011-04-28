//
//  WECalculatorTopBarView.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WECalculatorTopBarView.h"
#import "NSGradient+WCExtensions.h"


@implementation WECalculatorTopBarView

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super dealloc];
}

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_appDidBecomeActive:) name:NSApplicationDidBecomeActiveNotification object:[NSApplication sharedApplication]];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_appDidResignActive:) name:NSApplicationDidResignActiveNotification object:[NSApplication sharedApplication]];
	
	return self;
}

- (id)initWithFrame:(NSRect)frameRect {
	if (!(self = [super initWithFrame:frameRect]))
		return nil;
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_appDidBecomeActive:) name:NSApplicationDidBecomeActiveNotification object:[NSApplication sharedApplication]];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_appDidResignActive:) name:NSApplicationDidResignActiveNotification object:[NSApplication sharedApplication]];
	
	return self;
}

- (NSGradient *)gradient {
	if ([[NSApplication sharedApplication] isActive])
		return [NSGradient unifiedPressedGradient];
	return [NSGradient unifiedSelectedGradient];
}

- (NSColor *)bottomLineColor {
	if ([[NSApplication sharedApplication] isActive])
		return [NSColor darkGrayColor];
	return [NSColor lightGrayColor];
}

- (BOOL)drawsBottomLine {
	return YES;
}

- (CGFloat)angle {
	return 90.0;
}

- (void)_appDidBecomeActive:(NSNotification *)note {
	[self setNeedsDisplay:YES];
}

- (void)_appDidResignActive:(NSNotification *)note {
	[self setNeedsDisplay:YES];
}
@end
