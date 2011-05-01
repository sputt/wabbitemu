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
    
	[[self LCDView] setCalculator:[self calculator]];
	[[self LCDView] setCalc:[[self calculator] calc]];
	[[WCApplicationController sharedApplicationController] addLCDView:_LCDView];
}

- (void)windowWillClose:(NSNotification *)notification {
	[[WCApplicationController sharedApplicationController] removeLCDView:_LCDView];
	[[self calculator] setIsDebugging:NO];
}

@dynamic calculator;
- (id<RSCalculatorProtocol>)calculator {
	return (id<RSCalculatorProtocol>)[self document];
}
@synthesize LCDView=_LCDView;

@end
