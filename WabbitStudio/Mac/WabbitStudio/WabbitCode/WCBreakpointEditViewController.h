//
//  WCBreakpointEditViewController.h
//  WabbitStudio
//
//  Created by William Towe on 4/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>

@class WCBreakpoint,NoodleLineNumberView;

@interface WCBreakpointEditViewController : NSViewController <NSAnimationDelegate> {
@private
    __weak WCBreakpoint *_breakpoint;
	NSViewAnimation *_showAnimation;
	NSViewAnimation *_closeAnimation;
	__weak NoodleLineNumberView *_rulerView;
}
@property (readonly,nonatomic) WCBreakpoint *breakpoint;

- (IBAction)showEditViewController:(id)sender;
- (IBAction)closeEditViewController:(id)sender;

- (id)initWithBreakpoint:(WCBreakpoint *)breakpoint rulerView:(NoodleLineNumberView *)rulerView;
@end
