//
//  WCGradientTextFieldCell.m
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCGradientTextFieldCell.h"

@implementation WCGradientTextFieldCell

- (NSBackgroundStyle)backgroundStyle {
	return ([self drawGradient])?NSBackgroundStyleRaised:[super backgroundStyle];
}

@synthesize drawGradient=_drawGradient;
@end
