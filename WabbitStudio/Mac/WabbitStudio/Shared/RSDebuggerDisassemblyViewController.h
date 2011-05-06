//
//  RSDebuggerDisassemblyViewController.h
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>
#include "disassemble.h"


extern NSString *const kDisassemblyTableColumnAddressIdentifier;
extern NSString *const kDisassemblyTableColumnDataIdentifier;
extern NSString *const kDisassemblyTableColumnDisassemblyIdentifier;
extern NSString *const kDisassemblyTableColumnSizeIdentifier;

@class RSCalculator;

@interface RSDebuggerDisassemblyViewController : NSViewController <NSTableViewDataSource,NSTableViewDelegate> {
@private
	IBOutlet NSTableView *_disassemblyTableView;
	
    RSCalculator *_calculator;
	Z80_info_t *_info;
	NSUInteger _numberOfRows;
	uint16_t _startAddress;
}
@property (readonly,nonatomic) RSCalculator *calculator;

- (id)initWithCalculator:(RSCalculator *)calculator;

- (void)scrollToAddress:(uint16_t)address;
@end
