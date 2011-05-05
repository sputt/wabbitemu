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
#import "RSDebuggerMemoryViewController.h"


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
	[_verticalSplitView replaceSubview:[[_verticalSplitView subviews] lastObject] with:[_detailsViewController view]];
	
	_memoryViewController = [[RSDebuggerMemoryViewController alloc] initWithCalculator:[[self calculatorDocument] calculator]];
	
	[[_memoryViewController view] setFrameSize:[[[_horizontalSplitView subviews] lastObject] frame].size];
	[_horizontalSplitView replaceSubview:[[_horizontalSplitView subviews] lastObject] with:[_memoryViewController view]];
	
	[_memoryViewController scrollToAddress:[[[self calculatorDocument] calculator] programCounter]];
}

- (NSString *)windowTitleForDocumentDisplayName:(NSString *)displayName {
	return [NSString stringWithFormat:NSLocalizedString(@"%@ - Debugger", @"debugger window title"),displayName];
}

- (void)windowWillClose:(NSNotification *)notification {
	[[self calculatorDocument] setIsDebugging:NO];
	[[[self calculatorDocument] calculator] setIsRunning:YES];
}

- (BOOL)splitView:(NSSplitView *)splitView shouldAdjustSizeOfSubview:(NSView *)view {
	if ([splitView isVertical] && [[splitView subviews] lastObject] == view)
		return NO;
	else if (![splitView isVertical] && [[splitView subviews] lastObject] == view)
		return NO;
	return YES;
}

@dynamic calculatorDocument;
- (WECalculatorDocument *)calculatorDocument {
	return (WECalculatorDocument *)[self document];
}

@end
