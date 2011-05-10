//
//  WCFileViewStatusBarView.m
//  WabbitStudio
//
//  Created by William Towe on 4/15/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFileViewStatusBarView.h"
#import "NSGradient+WCExtensions.h"


@implementation WCFileViewStatusBarView

- (void)commonInit {
	[self setGradient:[NSGradient unifiedNormalGradient]];
	[self setBottomLineColor:[NSColor colorWithCalibratedWhite:0.6 alpha:1.0]];
}

- (CGFloat)angle {
	return 90.0;
}

- (BOOL)drawsBottomLine {
	return YES;
}

@end
