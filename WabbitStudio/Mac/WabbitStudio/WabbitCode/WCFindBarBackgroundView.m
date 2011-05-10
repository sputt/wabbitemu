//
//  WCFindBarBackgroundView.m
//  WabbitStudio
//
//  Created by William Towe on 4/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFindBarBackgroundView.h"
#import "NSGradient+WCExtensions.h"


@implementation WCFindBarBackgroundView

- (void)commonInit {
	[self setGradient:[NSGradient unifiedSelectedGradient]];
	[self setBottomLineColor:[NSColor colorWithCalibratedWhite:0.6 alpha:1.0]];
}

- (CGFloat)angle {
	return 90.0;
}

- (BOOL)drawsBottomLine {
	return YES;
}
@end
