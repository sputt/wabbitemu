//
//  WEPreferencesWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEPreferencesWindowController.h"
#import "WECalculator.h"
#import "WEPreviewLCDView.h"
#import "WEApplicationDelegate.h"
#import "WCDefines.h"


@implementation WEPreferencesWindowController

+ (NSString *)nibName {
	return @"WEPreferencesWindow";
}

- (void)setupToolbar {
	[self addView:_general label:NSLocalizedString(@"General", @"General") image:[NSImage imageNamed:NSImageNamePreferencesGeneral]];
	[self addView:_calculator label:NSLocalizedString(@"Hardware", @"Hardware")];
	[self addView:_advanced label:NSLocalizedString(@"Advanced", @"Advanced")];
}

- (void)windowDidLoad { 
	[super windowDidLoad];
	
	[[self window] setDelegate:self];
	
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	
	if ([[[NSDocumentController sharedDocumentController] documents] count] > 0) {
		WECalculator *calculator = [[[NSDocumentController sharedDocumentController] documents] objectAtIndex:0];
		
		[_LCDView setCalc:[calculator calc]];
		[WEApplicationDelegate addLCDView:_LCDView];
	}
	else {
		
	}
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_calculatorWillClose:) name:kWECalculatorWillCloseNotification object:nil];
}

- (void)windowWillClose:(NSNotification *)notification {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[_LCDView setCalc:NULL];
	[WEApplicationDelegate removeLCDView:_LCDView];
}

- (void)windowDidBecomeMain:(NSNotification *)notification {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	if ([[[NSDocumentController sharedDocumentController] documents] count] > 0) {
		WECalculator *calculator = [[[NSDocumentController sharedDocumentController] documents] objectAtIndex:0];
		
		[_LCDView setCalc:[calculator calc]];
		[WEApplicationDelegate addLCDView:_LCDView];
	}
}

- (void)_calculatorWillClose:(NSNotification *)note {
	if ([[note object] calc] == [_LCDView calc]) {
		[_LCDView setCalc:NULL];
		[WEApplicationDelegate removeLCDView:_LCDView];
	}
}
@end
