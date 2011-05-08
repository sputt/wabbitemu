//
//  RSDebuggerMemoryTableColumn.m
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerMemoryTableColumn.h"
#import "RSVerticallyCenteredTextFieldCell.h"
#import "WCTwoDigitHexFormatter.h"


@implementation RSDebuggerMemoryTableColumn

- (id)initWithIdentifier:(id)identifier {
	if (!(self = [super initWithIdentifier:identifier]))
		return nil;
	
	[self setResizingMask:NSTableColumnNoResizing];
	[[self headerCell] setTitle:NSLocalizedString(@"Memory",@"memory view memory table column title")];
	[self setHeaderToolTip:NSLocalizedString(@"Memory view", @"memory view memory table column tooltip")];
	
	RSVerticallyCenteredTextFieldCell *dataCell = [[[RSVerticallyCenteredTextFieldCell alloc] initTextCell:@""] autorelease];

	[dataCell setAlignment:NSCenterTextAlignment];
	[dataCell setControlSize:NSSmallControlSize];
	[dataCell setFont:[NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:[dataCell controlSize]]]];
	[dataCell setFormatter:[[[WCTwoDigitHexFormatter alloc] init] autorelease]];
	[dataCell setEditable:YES];
	
	[self setDataCell:dataCell];
	[self setEditable:YES];
	
	return self;
}

@end
