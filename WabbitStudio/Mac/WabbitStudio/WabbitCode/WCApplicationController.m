//
//  WCApplicationController.m
//  WabbitStudio
//
//  Created by William Towe on 4/30/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCApplicationController.h"
#import "WELCDView.h"
#include "calc.h"


@implementation WCApplicationController

+ (WCApplicationController *)sharedApplicationController; {
	return [self sharedController];
}

- (void)addLCDView:(WELCDView *)LCDView; {
	if (_LCDViews == nil)
		_LCDViews = [[NSMutableArray alloc] initWithCapacity:MAX_CALCS];
	
	if (![_LCDViews containsObject:LCDView]) {
		[_LCDViews addObject:LCDView];
		
		if (!_timer) {
			_timer = [[NSTimer alloc] initWithFireDate:[NSDate date] interval:1.0/FPS target:self selector:@selector(_timerFired:) userInfo:nil repeats:YES];
			// the normal mode for timers
			[[NSRunLoop currentRunLoop] addTimer:_timer forMode:NSDefaultRunLoopMode];
			// we have to add the timer for this mode so it will fire while the user is using a menu
			[[NSRunLoop currentRunLoop] addTimer:_timer forMode:NSEventTrackingRunLoopMode];
		}
	}
}
- (void)removeLCDView:(WELCDView *)LCDView; {
	[_LCDViews removeObject:LCDView];
	
	if ([_LCDViews count] == 0) {
		[_timer invalidate];
		[_timer release];
		_timer = nil;
	}
}

- (void)_timerFired:(NSTimer *)timer {
	calc_run_all();
	
	for (WELCDView *LCDView in _LCDViews)
		[LCDView setNeedsDisplay:YES];
	
	[timer setFireDate:[NSDate dateWithTimeIntervalSinceNow:1.0/FPS]];
}

@end
