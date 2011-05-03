//
//  WEPreferencesWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEPreferencesWindowController.h"
#import "WECalculatorDocument.h"
#import "WEPreviewLCDView.h"
#import "WEApplicationDelegate.h"
#import "WCDefines.h"
#import "RSCalculator.h"


@implementation WEPreferencesWindowController

+ (NSString *)nibName {
	return @"WEPreferencesWindow";
}

- (void)setupToolbar {
	[self addView:_general label:NSLocalizedString(@"General", @"General") image:[NSImage imageNamed:NSImageNamePreferencesGeneral]];
	[self addView:_calculator label:NSLocalizedString(@"Calculator", @"Calculator") image:[NSImage imageNamed:@"Hardware"]];
	[self addView:_advanced label:NSLocalizedString(@"Advanced", @"Advanced")];
}

- (void)windowWillLoad {
	[super windowWillLoad];
	
	_mcalculator = [[RSCalculator alloc] initWithOwner:self breakpointSelector:NULL];
	[_mcalculator loadRomOrSavestate:[[NSBundle mainBundle] pathForResource:@"starshooter" ofType:@"sav"] error:NULL];
}

- (void)windowDidLoad { 
	[super windowDidLoad];
	
	[[self window] setDelegate:self];
	
	[_LCDView setCalculator:_mcalculator];
	
	[WEApplicationDelegate addLCDView:_LCDView];
}

@end
