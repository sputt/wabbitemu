//
//  WCDebuggerWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 4/30/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCDebuggerWindowController.h"
#import "WCApplicationController.h"
#import "WELCDView.h"
#import "TLAnimatingOutlineView.h"


@implementation WCDebuggerWindowController

- (id)init {
	return [super initWithWindowNibName:[self windowNibName]];
}

- (void)dealloc {
    [super dealloc];
}

- (NSString *)windowNibName {
	return @"WCDebuggerWindow";
}

- (void)windowDidLoad {
    [super windowDidLoad];
	
	[[self LCDView] setCalculator:[[self calculatorOwner] calculator]];
	[[WCApplicationController sharedApplicationController] addLCDView:_LCDView];
	
	[[self animatingOutlineView] addView:_registersView withImage:[NSImage imageNamed:NSImageNamePreferencesGeneral] label:NSLocalizedString(@"Registers", @"registers view label") expanded:YES];
	[[self animatingOutlineView] addView:_flagsView withImage:[NSImage imageNamed:NSImageNameDotMac] label:NSLocalizedString(@"Flags", @"registers view label") expanded:YES];
	
	[_LCDDrawer toggle:nil];
}



- (void)windowWillClose:(NSNotification *)notification {
	[[WCApplicationController sharedApplicationController] removeLCDView:_LCDView];
	[[self calculatorOwner] setIsDebugging:NO];
}

@dynamic calculatorOwner;
- (id <RSCalculatorOwner>)calculatorOwner {
	return (id <RSCalculatorOwner>)[self document];
}
@synthesize LCDView=_LCDView;
@synthesize animatingOutlineView=_animatingOutlineView;
@end
