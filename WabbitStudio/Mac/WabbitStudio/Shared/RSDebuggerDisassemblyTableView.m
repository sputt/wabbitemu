//
//  RSDebuggerDisassemblyTableView.m
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerDisassemblyTableView.h"
#import "RSCalculator.h"
#import "RSDebuggerDisassemblyViewController.h"


@implementation RSDebuggerDisassemblyTableView

- (void)highlightSelectionInClipRect:(NSRect)clipRect {
	[super highlightSelectionInClipRect:clipRect];
	
	RSCalculator *calculator = [(RSDebuggerDisassemblyViewController *)[self dataSource] calculator];
	NSRange rowRange = [self rowsInRect:clipRect];
	NSUInteger rowIndex;
	uint16_t programCounter = [[(RSDebuggerDisassemblyViewController *)[self dataSource] calculator] programCounter];
	
	for (rowIndex = rowRange.location; rowIndex < NSMaxRange(rowRange); rowIndex++) {
		NSNumber *addressAtRow = [[self dataSource] tableView:self objectValueForTableColumn:[self tableColumnWithIdentifier:kDisassemblyTableColumnAddressIdentifier] row:rowIndex];
		
		if (programCounter == [addressAtRow unsignedShortValue]) {
			NSRect rowRect = [self rectOfRow:rowIndex];
			rowRect.size.height -= floor([self intercellSpacing].height/2.0);
			
			if ([[self selectedRowIndexes] containsIndex:rowIndex]) {
				NSGradient *gradient = [[[NSGradient alloc] initWithStartingColor:([[self window] firstResponder] == self && [NSApp keyWindow] == [self window])?[NSColor alternateSelectedControlColor]:[NSColor secondarySelectedControlColor] endingColor:([calculator CPUHalt])?[NSColor orangeColor]:[NSColor yellowColor]] autorelease];
				[gradient drawInRect:rowRect angle:180.0];
			}
			else {
				[([calculator CPUHalt])?[NSColor orangeColor]:[NSColor yellowColor] setFill];
				NSRectFill(rowRect);
			}
		}
	}
}

@end
