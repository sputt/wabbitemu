//
//  RSDebuggerMemoryViewController.h
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>
#import "RSGotoAddressControllerProtocol.h"


@class RSCalculator,RSDebuggerMemoryTableView;

@interface RSDebuggerMemoryViewController : NSViewController <NSTableViewDataSource,NSTableViewDelegate,RSGotoAddressController> {
@private
	IBOutlet RSDebuggerMemoryTableView *_memoryTableView;
	
    RSCalculator *_calculator;
	NSUInteger _numberOfRows;
}
@property (readonly,nonatomic) RSCalculator *calculator;
@property (readonly,nonatomic) RSDebuggerMemoryTableView *memoryTableView;

- (id)initWithCalculator:(RSCalculator *)calculator;

- (void)scrollToAddress:(uint16_t)address;

@end
