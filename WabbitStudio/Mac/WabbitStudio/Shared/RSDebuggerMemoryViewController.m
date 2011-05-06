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
#import "RSDebuggerMemoryTableView.h"
#import "RSDebuggerMemoryTableColumn.h"


@interface RSDebuggerMemoryViewController ()
@property (readonly,nonatomic) uint16_t startAddress;
@property (assign,nonatomic) NSUInteger numberOfRows;
@property (readonly,nonatomic) NSUInteger bytesPerRow;

- (void)_adjustMemoryTableViewColumns;
@end

@implementation RSDebuggerMemoryViewController

- (NSString *)viewNibName {
	return @"RSDebuggerMemoryView";
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_calculator removeObserver:self forKeyPath:@"programCounter"];
	[_memoryTableView setDelegate:nil];
	[_memoryTableView setDataSource:nil];
	[_calculator release];
    [super dealloc];
}

- (void)loadView {
	[super loadView];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_frameDidChange:) name:NSViewFrameDidChangeNotification object:[(NSScrollView *)[self view] contentView]];
	
	[self _adjustMemoryTableViewColumns];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	if ([keyPath isEqualToString:@"programCounter"] && context == self) {
		[self scrollToAddress:[[self calculator] programCounter]];
		[_memoryTableView reloadData];
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
	uint16_t writeAddress = (uint16_t)(address + [[tableColumn identifier] integerValue]);
	
	mem_write(&[[self calculator] calc]->mem_c, writeAddress, [object unsignedCharValue]);
}

- (BOOL)tableView:(NSTableView *)tableView shouldEditTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	if ([[tableView tableColumns] objectAtIndex:0] == tableColumn)
		return NO;
	return YES;
}

- (NSString *)tableView:(NSTableView *)tableView toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row mouseLocation:(NSPoint)mouseLocation {
	if ([[tableView tableColumns] objectAtIndex:0] == tableColumn)
		return nil;
	
	NSUInteger bytesPerRow = [self bytesPerRow];
	uint16_t address = [self startAddress] + (row * bytesPerRow);
	uint16_t readAddress = (uint16_t)(address + [[tableColumn identifier] integerValue]);
	
	return [NSString stringWithFormat:@"Address: %04X\nData: %02x",readAddress,mem_read(&[[self calculator] calc]->mem_c, readAddress)];
}

- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	if ([[tableView tableColumns] objectAtIndex:0] == tableColumn)
		return;
	
	NSUInteger bytesPerRow = [self bytesPerRow];
	uint16_t address = [self startAddress] + (row * bytesPerRow);
	uint16_t readAddress = (uint16_t)(address + [[tableColumn identifier] integerValue]);
	
	if (readAddress == [[self calculator] programCounter]) {
		[[cell formatter] setShouldDrawWithProgramCounterAttributes:YES];
		[[cell formatter] setCellIsHighlighted:[cell isHighlighted]];
	}
	else
		[[cell formatter] setShouldDrawWithProgramCounterAttributes:NO];
}

- (void)scrollToAddress:(uint16_t)address; {
	uint16_t startAddress = [self startAddress];
	NSUInteger bytesPerRow = [self bytesPerRow];
	NSUInteger rowIndex;
	
	for (rowIndex = 0; rowIndex < [self numberOfRows]; rowIndex++) {
		uint16_t rowStartAddress = startAddress + (rowIndex * bytesPerRow);
		
		if (address < rowStartAddress + bytesPerRow) {
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
@synthesize memoryTableView=_memoryTableView;
@dynamic startAddress;
- (uint16_t)startAddress {
	return 0;
}
@dynamic numberOfRows;
- (NSUInteger)numberOfRows {
	return _numberOfRows;
}
- (void)setNumberOfRows:(NSUInteger)numberOfRows {
	if (_numberOfRows == numberOfRows)
		return;
	
	_numberOfRows = numberOfRows;
	
	[_memoryTableView reloadData];
}
@dynamic bytesPerRow;
- (NSUInteger)bytesPerRow {
	return [_memoryTableView numberOfColumns] - 1;
}

#define CALC_MEMORY_SIZE 64384
#define MIN_COLUMN_WIDTH 25.0

- (void)_adjustMemoryTableViewColumns {
	CGFloat byteColumnWidth = MIN_COLUMN_WIDTH + [_memoryTableView intercellSpacing].width;
	
	CGFloat addressColumnWidth = [(NSTableColumn *)[[_memoryTableView tableColumns] objectAtIndex:0] width] + [_memoryTableView intercellSpacing].width;
	CGFloat availableWidth = NSWidth([[(NSScrollView *)[self view] contentView] documentVisibleRect]) - addressColumnWidth;
	NSUInteger currentNumberOfByteColumns = [self bytesPerRow];
	NSUInteger maxNumberOfByteColumns = (NSUInteger)floor(availableWidth/byteColumnWidth);
	
	if (currentNumberOfByteColumns < maxNumberOfByteColumns) {
		while (currentNumberOfByteColumns++ < maxNumberOfByteColumns) {
			RSDebuggerMemoryTableColumn *column = [[[RSDebuggerMemoryTableColumn alloc] initWithIdentifier:[NSString stringWithFormat:@"%u", currentNumberOfByteColumns - 1]] autorelease];
			
			[column setWidth:byteColumnWidth - [_memoryTableView intercellSpacing].width];
			
			[_memoryTableView addTableColumn:column];
		}
		
		[self setNumberOfRows:CALC_MEMORY_SIZE/maxNumberOfByteColumns];
	}
	else {
		while (currentNumberOfByteColumns-- > maxNumberOfByteColumns) {
			[_memoryTableView removeTableColumn:[[_memoryTableView tableColumns] lastObject]];
		}
		
		[self setNumberOfRows:CALC_MEMORY_SIZE/maxNumberOfByteColumns];
	}
}

- (void)_frameDidChange:(NSNotification *)note {
	[self _adjustMemoryTableViewColumns];
}
@end
