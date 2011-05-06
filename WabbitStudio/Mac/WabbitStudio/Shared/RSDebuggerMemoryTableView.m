//
//  RSMemoryTableView.m
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerMemoryTableView.h"
#import "RSDebuggerMemoryTableHeaderView.h"
#import "RSDebuggerGotoAddressSheetController.h"
#import "RSCalculator.h"


@implementation RSDebuggerMemoryTableView

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	[self setHeaderView:[[[RSDebuggerMemoryTableHeaderView alloc] init] autorelease]];
	
	return self;
}

- (IBAction)gotoAddress:(id)sender; {
	[RSDebuggerGotoAddressSheetController presentGotoAddressSheetForGotoAddressController:(id <RSGotoAddressController>)[self dataSource]];
}

- (IBAction)gotoProgramCounter:(id)sender; {
	if ([self clickedRow] == -1) {
		NSBeep();
		return;
	}
	
	[(id <RSGotoAddressController>)[self dataSource] scrollToAddress:[[(id <RSGotoAddressController>)[self dataSource] calculator] programCounter]];
}

@end
