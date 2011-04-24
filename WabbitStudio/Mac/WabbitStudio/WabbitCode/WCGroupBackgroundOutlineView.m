//
//  WCTestOutlineView.m
//  WabbitStudio
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCGroupBackgroundOutlineView.h"
#import "NSColor-NTExtensions.h"


@implementation WCGroupBackgroundOutlineView

- (void)drawBackgroundInClipRect:(NSRect)clipRect {
	[super drawBackgroundInClipRect:clipRect];
	
	NSRange rowRange = [self rowsInRect:clipRect];
	
	for (NSUInteger row = rowRange.location; row < NSMaxRange(rowRange); row++) {
		// skip our root item depending on subclass override
		if (![self drawsGroupBackgroundForRootItems] &&
			// we must check to see if our datasource is NSTreeController class and ask the represented object if it's
			// parent node is nil; this is because the NSTreeNode class always returns a parentNode unless its the root
			// node returned from NSTreeController arrangedObjects method
			(([[self dataSource] isKindOfClass:[NSTreeController class]] &&
			[[[self itemAtRow:row] representedObject] parentNode] == nil) ||
			 [[self itemAtRow:row] parentNode] == nil))
			continue;
		// if the item is expanded, start building a range of rows to highlight
		else if ([self isItemExpanded:[self itemAtRow:row]]) {
			// the starting row is the item we just encountered, i.e. the parent that is expanded
			NSUInteger start = row;
			// make sure we dont search farther than the visible rows
			// stop as soon as we encounter a node that is not of child of the current parent
			while (row < NSMaxRange(rowRange) && [[self itemAtRow:row+1] parentNode] == [self itemAtRow:start])
				row++;
			
			// union the start and end row rects together
			NSRect hRect = NSUnionRect([self rectOfRow:++start], [self rectOfRow:row]);
			
			// [[NSColor colorWithCalibratedRed:(229/255.0) green:(233/255.0) blue:(238/255.0) alpha:1.0] setFill];
			// draw a lighter color for the background if we are key window
			if ([[NSApplication sharedApplication] keyWindow] == [self window])
				[[[self backgroundColor] lighterColor:0.55] setFill];
			else
				[[NSColor colorWithCalibratedWhite:0.941 alpha:1.0] setFill];
			NSRectFill(hRect);
			// use the standard grid color for the top and bottom bars
			[[NSColor gridColor] setFill];
			NSRectFill(NSMakeRect(NSMinX(hRect), NSMinY(hRect)-1.0, NSWidth(hRect), 1.0));
			NSRectFill(NSMakeRect(NSMinX(hRect), NSMaxY(hRect)-1.0, NSWidth(hRect), 1.0));
		}
	}
}

// we must override these or the superclass will undo our work
- (void)selectRowIndexes:(NSIndexSet *)indexes byExtendingSelection:(BOOL)extend {
	[super selectRowIndexes:indexes byExtendingSelection:extend];
	[self setNeedsDisplay:YES];
}

- (void)collapseItem:(id)item {
	[super collapseItem:item];
	[self setNeedsDisplay:YES];
}

- (void)expandItem:(id)item {
	[super expandItem:item];
	[self setNeedsDisplay:YES];
}

@dynamic drawsGroupBackgroundForRootItems;
- (BOOL)drawsGroupBackgroundForRootItems {
	return YES;
}
@end
