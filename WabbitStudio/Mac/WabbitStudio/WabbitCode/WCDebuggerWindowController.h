//
//  WCDebuggerWindowController.h
//  WabbitStudio
//
//  Created by William Towe on 4/30/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>
#import "RSCalculatorOwnerProtocol.h"


@class WELCDView,TLAnimatingOutlineView;

@interface WCDebuggerWindowController : NSWindowController <NSWindowDelegate> {
@private
    IBOutlet WELCDView *_LCDView;
	IBOutlet TLAnimatingOutlineView *_animatingOutlineView;
	IBOutlet NSView *_registersView;
	IBOutlet NSView *_flagsView;
}
@property (readonly,nonatomic) id <RSCalculatorOwner> calculatorOwner;
@property (readonly,nonatomic) WELCDView *LCDView;
@property (readonly,nonatomic) TLAnimatingOutlineView *animatingOutlineView;
@end
