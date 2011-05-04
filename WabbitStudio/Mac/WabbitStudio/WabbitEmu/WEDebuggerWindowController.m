//
//  WEDebuggerWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 5/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEDebuggerWindowController.h"
#import "RSDebuggerDetailsViewController.h"
#import "WECalculatorDocument.h"
#import "RSCalculator.h"


@implementation WEDebuggerWindowController

- (id)init {
	return [super initWithWindowNibName:[self windowNibName]];
}

- (void)dealloc {
	[_detailsViewController release];
    [super dealloc];
}

- (NSString *)windowNibName {
	return @"WEDebuggerWindow";
}

- (void)windowDidLoad {
    [super windowDidLoad];
    
    _detailsViewController = [[RSDebuggerDetailsViewController alloc] initWithCalculator:[[self calculatorDocument] calculator]];
	
	[[_detailsViewController view] setFrameSize:[[[_verticalSplitView subviews] lastObject] frame].size];
	[[[_verticalSplitView subviews] lastObject] addSubview:[_detailsViewController view]];
}

- (NSString *)windowTitleForDocumentDisplayName:(NSString *)displayName {
	return [NSString stringWithFormat:NSLocalizedString(@"%@ - Debugger", @"debugger window title"),displayName];
}

- (void)windowWillClose:(NSNotification *)notification {
	[[self calculatorDocument] setIsDebugging:NO];
	[[[self calculatorDocument] calculator] setIsRunning:YES];
}

@dynamic calculatorDocument;
- (WECalculatorDocument *)calculatorDocument {
	return (WECalculatorDocument *)[self document];
}

@end
