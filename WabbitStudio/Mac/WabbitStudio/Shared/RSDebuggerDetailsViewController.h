//
//  RSDebuggerDetailsViewController.h
//  WabbitStudio
//
//  Created by William Towe on 5/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>
#import "TLAnimatingOutlineView.h"


@class RSCalculator;

@interface RSDebuggerDetailsViewController : NSViewController <TLAnimatingOutlineViewDelegate> {
@private
	IBOutlet NSView *_registersView;
	IBOutlet NSView *_flagsView;
	IBOutlet NSView *_cpuView;
	IBOutlet NSView *_memoryMapView;
	IBOutlet NSView *_interruptsView;
	IBOutlet NSView *_displayView;
	
	IBOutlet TLAnimatingOutlineView *_animatingOutlineView;
	
    RSCalculator *_calculator;
}
@property (retain,nonatomic) RSCalculator *calculator;
@property (readonly,nonatomic) TLAnimatingOutlineView *animatingOutlineView;

- (id)initWithCalculator:(RSCalculator *)calculator;

@end
