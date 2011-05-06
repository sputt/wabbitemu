//
//  RSMemoryTableView.m
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerMemoryTableView.h"
#import "RSDebuggerMemoryTableHeaderView.h"
#import "WCTwoDigitHexFormatter.h"
#import "RSDebuggerMemoryViewController.h"
#import "RSDebuggerMemoryGotoAddressSheetController.h"
#import "WCVerticallyCenteredTextFieldCell.h"


@implementation RSDebuggerMemoryTableView

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	[self setHeaderView:[[[RSDebuggerMemoryTableHeaderView alloc] init] autorelease]];
	
	for (NSTableColumn *tableColumn in [self tableColumns]) {
		//[tableColumn setDataCell:[[[WCVerticallyCenteredTextFieldCell alloc] init] autorelease]];
		//[[tableColumn dataCell] setControlSize:NSSmallControlSize];
		//[[tableColumn dataCell] setFormatter:[[[WCHexFormatter alloc] init] autorelease]];
		//[[tableColumn dataCell] setFont:[NSFont fontWithName:@"Menlo" size:[NSFont systemFontSizeForControlSize:[[tableColumn dataCell] controlSize]]]];
	}
	
	return self;
}

- (void)dealloc {
    [super dealloc];
}

- (void)addTableColumn:(NSTableColumn *)tableColumn {
	//[tableColumn setDataCell:[[[WCVerticallyCenteredTextFieldCell alloc] init] autorelease]];
	//[[tableColumn dataCell] setControlSize:NSSmallControlSize];
	//[[tableColumn dataCell] setFormatter:[[[WCTwoDigitHexFormatter alloc] init] autorelease]];
	//[[tableColumn dataCell] setFont:[NSFont fontWithName:@"Menlo" size:[NSFont systemFontSizeForControlSize:[[tableColumn dataCell] controlSize]]]];
	[super addTableColumn:tableColumn];
}

- (IBAction)gotoAddress:(id)sender; {
	[RSDebuggerMemoryGotoAddressSheetController presentGotoAddressSheetForTableViewController:_tableViewController];
}

@end
