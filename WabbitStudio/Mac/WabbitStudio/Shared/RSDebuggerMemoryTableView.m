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


@implementation RSDebuggerMemoryTableView

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	[self setHeaderView:[[[RSDebuggerMemoryTableHeaderView alloc] init] autorelease]];
	
	return self;
}

- (IBAction)gotoAddress:(id)sender; {
	[RSDebuggerGotoAddressSheetController presentGotoAddressSheetForGotoAddressController:_gotoAddressController];
}

@end
