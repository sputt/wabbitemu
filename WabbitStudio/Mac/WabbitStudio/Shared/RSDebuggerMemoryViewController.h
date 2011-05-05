//
//  RSDebuggerMemoryViewController.h
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>


@class RSCalculator;

@interface RSDebuggerMemoryViewController : NSViewController <NSTableViewDataSource,NSTableViewDelegate> {
@private
	IBOutlet NSTableView *_memoryTableView;
	
    RSCalculator *_calculator;
	uint16_t _startAddress;
	NSUInteger _numberOfRows;
}
@property (retain,nonatomic) RSCalculator *calculator;
@property (assign,nonatomic) uint16_t startAddress;
@property (assign,nonatomic) NSUInteger numberOfRows;

- (id)initWithCalculator:(RSCalculator *)calculator;

- (void)scrollToAddress:(uint16_t)address;

@end
