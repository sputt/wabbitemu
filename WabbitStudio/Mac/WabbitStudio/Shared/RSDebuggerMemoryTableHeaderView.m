//
//  RSDebuggerMemoryHeaderView.m
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerMemoryTableHeaderView.h"


@implementation RSDebuggerMemoryTableHeaderView

- (NSRect)headerRectOfColumn:(NSInteger)index {
	if (index == 0)
		return [super headerRectOfColumn:index];
	
	NSUInteger columns = [[self tableView] numberOfColumns];
	NSRect unionRect = NSZeroRect;
	NSUInteger i;
	
	for (i = 1; i < columns; i++)
		unionRect = NSUnionRect(unionRect, [super headerRectOfColumn:i]);

	return unionRect;
}

@end
