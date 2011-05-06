//
//  RSDebuggerDisassemblyViewController.m
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerDisassemblyViewController.h"
#import "RSCalculator.h"
#import "NSViewController+RSExtensions.h"


NSString *const kDisassemblyTableColumnAddressIdentifier = @"address";
NSString *const kDisassemblyTableColumnDataIdentifier = @"data";
NSString *const kDisassemblyTableColumnDisassemblyIdentifier = @"disassembly";
NSString *const kDisassemblyTableColumnSizeIdentifier = @"size";

@interface RSDebuggerDisassemblyViewController ()
@property (assign,nonatomic) NSUInteger numberOfRows;
@property (assign,nonatomic) uint16_t startAddress;
@end

@implementation RSDebuggerDisassemblyViewController

- (void)dealloc {
	[_calculator removeObserver:self forKeyPath:@"programCounter"];
	[_calculator removeObserver:self forKeyPath:@"CPUHalt"];
	[_calculator release];
    [super dealloc];
}

- (NSString *)viewNibName {
	return @"RSDebuggerDisassemblyView";
}

#define CALC_DISASSEMBLY_SIZE 64384

- (void)loadView {
	[super loadView];
	
	[self setNumberOfRows:CALC_DISASSEMBLY_SIZE];
	[self setStartAddress:0];
	[self scrollToAddress:[[self calculator] programCounter]];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	if ([keyPath isEqualToString:@"programCounter"] && context == self) {
		[self setStartAddress:0];
		[self scrollToAddress:[[self calculator] programCounter]];
	}
	else if ([keyPath isEqualToString:@"CPUHalt"] && context == self) {
		[_disassemblyTableView setNeedsDisplay:YES];
	}
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
	return [self numberOfRows];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	if ([[tableColumn identifier] isEqualToString:kDisassemblyTableColumnAddressIdentifier]) {
		return [NSNumber numberWithUnsignedShort:_info[row].addr];
	}
	else if ([[tableColumn identifier] isEqualToString:kDisassemblyTableColumnDataIdentifier]) {
		uint32_t offset, total = 0;
		for (offset = 0; offset < _info[row].size; offset++) {
			total += mem_read(&[[self calculator] calc]->mem_c, _info->addr + offset);
			total <<= 8;
		}
		return [NSNumber numberWithUnsignedInt:total];
	}
	else if ([[tableColumn identifier] isEqualToString:kDisassemblyTableColumnDisassemblyIdentifier]) {
		return [[[NSString alloc] initWithUTF8String:_info[row].expanded] autorelease];
	}
	else if ([[tableColumn identifier] isEqualToString:kDisassemblyTableColumnSizeIdentifier]) {
		return [NSNumber numberWithUnsignedInt:_info[row].size];
	}
	return nil;
}



- (id)initWithCalculator:(RSCalculator *)calculator; {
	if (!(self = [super initWithNibName:[self viewNibName] bundle:nil]))
		return nil;
	
	_calculator = [calculator retain];
	
	[calculator addObserver:self forKeyPath:@"programCounter" options:NSKeyValueObservingOptionNew context:(void *)self];
	[calculator addObserver:self forKeyPath:@"CPUHalt" options:NSKeyValueObservingOptionNew context:(void *)self];
	
	return self;
}

- (void)scrollToAddress:(uint16_t)address; {
	NSUInteger rowIndex;
	
	for (rowIndex = 0; rowIndex < [self numberOfRows]; rowIndex++) {
		if (_info[rowIndex].addr == address) {
			[_disassemblyTableView selectRowIndexes:[NSIndexSet indexSetWithIndex:rowIndex] byExtendingSelection:NO];
			[_disassemblyTableView scrollRowToVisible:rowIndex];
			return;
		}
	}
	
	[self setStartAddress:address];
	[self scrollToAddress:address];
}

@synthesize calculator=_calculator;
@synthesize numberOfRows=_numberOfRows;
@dynamic startAddress;
- (uint16_t)startAddress {
	return _startAddress;
}
- (void)setStartAddress:(uint16_t)startAddress {	
	_startAddress = startAddress;
	
	free(_info);
	_info = calloc([self numberOfRows], sizeof(Z80_info_t));
	
	lpDebuggerCalc = [[self calculator] calc];
	
	disassemble(&[[self calculator] calc]->mem_c, _startAddress, (int32_t)[self numberOfRows], _info);
	
	[_disassemblyTableView reloadData];
}

@end
