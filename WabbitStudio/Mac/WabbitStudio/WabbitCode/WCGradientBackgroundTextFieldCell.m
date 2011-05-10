//
//  WCGradientTextFieldCell.m
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCGradientBackgroundTextFieldCell.h"

@implementation WCGradientBackgroundTextFieldCell

- (NSBackgroundStyle)backgroundStyle {
	return ([self drawGradient])?NSBackgroundStyleRaised:[super backgroundStyle];
}

- (NSColor *)badgeBackgroundColor {
	if ([self isHighlighted])
		return [NSColor alternateSelectedControlTextColor];
	else
		return [NSColor colorWithDeviceWhite:(180/255.0) alpha:1.0];
}

@synthesize drawGradient=_drawGradient;
@dynamic gradient;
- (NSGradient *)gradient {
	return [[[NSGradient alloc] initWithStartingColor:[NSColor whiteColor] endingColor:[NSColor lightGrayColor]] autorelease];
}
@dynamic borderColor;
- (NSColor *)borderColor {
	return [NSColor colorWithCalibratedWhite:0.58 alpha:1.0];
}
@end
