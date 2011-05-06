//
//  RSDebuggerMemoryGotoAddressSheetController.m
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerGotoAddressSheetController.h"
#import "RSDebuggerMemoryViewController.h"


@interface RSDebuggerGotoAddressSheetController ()
- (id)_initWithTableViewController:(id <RSGotoAddressController>)gotoAddressController;
@end

@implementation RSDebuggerGotoAddressSheetController

- (NSString *)windowNibName {
	return @"RSDebuggerGotoAddressSheet";
}

- (void)dealloc {
	_gotoAddressController = nil;
    [super dealloc];
}

+ (void)presentGotoAddressSheetForGotoAddressController:(id <RSGotoAddressController>)gotoAddressController; {
	RSDebuggerGotoAddressSheetController *controller = [[[self class] alloc] _initWithTableViewController:gotoAddressController];
	
	[[NSApplication sharedApplication] beginSheet:[controller window] modalForWindow:[[gotoAddressController view] window] modalDelegate:controller didEndSelector:@selector(_sheetDidEnd:code:info:) contextInfo:NULL];
}

@synthesize address=_address;
@synthesize gotoAddressController=_gotoAddressController;

- (id)_initWithTableViewController:(id <RSGotoAddressController>)gotoAddressController; {
	if (!(self = [super initWithWindowNibName:[self windowNibName]]))
		return nil;
	
	_gotoAddressController = gotoAddressController;
	
	return self;
}

- (void)_sheetDidEnd:(NSWindow *)sheet code:(NSInteger)code info:(void *)info {
	[self autorelease];
	if (code != NSOKButton)
		return;
	
	[[self gotoAddressController] scrollToAddress:[self address]];
}

@end
