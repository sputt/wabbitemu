//
//  WCBuildMessagesTextFieldCell.m
//  WabbitStudio
//
//  Created by William Towe on 5/8/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBuildMessagesTextFieldCell.h"


@implementation WCBuildMessagesTextFieldCell

- (NSColor *)badgeFillColor {
	if ([self isHighlighted])
		return [super badgeFillColor];
	else if ([self hasOnlyWarnings])
		return [NSColor colorWithCalibratedRed:0.965 green:0.89 blue:0.58 alpha:1.0];
	return [NSColor colorWithCalibratedRed:0.75 green:0.0 blue:0.0 alpha:1.0];
}

- (NSColor *)badgeTextColor {
	if ([self hasOnlyWarnings] && ![self isHighlighted])
		return [NSColor textColor];
	return [super badgeTextColor];
}

@synthesize hasOnlyWarnings=_hasOnlyWarnings;

@end
