//
//  RSScreenCaptureIndicatorBar.m
//  WabbitStudio
//
//  Created by William Towe on 5/8/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSScreenCaptureIndicatorBar.h"
#import "WCDefines.h"


@implementation RSScreenCaptureIndicatorBar

- (id)initWithFrame:(NSRect)frameRect {
	if (!(self = [super initWithFrame:frameRect]))
		return nil;
	
	[self setAutoresizingMask:NSViewWidthSizable];
	
	return self;
}

- (BOOL)drawsTopLine { return YES; }
- (NSColor *)topLineColor { return [NSColor gridColor]; }
- (NSGradient *)gradient {
	return [[[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedRed:0.85 green:0.0 blue:0.0 alpha:1.0] endingColor:[NSColor colorWithCalibratedRed:0.6 green:0.0 blue:0.0 alpha:1.0]] autorelease];
}
- (CGFloat)angle { return 90.0; }

- (void)drawRect:(NSRect)dirtyRect {
	[super drawRect:dirtyRect];
	
	NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
	[style setAlignment:NSCenterTextAlignment];
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:style,NSParagraphStyleAttributeName,[NSFont boldSystemFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]],NSFontAttributeName,[NSColor whiteColor],NSForegroundColorAttributeName, nil];
	NSAttributedString *attributedString = [[[NSAttributedString alloc] initWithString:NSLocalizedString(@"Recording\u2026", @"Recording with ellipsis") attributes:attributes] autorelease];
	
	[attributedString drawInRect:WCCenteredRectWithSize(NSMakeSize(NSWidth([self bounds]), [attributedString size].height), [self bounds])];
}

@end
