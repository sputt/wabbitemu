//
//  WCTopBarView.m
//  WabbitStudio
//
//  Created by William Towe on 3/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTopBarView.h"


@implementation WCTopBarView

- (void)commonInit {
	[self setGradient:[[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedWhite:253.0/255.0 alpha:1.0],0.0,[NSColor colorWithCalibratedWhite:242.0/255.0 alpha:1.0],0.45454,[NSColor colorWithCalibratedWhite:230.0/255.0 alpha:1.0],0.45454,[NSColor colorWithCalibratedWhite:230.0/255.0 alpha:1.0],1.0,nil] autorelease]];
	[self setBottomLineColor:[NSColor colorWithCalibratedWhite:190.0/255.0 alpha:1.0]];
}

- (CGFloat)angle {
	return -90.0;
}

- (BOOL)drawsBottomLine {
	return YES;
}
@end
