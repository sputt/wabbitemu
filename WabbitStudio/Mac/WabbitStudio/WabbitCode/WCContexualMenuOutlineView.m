//
//  WCContexualMenuOutlineView.m
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCContexualMenuOutlineView.h"
#import "WCDefines.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"


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

- (void)keyDown:(NSEvent *)event {
	switch ([event keyCode]) {
		case WCReturnKeyCode:
		case WCEnterKeyCode:
			if ([[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesFilesOpenWithKey] == WCPreferencesFilesOpenWithSingleClick)
				[self sendAction:[self action] to:[self target]];
			else if ([[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesFilesOpenWithKey] == WCPreferencesFilesOpenWithDoubleClick)
				[self sendAction:[self doubleAction] to:[self target]];
			return;
		default:
			break;
	}
	[super keyDown:event];
}

- (void)mouseDown:(NSEvent *)event {
	// auto expand rows with children on a double click
	if ([event type] == NSLeftMouseDown &&
		[event clickCount] == 2) {
		
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
	
	[super mouseDown:event];
}

@end
