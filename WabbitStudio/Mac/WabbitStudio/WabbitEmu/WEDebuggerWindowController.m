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
#import "RSDebuggerDisassemblyViewController.h"


NSString *const kRSDebuggerWindowToolbarStepIdentifier = @"stepIdentifier";
NSString *const kRSDebuggerWindowToolbarStepOverIdentifier = @"stepOverIdentifier";

@implementation WEDebuggerWindowController

- (id)init {
	return [super initWithWindowNibName:[self windowNibName]];
}

- (void)dealloc {
	[_detailsViewController release];
	[_memoryViewController release];
	[_disassemblyViewController release];
    [super dealloc];
}

- (NSString *)windowNibName {
	return @"WEDebuggerWindow";
}

- (void)windowDidLoad {
    [super windowDidLoad];
    
	NSToolbar *toolbar = [[[NSToolbar alloc] initWithIdentifier:@"WEDebuggerWindowToolbar"] autorelease];
	
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeRegular];
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setDelegate:self];
	
#ifndef DEBUG
	[toolbar setAutosavesConfiguration:YES];
#endif
	
	[[self window] setToolbar:toolbar];
	
    _detailsViewController = [[RSDebuggerDetailsViewController alloc] initWithCalculator:[[self calculatorDocument] calculator]];
	
	[[_detailsViewController view] setFrameSize:[[[_verticalSplitView subviews] lastObject] frame].size];
	[_verticalSplitView replaceSubview:[[_verticalSplitView subviews] lastObject] with:[_detailsViewController view]];
	
	_memoryViewController = [[RSDebuggerMemoryViewController alloc] initWithCalculator:[[self calculatorDocument] calculator]];
	
	[[_memoryViewController view] setFrameSize:[[[_horizontalSplitView subviews] lastObject] frame].size];
	[_horizontalSplitView replaceSubview:[[_horizontalSplitView subviews] lastObject] with:[_memoryViewController view]];
	
	[_memoryViewController scrollToAddress:[[[self calculatorDocument] calculator] programCounter]];
	
	_disassemblyViewController = [[RSDebuggerDisassemblyViewController alloc] initWithCalculator:[[self calculatorDocument] calculator]];
	[[_disassemblyViewController view] setFrameSize:[[[_horizontalSplitView subviews] objectAtIndex:0] frame].size];
	[_horizontalSplitView replaceSubview:[[_horizontalSplitView subviews] objectAtIndex:0] with:[_disassemblyViewController view]];
}

- (NSString *)windowTitleForDocumentDisplayName:(NSString *)displayName {
	return [NSString stringWithFormat:NSLocalizedString(@"%@ - Debugger", @"debugger window title"),displayName];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar {
	return [NSArray arrayWithObjects:kRSDebuggerWindowToolbarStepIdentifier,kRSDebuggerWindowToolbarStepOverIdentifier, nil];
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar {
	return [NSArray arrayWithObjects:kRSDebuggerWindowToolbarStepIdentifier,kRSDebuggerWindowToolbarStepOverIdentifier, nil];
}

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSString *)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag {
	NSToolbarItem *item = [[[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier] autorelease];
	
	if ([itemIdentifier isEqualToString:kRSDebuggerWindowToolbarStepIdentifier]) {
		[item setLabel:NSLocalizedString(@"Step", @"step toolbar item label")];
		[item setPaletteLabel:[item label]];
		[item setImage:[NSImage imageNamed:@"Step32x32"]];
		[item setAction:@selector(step:)];
	}
	else if ([itemIdentifier isEqualToString:kRSDebuggerWindowToolbarStepOverIdentifier]) {
		[item setLabel:NSLocalizedString(@"Step Over", @"step over toolbar item label")];
		[item setPaletteLabel:[item label]];
		[item setImage:[NSImage imageNamed:@"StepOver32x32"]];
		[item setAction:@selector(stepOver:)];
	}
	return item;
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
