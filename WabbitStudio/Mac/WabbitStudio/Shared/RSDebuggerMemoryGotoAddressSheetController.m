//
//  RSDebuggerMemoryGotoAddressSheetController.m
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerMemoryGotoAddressSheetController.h"
#import "RSDebuggerMemoryViewController.h"


@interface RSDebuggerMemoryGotoAddressSheetController ()
- (id)_initWithTableViewController:(RSDebuggerMemoryViewController *)tableViewController;
@end

@implementation RSDebuggerMemoryGotoAddressSheetController

- (NSString *)windowNibName {
	return @"RSDebuggerMemoryGotoAddressSheet";
}

- (void)dealloc {
	_tableViewController = nil;
    [super dealloc];
}

+ (void)presentGotoAddressSheetForTableViewController:(RSDebuggerMemoryViewController *)tableViewController; {
	RSDebuggerMemoryGotoAddressSheetController *controller = [[[self class] alloc] _initWithTableViewController:tableViewController];
	
	[[NSApplication sharedApplication] beginSheet:[controller window] modalForWindow:[[tableViewController view] window] modalDelegate:controller didEndSelector:@selector(_sheetDidEnd:code:info:) contextInfo:NULL];
}

@synthesize address=_address;
@synthesize tableViewController=_tableViewController;

- (id)_initWithTableViewController:(RSDebuggerMemoryViewController *)tableViewController {
	if (!(self = [super initWithWindowNibName:[self windowNibName]]))
		return nil;
	
	_tableViewController = tableViewController;
	
	return self;
}

- (void)_sheetDidEnd:(NSWindow *)sheet code:(NSInteger)code info:(void *)info {
	[self autorelease];
	if (code != NSOKButton)
		return;
	
	[[self tableViewController] scrollToAddress:[self address]];
}

@end
