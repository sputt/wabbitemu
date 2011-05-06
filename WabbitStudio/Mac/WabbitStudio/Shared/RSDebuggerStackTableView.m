//
//  RSDebuggerStackTableView.m
//  WabbitStudio
//
//  Created by William Towe on 5/6/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerStackTableView.h"
#import "RSDebuggerGotoAddressSheetController.h"
#import "RSCalculator.h"


NSString *const kRSDebuggerStackTableColumnAddressIdentifier = @"address";
NSString *const kRSDebuggerStackTableColumnStackIdentifier = @"stack";

@implementation RSDebuggerStackTableView

- (IBAction)setStackPointerToSelectedAddress:(id)sender; {
	if ([self clickedRow] == -1) {
		NSBeep();
		return;
	}
	
	NSNumber *addressAtRow = [[self dataSource] tableView:self objectValueForTableColumn:[self tableColumnWithIdentifier:kRSDebuggerStackTableColumnAddressIdentifier] row:[self clickedRow]];
	
	[[(id <RSGotoAddressController>)[self dataSource] calculator] setStackPointer:[addressAtRow unsignedShortValue]];
}

- (IBAction)gotoAddress:(id)sender; {
	[RSDebuggerGotoAddressSheetController presentGotoAddressSheetForGotoAddressController:(id <RSGotoAddressController>)[self dataSource]];
}

- (void)gotoStackPointer:(id)sender {
	[(id <RSGotoAddressController>)[self dataSource] scrollToAddress:[[(id <RSGotoAddressController>)[self dataSource] calculator] stackPointer]];
}

@end
