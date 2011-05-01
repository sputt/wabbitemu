//
//  WEApplicationDelegateDelegate.m
//  wabbitcomp
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEApplicationDelegate.h"
#import "WELCDView.h"
#import "WEPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "WEPreferencesWindowController.h"


@implementation WEApplicationDelegate
- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender {
	return NO;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
	WEGeneralOnStartup action = [[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWEPreferencesGeneralOnStartupKey];
	switch (action) {
		case WEGeneralOnStartupShowOpenPanel:
			[[NSDocumentController sharedDocumentController] openDocument:nil];
			break;
		case WEGeneralOnStartupOpenMostRecentRomOrSavestate:
			if ([[[NSDocumentController sharedDocumentController] recentDocumentURLs] count] > 0)
				[[NSDocumentController sharedDocumentController] openDocumentWithContentsOfURL:[[[NSDocumentController sharedDocumentController] recentDocumentURLs] objectAtIndex:0] display:YES error:NULL];
			else
				[[NSDocumentController sharedDocumentController] openDocument:nil];
			break;
		case WEGeneralOnStartupDoNothing:
		default:
			break;
	}
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
			
			_FPSTimer = [[NSTimer alloc] initWithFireDate:[NSDate date] interval:1.0 target:self selector:@selector(_FPSTimerFired:) userInfo:nil repeats:YES];
			// the normal mode for timers
			[[NSRunLoop currentRunLoop] addTimer:_FPSTimer forMode:NSDefaultRunLoopMode];
			// we have to add the timer for this mode so it will fire while the user is using a menu
			[[NSRunLoop currentRunLoop] addTimer:_FPSTimer forMode:NSEventTrackingRunLoopMode];
		}
	}
}
- (void)removeLCDView:(WELCDView *)LCDView; {
	[_LCDViews removeObject:LCDView];
	
	if ([_LCDViews count] == 0) {
		[_timer invalidate];
		[_timer release];
		_timer = nil;
		[_FPSTimer invalidate];
		[_FPSTimer release];
		_FPSTimer = nil;
	}
}

- (IBAction)preferences:(id)sender; {
	[[WEPreferencesWindowController sharedPrefsWindowController] showWindow:nil];
}

- (void)_timerFired:(NSTimer *)timer {
	calc_run_all();
	
	for (WELCDView *LCDView in _LCDViews)
		[LCDView setNeedsDisplay:YES];
	
	[timer setFireDate:[NSDate dateWithTimeIntervalSinceNow:1.0/FPS]];
}

- (void)_FPSTimerFired:(NSTimer *)timer {
	for (WELCDView *LCDView in _LCDViews)
		[[LCDView calculator] updateFPSString];
	
	[timer setFireDate:[NSDate dateWithTimeIntervalSinceNow:1.0]];
}
@end
