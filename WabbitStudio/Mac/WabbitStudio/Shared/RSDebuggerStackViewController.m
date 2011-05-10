//
//  RSDebuggerStackViewController.m
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerStackViewController.h"
#import "RSCalculator.h"
#import "RSDebuggerStackTableView.h"
#import "NSViewController+RSExtensions.h"
#import "WCHexFormatter.h"


@interface RSDebuggerStackViewController ()
@property (assign,nonatomic) NSUInteger numberOfRows;
@property (assign,nonatomic) uint16_t startAddress;
@end

@implementation RSDebuggerStackViewController

- (void)dealloc {
	[_stackTableView setDelegate:nil];
	[_stackTableView setDataSource:nil];
	[_calculator removeObserver:self forKeyPath:@"stackPointer"];
	[_calculator release];
    [super dealloc];
}

- (NSString *)viewNibName {
	return @"RSDebuggerStackView";
}

- (void)loadView {
	[super loadView];
	
	[self setNumberOfRows:150];
	[self setStartAddress:[[self calculator] stackPointer]];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	if ([keyPath isEqualToString:@"stackPointer"] && context == self) {
		[self setStartAddress:[[self calculator] stackPointer]];
		[self scrollToAddress:[[self calculator] stackPointer]];
	}
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
	return [self numberOfRows];
}

#define BYTES_PER_ROW 2

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	if ([[tableColumn identifier] isEqualToString:kRSDebuggerStackTableColumnAddressIdentifier]) {
		uint16_t address = [self startAddress] + (row * BYTES_PER_ROW);
		
		return [NSNumber numberWithUnsignedShort:address];
	}
	else if ([[tableColumn identifier] isEqualToString:kRSDebuggerStackTableColumnStackIdentifier]) {
		uint16_t address = [self startAddress] + (row * BYTES_PER_ROW);
		
		return [NSNumber numberWithUnsignedShort:mem_read16(&[[self calculator] calc]->mem_c, address)];
	}
	return nil;
}

- (void)tableView:(NSTableView *)tableView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	if ([[tableColumn identifier] isEqualToString:kRSDebuggerStackTableColumnStackIdentifier]) {
		uint16_t address = [self startAddress] + (row * BYTES_PER_ROW);
		uint16_t data = [object unsignedShortValue];
		
		// have to write each 8 bit chunk
		mem_write(&[[self calculator] calc]->mem_c, address, (data & 0xFF));
		mem_write(&[[self calculator] calc]->mem_c, ++address, ((data >> 8) & 0xFF));
	}
}

- (NSString *)tableView:(NSTableView *)tableView toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row mouseLocation:(NSPoint)mouseLocation {
	if ([[tableColumn identifier] isEqualToString:kRSDebuggerStackTableColumnAddressIdentifier])
		return nil;
	
	uint16_t address = [self startAddress] + (row * BYTES_PER_ROW);
	
	return [NSString stringWithFormat:NSLocalizedString(@"Address: %04X\nData: %04X", @"stack table view cell tooltip"),address,mem_read16(&[[self calculator] calc]->mem_c, address)];
}

- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	if ([[tableColumn identifier] isEqualToString:kRSDebuggerStackTableColumnAddressIdentifier]) {
		uint16_t address = [self startAddress] + (row * BYTES_PER_ROW);
		
		if (address == [[self calculator] stackPointer]) {
			[[cell formatter] setShouldDrawWithProgramCounterAttributes:YES];
			[[cell formatter] setCellIsHighlighted:[cell isHighlighted]];
		}
		else
			[[cell formatter] setShouldDrawWithProgramCounterAttributes:NO];
	}
	else
		[[cell formatter] setShouldDrawWithProgramCounterAttributes:NO];
}

- (id)initWithCalculator:(RSCalculator *)calculator; {
	if (!(self = [super initWithNibName:[self viewNibName] bundle:nil]))
		return nil;
	
	_calculator = [calculator retain];
	
	[calculator addObserver:self forKeyPath:@"stackPointer" options:NSKeyValueObservingOptionNew context:(void *)self];
	
	return self;
}

- (void)scrollToAddress:(uint16_t)address; {
	NSUInteger rowIndex;
	
	for (rowIndex = 0; rowIndex < [self numberOfRows]; rowIndex++) {
		uint16_t rowAddress = [self startAddress] + (rowIndex * BYTES_PER_ROW);
		
		if (rowAddress >= address) {
			[_stackTableView selectRowIndexes:[NSIndexSet indexSetWithIndex:rowIndex] byExtendingSelection:NO];
			[_stackTableView scrollRowToVisible:rowIndex];
			return;
		}
	}
	
	NSBeep();
}

@synthesize calculator=_calculator;
@synthesize numberOfRows=_numberOfRows;
@dynamic startAddress;
- (uint16_t)startAddress {
	return _startAddress;
}
- (void)setStartAddress:(uint16_t)startAddress {
	_startAddress = startAddress;
	
	[_stackTableView reloadData];
}
@end
