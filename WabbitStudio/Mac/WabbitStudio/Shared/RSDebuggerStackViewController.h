//
//  RSDebuggerStackViewController.h
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>
#import "RSGotoAddressControllerProtocol.h"


@class RSDebuggerStackTableView;

@interface RSDebuggerStackViewController : NSViewController <NSTableViewDataSource,NSTableViewDelegate,RSGotoAddressController> {
@private
	IBOutlet RSDebuggerStackTableView *_stackTableView;
	
    RSCalculator *_calculator;
	NSUInteger _numberOfRows;
	uint16_t _startAddress;
}
@property (readonly,nonatomic) RSCalculator *calculator;

- (id)initWithCalculator:(RSCalculator *)calculator;

- (void)scrollToAddress:(uint16_t)address;
@end
