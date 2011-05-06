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
#import "RSDebuggerDisassemblyTableView.h"

#define restrict
#import <RegexKit/RegexKit.h>


static RKRegex *kWCSyntaxHighlighterOpCodesRegex = nil;
static RKRegex *kWCSyntaxHighlighterConditionalsRegex = nil;
static RKRegex *kWCSyntaxHighlighterRegistersRegex = nil;
static RKRegex *kWCSyntaxHighlighterNumbersRegex = nil;
static RKRegex *kWCSyntaxHighlighterHexadeicmalsRegex = nil;
static RKRegex *kWCSyntaxHighlighterBinariesRegex = nil;

@interface RSDebuggerDisassemblyViewController ()
@property (assign,nonatomic) NSUInteger numberOfRows;
@property (assign,nonatomic) uint16_t startAddress;
@end

@implementation RSDebuggerDisassemblyViewController

+ (void)initialize {
	if ([RSDebuggerDisassemblyViewController class] != self)
		return;
	
	kWCSyntaxHighlighterOpCodesRegex = [[RKRegex alloc] initWithRegexString:@"\\b(?:adc|add|and|bit|call|ccf|cpdr|cpd|cpir|cpi|cpl|cp|daa|dec|di|djnz|ei|exx|ex|halt|im|inc|indr|ind|inir|ini|in|jp|jr|lddr|ldd|ldir|ldi|ld|neg|nop|or|otdr|otir|outd|outi|out|pop|push|res|reti|retn|ret|rla|rlca|rlc|rld|rl|rra|rrca|rrc|rrd|rr|rst|sbc|scf|set|sla|sll|sra|srl|sub|xor)\\b" options:RKCompileUTF8];
	kWCSyntaxHighlighterConditionalsRegex = [[RKRegex alloc] initWithRegexString:@"\\b(?:nz|nv|nc|po|pe|c|p|m|n|z|v)\\b" options:RKCompileUTF8];
	kWCSyntaxHighlighterRegistersRegex = [[RKRegex alloc] initWithRegexString:@"\\b(?:ixh|iyh|ixl|iyl|sp|af|pc|bc|de|hl|ix|iy|a|f|b|c|d|e|h|l|r|i)\\b" options:RKCompileUTF8];
	kWCSyntaxHighlighterNumbersRegex = [[RKRegex alloc] initWithRegexString:@"\\b[0-9]+\\b" options:RKCompileUTF8];
	kWCSyntaxHighlighterHexadeicmalsRegex = [[RKRegex alloc] initWithRegexString:@"(?:\\$[0-9a-fA-F]+\\b)|(?:(?<=[^$%]\\b)[0-9a-fA-F]+h\\b)" options:RKCompileUTF8];
	kWCSyntaxHighlighterBinariesRegex = [[RKRegex alloc] initWithRegexString:@"(?:%[01]+\\b)|(?:(?<=[^$%]\\b)[01]+b)" options:RKCompileUTF8];
}

- (void)dealloc {
	[_disassemblyTableView setDelegate:nil];
	[_disassemblyTableView setDataSource:nil];
	[_calculator removeObserver:self forKeyPath:@"programCounter"];
	[_calculator removeObserver:self forKeyPath:@"CPUHalt"];
	[_calculator release];
	free(_info);
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
	else if ([keyPath isEqualToString:@"CPUHalt"] && context == self)
		[[self disassemblyTableView] setNeedsDisplay:YES];
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

- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	if ([[tableColumn identifier] isEqualToString:kDisassemblyTableColumnDisassemblyIdentifier]) {
		if (![cell isHighlighted])
			[cell setAttributedStringValue:[self attributedStringByHighlightingAttributedString:[cell attributedStringValue]]];
	}
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
			[[self disassemblyTableView] selectRowIndexes:[NSIndexSet indexSetWithIndex:rowIndex] byExtendingSelection:NO];
			[[self disassemblyTableView] scrollRowToVisible:rowIndex];
			return;
		}
	}
	
	[self setStartAddress:address];
	[self scrollToAddress:address];
}

- (NSAttributedString *)attributedStringByHighlightingAttributedString:(NSAttributedString *)attributedString; {
	NSString *searchString = [attributedString string];
	NSMutableAttributedString *retval = [[attributedString mutableCopy] autorelease];
	
	// op codes
	RKEnumerator *opCodesEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterOpCodesRegex string:searchString] autorelease];
	NSColor *opCodesColor = [NSColor blueColor];
	
	while ([opCodesEnum nextRanges] != NULL) {
		NSRangePointer matchRange = [opCodesEnum currentRanges];
		
		[retval addAttribute:NSForegroundColorAttributeName value:opCodesColor range:*matchRange];
	}
	
	// conditionals
	RKEnumerator *conditionalsEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterConditionalsRegex string:searchString] autorelease];
	NSColor *conditionalsColor = [NSColor colorWithCalibratedRed:0.0 green:181/255.0 blue:1.0 alpha:1.0];
	
	while ([conditionalsEnum nextRanges] != NULL) {
		NSRangePointer matchRange = [conditionalsEnum currentRanges];
		
		[retval addAttribute:NSForegroundColorAttributeName value:conditionalsColor range:*matchRange];
	}
	
	// registers
	RKEnumerator *registersEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterRegistersRegex string:searchString] autorelease];
	NSColor *registersColor = [NSColor redColor];
	
	while ([registersEnum nextRanges] != NULL) {
		NSRangePointer matchRange = [registersEnum currentRanges];
		
		[retval addAttribute:NSForegroundColorAttributeName value:registersColor range:*matchRange];
	}
	
	// numbers
	RKEnumerator *numbersEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterNumbersRegex string:searchString] autorelease];
	NSColor *numbersColor = [NSColor blueColor];
	
	while ([numbersEnum nextRanges] != NULL) {
		NSRangePointer matchRange = [numbersEnum currentRanges];
		
		[retval addAttribute:NSForegroundColorAttributeName value:numbersColor range:*matchRange];
	}
	
	// hexadecimals
	RKEnumerator *hexadecimalsEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterHexadeicmalsRegex string:searchString] autorelease];
	NSColor *hexadecimalsColor = [NSColor magentaColor];
	
	while ([hexadecimalsEnum nextRanges] != NULL) {
		NSRangePointer matchRange = [hexadecimalsEnum currentRanges];
		
		[retval addAttribute:NSForegroundColorAttributeName value:hexadecimalsColor range:*matchRange];
	}
	
	// binaries
	RKEnumerator *binariesEnum = [[[RKEnumerator alloc] initWithRegex:kWCSyntaxHighlighterBinariesRegex string:searchString] autorelease];
	NSColor *binariesColor = [NSColor greenColor];
	
	while ([binariesEnum nextRanges] != NULL) {
		NSRangePointer matchRange = [binariesEnum currentRanges];
		
		[retval addAttribute:NSForegroundColorAttributeName value:binariesColor range:*matchRange];
	}
	
	return [[retval copy] autorelease];
}

@synthesize calculator=_calculator;
@synthesize disassemblyTableView=_disassemblyTableView;
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
	
	[[self disassemblyTableView] reloadData];
}

@end
