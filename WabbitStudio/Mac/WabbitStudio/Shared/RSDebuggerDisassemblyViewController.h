//
//  RSDebuggerDisassemblyViewController.h
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>
#import "RSGotoAddressControllerProtocol.h"
#include "disassemble.h"


@class RSCalculator,RSDebuggerDisassemblyTableView;

@interface RSDebuggerDisassemblyViewController : NSViewController <NSTableViewDataSource,NSTableViewDelegate,RSGotoAddressController> {
@private
	IBOutlet RSDebuggerDisassemblyTableView *_disassemblyTableView;
	
    RSCalculator *_calculator;
	Z80_info_t *_info;
	NSUInteger _numberOfRows;
	uint16_t _startAddress;
}
@property (readonly,nonatomic) RSCalculator *calculator;
@property (readonly,nonatomic) RSDebuggerDisassemblyTableView *disassemblyTableView;

- (id)initWithCalculator:(RSCalculator *)calculator;

- (void)scrollToAddress:(uint16_t)address;

- (NSAttributedString *)attributedStringByHighlightingAttributedString:(NSAttributedString *)attributedString;
@end
