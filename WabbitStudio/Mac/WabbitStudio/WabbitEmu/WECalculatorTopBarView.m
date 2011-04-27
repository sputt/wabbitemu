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

- (NSGradient *)gradient {
	return [NSGradient unifiedPressedGradient];
}

- (NSColor *)bottomLineColor {
	return [NSColor darkGrayColor];
}

- (BOOL)drawsBottomLine {
	return YES;
}

- (CGFloat)angle {
	return 90.0;
}
@end
