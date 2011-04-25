//
//  WCContexualMenuOutlineView.m
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCContexualMenuOutlineView.h"


@implementation WCContexualMenuOutlineView

- (NSMenu *)menuForEvent:(NSEvent *)event {
	if (([event type] == NSLeftMouseDown && ([event modifierFlags] & NSControlKeyMask)) ||
		([event type] == NSRightMouseDown)) {
		
		NSInteger row = [self rowAtPoint:[self convertPointFromBase:[event locationInWindow]]];
		
		if (row != -1) {
			if (![[self selectedRowIndexes] containsIndex:row])
				[self selectRowIndexes:[NSIndexSet indexSetWithIndex:row] byExtendingSelection:NO];
		}
		return [self menu];
	}
	return nil;
}

- (void)mouseDown:(NSEvent *)theEvent {
	// auto expand rows with children on a double click
	if ([theEvent type] == NSLeftMouseDown &&
		[theEvent clickCount] == 2) {
		
		if ([[self selectedRowIndexes] count] == 1) {
			NSUInteger fIndex = [[self selectedRowIndexes] firstIndex];
			if (([[self dataSource] isKindOfClass:[NSTreeController class]] &&
				![[[self itemAtRow:fIndex] representedObject] isLeaf]) ||
				![[self itemAtRow:fIndex] isLeaf]) {
				if ([self isItemExpanded:[self itemAtRow:fIndex]])
					[self collapseItem:[self itemAtRow:fIndex]];
				else
					[self expandItem:[self itemAtRow:fIndex]];
				
				return;
			}
		}
	}
	
	[super mouseDown:theEvent];
}

@end
