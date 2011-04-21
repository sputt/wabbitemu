//
//  WCBreakpointEditViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBreakpointEditViewController.h"
#import "WCBreakpoint.h"
#import "MAAttachedWindow.h"
#import "NoodleLineNumberView.h"


@implementation WCBreakpointEditViewController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[_showAnimation release];
	[_closeAnimation release];
	_breakpoint = nil;
    [super dealloc];
}

- (id)initWithBreakpoint:(WCBreakpoint *)breakpoint rulerView:(NoodleLineNumberView *)rulerView {
	if (!(self = [super initWithNibName:@"WCBreakpointEditView" bundle:nil]))
		return nil;
	
	_breakpoint = breakpoint;
	_rulerView = rulerView;
	
	return self;
}

@synthesize breakpoint=_breakpoint;

- (IBAction)showEditViewController:(id)sender; {
	/*
	[[[self view] window] setAlphaValue:0.0];
	[[[self view] window] orderFront:nil];
	[[[self view] window] setLevel:NSPopUpMenuWindowLevel];
	
	_showAnimation = [[NSViewAnimation alloc] initWithDuration:0.2 animationCurve:NSAnimationEaseIn];
	
	[_showAnimation setDelegate:self];
	[_showAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:[[self view] window],NSViewAnimationTargetKey,NSViewAnimationFadeInEffect,NSViewAnimationEffectKey, nil], nil]];
	
	[_showAnimation startAnimation];
	 */
	
	[[_rulerView window] addChildWindow:[[self view] window] ordered:NSWindowAbove];
	[[[self view] window] makeKeyWindow];
}

- (IBAction)closeEditViewController:(id)sender; {
	_closeAnimation = [[NSViewAnimation alloc] initWithDuration:0.2 animationCurve:NSAnimationEaseIn];
	
	[_closeAnimation setDelegate:self];
	[_closeAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:[[self view] window],NSViewAnimationTargetKey,NSViewAnimationFadeOutEffect,NSViewAnimationEffectKey, nil], nil]];
	
	[_closeAnimation startAnimation];
}

- (void)animationDidEnd:(NSAnimation *)animation {
	if (animation == _showAnimation) {
		[[_rulerView window] addChildWindow:[[self view] window] ordered:NSWindowAbove];
		[[[self view] window] makeKeyWindow];
	}
	else {
		[self autorelease];
		[_rulerView setCurrentEditViewController:nil];
		
		MAAttachedWindow *window = (MAAttachedWindow *)[[self view] window];
		
		[[window parentWindow] removeChildWindow:window];
		[window orderOut:nil];
		[window release];
	}
}
@end
