//
//  RSMemoryTableView.m
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSMemoryTableView.h"
#import "RSDebuggerMemoryHeaderView.h"
#import "WCTwoDigitHexFormatter.h"


@implementation RSMemoryTableView

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	[self setHeaderView:[[[RSDebuggerMemoryHeaderView alloc] init] autorelease]];
	
	for (NSTableColumn *tableColumn in [self tableColumns]) {
		[[tableColumn dataCell] setControlSize:NSSmallControlSize];
		[[tableColumn dataCell] setFormatter:[[[WCHexFormatter alloc] init] autorelease]];
		[[tableColumn dataCell] setFont:[NSFont fontWithName:@"Menlo" size:[NSFont systemFontSizeForControlSize:[[tableColumn dataCell] controlSize]]]];
	}
	
	return self;
}

- (void)dealloc {
    [super dealloc];
}

- (void)addTableColumn:(NSTableColumn *)tableColumn {
	[[tableColumn dataCell] setControlSize:NSSmallControlSize];
	[[tableColumn dataCell] setFormatter:[[[WCTwoDigitHexFormatter alloc] init] autorelease]];
	[[tableColumn dataCell] setFont:[NSFont fontWithName:@"Menlo" size:[NSFont systemFontSizeForControlSize:[[tableColumn dataCell] controlSize]]]];
	[super addTableColumn:tableColumn];
}

@end
