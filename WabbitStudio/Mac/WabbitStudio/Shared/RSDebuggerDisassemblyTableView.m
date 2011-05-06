//
//  RSDebuggerDisassemblyTableView.m
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerDisassemblyTableView.h"
#import "RSCalculator.h"
#import "RSDebuggerGotoAddressSheetController.h"


NSString *const kDisassemblyTableColumnAddressIdentifier = @"address";
NSString *const kDisassemblyTableColumnDataIdentifier = @"data";
NSString *const kDisassemblyTableColumnDisassemblyIdentifier = @"disassembly";
NSString *const kDisassemblyTableColumnSizeIdentifier = @"size";

@implementation RSDebuggerDisassemblyTableView

- (void)highlightSelectionInClipRect:(NSRect)clipRect {
	[super highlightSelectionInClipRect:clipRect];
	
	RSCalculator *calculator = [(id <RSGotoAddressController>)[self dataSource] calculator];
	NSRange rowRange = [self rowsInRect:clipRect];
	NSUInteger rowIndex;
	uint16_t programCounter = [[(id <RSGotoAddressController>)[self dataSource] calculator] programCounter];
	
	for (rowIndex = rowRange.location; rowIndex < NSMaxRange(rowRange); rowIndex++) {
		NSNumber *addressAtRow = [[self dataSource] tableView:self objectValueForTableColumn:[self tableColumnWithIdentifier:kDisassemblyTableColumnAddressIdentifier] row:rowIndex];
		
		if (programCounter == [addressAtRow unsignedShortValue]) {
			NSRect rowRect = [self rectOfRow:rowIndex];
			rowRect.size.height -= floor([self intercellSpacing].height/2.0);
			
			if ([[self selectedRowIndexes] containsIndex:rowIndex]) {
				if ([[(id <RSGotoAddressController>)[self dataSource] calculator] checkBreakpointAtAddress:[addressAtRow unsignedShortValue]] != RSBreakpointTypeNone) {
					RSBreakpointType breakpointType = [[(id <RSGotoAddressController>)[self dataSource] calculator] checkBreakpointAtAddress:[addressAtRow unsignedShortValue]];
					NSGradient *gradient = nil;
					
					if (breakpointType == RSBreakpointTypeNormal)
						gradient = [[[NSGradient alloc] initWithColors:[NSArray arrayWithObjects:([[self window] firstResponder] == self && [NSApp keyWindow] == [self window])?[NSColor alternateSelectedControlColor]:[NSColor secondarySelectedControlColor],[NSColor redColor],([calculator CPUHalt])?[NSColor orangeColor]:[NSColor yellowColor], nil]] autorelease];
					
					[gradient drawInRect:rowRect angle:180.0];
				}
				else {
					[[[[NSGradient alloc] initWithStartingColor:([[self window] firstResponder] == self && [NSApp keyWindow] == [self window])?[NSColor alternateSelectedControlColor]:[NSColor secondarySelectedControlColor] endingColor:([calculator CPUHalt])?[NSColor orangeColor]:[NSColor yellowColor]] autorelease] drawInRect:rowRect angle:180.0];
				}
			}
			else if ([[(id <RSGotoAddressController>)[self dataSource] calculator] checkBreakpointAtAddress:[addressAtRow unsignedShortValue]] != RSBreakpointTypeNone) {
				RSBreakpointType breakpointType = [[(id <RSGotoAddressController>)[self dataSource] calculator] checkBreakpointAtAddress:[addressAtRow unsignedShortValue]];
				NSGradient *gradient = nil;
				
				if (breakpointType == RSBreakpointTypeNormal)
					gradient = [[[NSGradient alloc] initWithStartingColor:[NSColor redColor] endingColor:([calculator CPUHalt])?[NSColor orangeColor]:[NSColor yellowColor]] autorelease];
				
				[gradient drawInRect:rowRect angle:180.0];
			}
			else {
				[([calculator CPUHalt])?[NSColor orangeColor]:[NSColor yellowColor] setFill];
				NSRectFill(rowRect);
			}
		}
		else if ([[(id <RSGotoAddressController>)[self dataSource] calculator] checkBreakpointAtAddress:[addressAtRow unsignedShortValue]] != RSBreakpointTypeNone) {
			RSBreakpointType breakpointType = [[(id <RSGotoAddressController>)[self dataSource] calculator] checkBreakpointAtAddress:[addressAtRow unsignedShortValue]];
			NSRect rowRect = [self rectOfRow:rowIndex];
			rowRect.size.height -= floor([self intercellSpacing].height/2.0);
			
			if ([[self selectedRowIndexes] containsIndex:rowIndex]) {
				NSGradient *gradient = nil;
				
				if (breakpointType == RSBreakpointTypeNormal)
					gradient = [[[NSGradient alloc] initWithStartingColor:([[self window] firstResponder] == self && [NSApp keyWindow] == [self window])?[NSColor alternateSelectedControlColor]:[NSColor secondarySelectedControlColor] endingColor:[NSColor redColor]] autorelease];
				
				[gradient drawInRect:rowRect angle:180.0];
			}
			else {
				if (breakpointType == RSBreakpointTypeNormal)
					[[NSColor redColor] setFill];
				
				NSRectFill(rowRect);
			}
		}
	}
}

- (IBAction)gotoAddress:(id)sender; {
	[RSDebuggerGotoAddressSheetController presentGotoAddressSheetForGotoAddressController:(id <RSGotoAddressController>)[self dataSource]];
}

- (IBAction)toggleBreakpoint:(id)sender; {
	[[self selectedRowIndexes] enumerateIndexesUsingBlock:^(NSUInteger rowIndex,BOOL *stop) {
		[[(id <RSGotoAddressController>)[self dataSource] calculator] toggleBreakpointOfType:RSBreakpointTypeNormal atAddress:[[[self dataSource] tableView:self objectValueForTableColumn:[self tableColumnWithIdentifier:kDisassemblyTableColumnAddressIdentifier] row:rowIndex] unsignedShortValue]];
	}];
	[self setNeedsDisplay:YES];
}

@end
