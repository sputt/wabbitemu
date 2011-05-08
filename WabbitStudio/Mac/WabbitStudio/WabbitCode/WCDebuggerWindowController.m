//
//  WCDebuggerWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 4/30/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCDebuggerWindowController.h"
#import "WCApplicationController.h"
#import "RSLCDView.h"
#import "RSCalculator.h"


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
}



- (void)windowWillClose:(NSNotification *)notification {
	[[WCApplicationController sharedApplicationController] removeLCDView:_LCDView];
	[[self calculatorOwner] setIsDebugging:NO];
	[[[self calculatorOwner] calculator] setIsDebugging:NO];
}

@dynamic calculatorOwner;
- (id <RSCalculatorOwner>)calculatorOwner {
	return (id <RSCalculatorOwner>)[self document];
}
@synthesize LCDView=_LCDView;
@end
