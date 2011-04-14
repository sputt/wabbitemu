//
//  WCContextualMenuTableView.m
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCContextualMenuTableView.h"


@implementation WCContextualMenuTableView

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

@end
