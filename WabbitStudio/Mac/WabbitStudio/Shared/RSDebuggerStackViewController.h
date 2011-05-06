//
//  RSDebuggerStackViewController.h
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>


@class RSCalculator;

@interface RSDebuggerStackViewController : NSViewController <NSTableViewDataSource,NSTableViewDelegate> {
@private
    RSCalculator *_calculator;
	NSUInteger _numberOfRows;
}
@property (readonly,nonatomic) RSCalculator *calculator;
@end
