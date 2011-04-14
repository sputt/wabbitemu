//
//  WCFindBarBackgroundView.m
//  WabbitStudio
//
//  Created by William Towe on 4/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFindBarBackgroundView.h"


@implementation WCFindBarBackgroundView

- (void)commonInit {
	[self setGradient:[[[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:0.82 alpha:1.0] endingColor:[NSColor colorWithCalibratedWhite:0.93 alpha:1.0]] autorelease]];
	[self setBottomLineColor:[NSColor colorWithCalibratedWhite:0.6 alpha:1.0]];
}

- (CGFloat)angle {
	return 90.0;
}

- (BOOL)drawsBottomLine {
	return YES;
}
@end
