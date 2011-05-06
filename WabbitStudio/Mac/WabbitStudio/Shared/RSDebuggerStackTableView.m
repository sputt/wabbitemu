//
//  RSDebuggerStackTableView.m
//  WabbitStudio
//
//  Created by William Towe on 5/6/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerStackTableView.h"
#import "RSDebuggerGotoAddressSheetController.h"


@implementation RSDebuggerStackTableView

- (IBAction)gotoAddress:(id)sender; {
	[RSDebuggerGotoAddressSheetController presentGotoAddressSheetForGotoAddressController:(id <RSGotoAddressController>)[self dataSource]];
}

@end
