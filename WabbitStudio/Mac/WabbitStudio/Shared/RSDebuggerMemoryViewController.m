//
//  RSDebuggerMemoryViewController.m
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerMemoryViewController.h"
#import "RSCalculator.h"
#import "NSViewController+RSExtensions.h"
#import "WCTwoDigitHexFormatter.h"


@interface RSDebuggerMemoryViewController ()
- (void)_adjustMemoryTableViewColumns;
@end

@implementation RSDebuggerMemoryViewController

- (NSString *)viewNibName {
	return @"RSDebuggerMemoryView";
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_calculator removeObserver:self forKeyPath:@"programCounter"];
	[_calculator release];
    [super dealloc];
}

- (void)loadView {
	[self setNumberOfRows:128];
	
	[super loadView];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_frameDidChange:) name:NSViewFrameDidChangeNotification object:[(NSScrollView *)[self view] contentView]];
	
	[self _adjustMemoryTableViewColumns];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	if ([keyPath isEqualToString:@"programCounter"] && context == self) {
		[self scrollToAddress:[[self calculator] programCounter]];
	}
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
	return [self numberOfRows];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	NSUInteger bytesPerRow = [_memoryTableView numberOfColumns] - 1;
	uint16_t address = [self startAddress] + (row * bytesPerRow);
	
	if ([[tableView tableColumns] objectAtIndex:0] == tableColumn)
		return [NSNumber numberWithUnsignedShort:address];
	
	uint16_t readAddress = (uint16_t)(address + [[tableColumn identifier] integerValue]);

	return [NSNumber numberWithUnsignedChar:mem_read(&[[self calculator] calc]->mem_c,readAddress)];
}

- (void)tableView:(NSTableView *)tableView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	NSUInteger bytesPerRow = [_memoryTableView numberOfColumns] - 1;
	uint16_t address = [self startAddress] + (row * bytesPerRow);
	
	mem_write(&[[self calculator] calc]->mem_c, address + [[tableColumn identifier] integerValue], [object unsignedCharValue]);
}

- (NSString *)tableView:(NSTableView *)tableView toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row mouseLocation:(NSPoint)mouseLocation {
	if ([[tableView tableColumns] objectAtIndex:0] == tableColumn)
		return nil;
	
	NSUInteger bytesPerRow = [_memoryTableView numberOfColumns] - 1;
	uint16_t address = [self startAddress] + (row * bytesPerRow);
	uint16_t readAddress = (uint16_t)(address + [[tableColumn identifier] integerValue]);
	
	return [NSString stringWithFormat:@"Address: %04X\nData: %02x",readAddress,mem_read(&[[self calculator] calc]->mem_c, readAddress)];
}

- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	if ([[tableView tableColumns] objectAtIndex:0] == tableColumn)
		return;
	
	NSUInteger bytesPerRow = [_memoryTableView numberOfColumns] - 1;
	uint16_t address = [self startAddress] + (row * bytesPerRow);
	uint16_t readAddress = (uint16_t)(address + [[tableColumn identifier] integerValue]);
	
	if (readAddress == [[self calculator] programCounter])
		[[cell formatter] setShouldDrawWithProgramCounterAttributes:YES];
	else
		[[cell formatter] setShouldDrawWithProgramCounterAttributes:NO];
}

- (void)scrollToAddress:(uint16_t)address; {
	uint16_t startAddress = [self startAddress];
	NSUInteger bytesPerRow = [_memoryTableView numberOfColumns] - 1;
	NSUInteger rowIndex;
	
	for (rowIndex = 0; rowIndex < [self numberOfRows]; rowIndex++) {
		uint16_t rowStartAddress = startAddress + (rowIndex * bytesPerRow);
		
		if (address <= rowStartAddress + bytesPerRow) {
			
			[_memoryTableView selectRowIndexes:[NSIndexSet indexSetWithIndex:rowIndex] byExtendingSelection:NO];
			[_memoryTableView scrollRowToVisible:rowIndex];
			return;
		}
	}
	
	NSBeep();
}

- (id)initWithCalculator:(RSCalculator *)calculator; {
	if (!(self = [super initWithNibName:[self viewNibName] bundle:nil]))
		return nil;
	
	_calculator = [calculator retain];
	
	[calculator addObserver:self forKeyPath:@"programCounter" options:NSKeyValueObservingOptionNew context:(void *)self];
	
	return self;
}

@synthesize calculator=_calculator;
@dynamic startAddress;
- (uint16_t)startAddress {
	return _startAddress;
}
- (void)setStartAddress:(uint16_t)startAddress {	
	_startAddress = startAddress;
	
	[_memoryTableView reloadData];
}
@synthesize numberOfRows=_numberOfRows;

- (void)_adjustMemoryTableViewColumns {
	CGFloat byteColumnWidth = 25.0 + [_memoryTableView intercellSpacing].width;
	
	CGFloat addressColumnWidth = NSWidth([[_memoryTableView headerView] headerRectOfColumn:0]);
	CGFloat availableWidth = NSWidth([[(NSScrollView *)[self view] contentView] documentVisibleRect]) - addressColumnWidth - NSWidth([[(NSScrollView *)[self view] verticalScroller] frame]);
	NSUInteger currentNumberOfByteColumns = [_memoryTableView numberOfColumns] - 1;
	NSUInteger maxNumberOfByteColumns = (NSUInteger)floor(availableWidth/byteColumnWidth);
	
	if (currentNumberOfByteColumns < maxNumberOfByteColumns) {
		while (currentNumberOfByteColumns++ < maxNumberOfByteColumns) {
#ifdef DEBUG
			//NSLog(@"adding column");
#endif
			
			NSTableColumn *column = [[[NSTableColumn alloc] initWithIdentifier:[NSString stringWithFormat:@"%u", currentNumberOfByteColumns - 1]] autorelease];
			[column setWidth:byteColumnWidth - [_memoryTableView intercellSpacing].width];
			[column setResizingMask:NSTableColumnNoResizing];
			[[column headerCell] setTitle:NSLocalizedString(@"Memory",@"memory view memory table column title")];
			[_memoryTableView addTableColumn:column];
		}
		
		[self setNumberOfRows:64384/maxNumberOfByteColumns];
		//[_memoryTableView performSelector:@selector(reloadData) withObject:nil afterDelay:0.0];
		[_memoryTableView reloadData];
	}
	else if (currentNumberOfByteColumns > maxNumberOfByteColumns) {
		while (currentNumberOfByteColumns-- > maxNumberOfByteColumns) {
#ifdef DEBUG
			//NSLog(@"removing column");
#endif
			[_memoryTableView removeTableColumn:[[_memoryTableView tableColumns] lastObject]];
		}
		
		[self setNumberOfRows:64384/maxNumberOfByteColumns];
		//[_memoryTableView performSelector:@selector(reloadData) withObject:nil afterDelay:0.0];
		[_memoryTableView reloadData];
	}
}

- (void)_frameDidChange:(NSNotification *)note {
	[self _adjustMemoryTableViewColumns];
}
@end
