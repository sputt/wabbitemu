//
//  WCFindBarViewController.m
//  WabbitStudio
//
//  Created by William Towe on 3/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFindBarViewController.h"
#import "WCTextView.h"
#import "WCFileViewController.h"
#import "WCTopBarView.h"

@interface WCFindBarViewController (Private)
- (id)_initWithTextView:(WCTextView *)textView;

- (void)_showFindBar;
@end

@implementation WCFindBarViewController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	_textView = nil;
    [super dealloc];
}

+ (void)presentFindBarForTextView:(WCTextView *)textView; {
	WCFindBarViewController *controller = [[[self class] alloc] _initWithTextView:textView];
	
	[controller _showFindBar];
}

- (IBAction)closeFindBar:(id)sender; {	
	NSRect frame = [[self view] frame];
	_closeAnimation = [[NSViewAnimation alloc] initWithDuration:0.25 animationCurve:NSAnimationEaseIn];
	NSRect scrollFrame = [[_textView enclosingScrollView] frame];
	
	[_closeAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:[self view],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(frame.origin.x, frame.origin.y+frame.size.height, frame.size.width, frame.size.height)],NSViewAnimationEndFrameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:[_textView enclosingScrollView],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(scrollFrame.origin.x, scrollFrame.origin.y, scrollFrame.size.width, scrollFrame.size.height+frame.size.height)],NSViewAnimationEndFrameKey, nil], nil]];
	
	[_closeAnimation setDelegate:self];
	
	[_closeAnimation startAnimation];
}

- (IBAction)toggleReplaceControls:(id)sender; {
	_toggleReplaceControlsAnimation = [[NSViewAnimation alloc] initWithDuration:0.25 animationCurve:NSAnimationEaseIn];
	NSRect frame = [[self view] frame];
	NSRect scrollFrame = [[_textView enclosingScrollView] frame];
	[_toggleReplaceControlsAnimation setDelegate:self];
	
	if (_replaceControlsVisible) {
		[_toggleReplaceControlsAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:[self view],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(frame.origin.x, frame.origin.y+floor(frame.size.height/2.0), NSWidth(frame), floor(NSHeight(frame)/2.0))],NSViewAnimationEndFrameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:[_textView enclosingScrollView],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(scrollFrame.origin.x, scrollFrame.origin.y, NSWidth(scrollFrame), NSHeight(scrollFrame)+floor(NSHeight(frame)/2.0))],NSViewAnimationEndFrameKey, nil], nil]];
	}
	else {
		[_toggleReplaceControlsAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:[self view],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(frame.origin.x, frame.origin.y-frame.size.height, NSWidth(frame), NSHeight(frame)*2.0)],NSViewAnimationEndFrameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:[_textView enclosingScrollView],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(scrollFrame.origin.x, scrollFrame.origin.y, NSWidth(scrollFrame), NSHeight(scrollFrame)-NSHeight(frame))],NSViewAnimationEndFrameKey, nil], nil]];
	}
	
	[_toggleReplaceControlsAnimation startAnimation];
}

- (IBAction)find:(id)sender; {
	NSString *string = [_textView string];
	NSRange entireRange = NSMakeRange(0, [string length]);
	
	[[_textView layoutManager] removeTemporaryAttribute:NSBackgroundColorAttributeName forCharacterRange:entireRange];
	[[_textView layoutManager] removeTemporaryAttribute:NSUnderlineStyleAttributeName forCharacterRange:entireRange];
	[[_textView layoutManager] removeTemporaryAttribute:NSUnderlineColorAttributeName forCharacterRange:entireRange];
	
	if (![sender stringValue] || ![[sender stringValue] length])
		return;
	
	NSMutableArray *foundRanges = [NSMutableArray array];
	NSString *searchString = [sender stringValue];
	NSRange searchRange = entireRange;
	NSStringCompareOptions options = NSLiteralSearch;
	NSRange matchRange = NSMakeRange(NSNotFound, 0);
	
	while (searchRange.location < entireRange.length) {
		//NSLog(@"%@",NSStringFromRange(searchRange));
		matchRange = [string rangeOfString:searchString options:options range:searchRange];
		
		if (matchRange.location == NSNotFound)
			break;
		
		[foundRanges addObject:[NSValue valueWithRange:matchRange]];
		
		searchRange = NSMakeRange(NSMaxRange(matchRange), entireRange.length-NSMaxRange(matchRange));
	}
	
	for (NSValue *rValue in foundRanges) {
		NSRange range = [rValue rangeValue];
		
		[[_textView layoutManager] addTemporaryAttributes:[NSDictionary dictionaryWithObjectsAndKeys:[NSColor yellowColor],NSBackgroundColorAttributeName,[NSNumber numberWithUnsignedInteger:NSUnderlineStyleThick],NSUnderlineStyleAttributeName,[NSColor orangeColor],NSUnderlineColorAttributeName, nil] forCharacterRange:range];
	}
}

- (id)_initWithTextView:(WCTextView *)textView; {
	if (!(self = [super initWithNibName:@"WCFindBarView" bundle:nil]))
		return nil;
	
	_textView = textView;
	
	return self;
}

- (void)_showFindBar; {
	[[[_textView fileViewController] view] addSubview:[self view] positioned:NSWindowBelow relativeTo:[[_textView fileViewController] topBarView]];
	NSRect topFrame = [[[_textView fileViewController] topBarView] frame];
	[[self view] setFrame:NSMakeRect(topFrame.origin.x, topFrame.origin.y, NSWidth(topFrame), NSHeight([[self view] frame]))];
	NSRect frame = [[self view] frame];
	_showAnimation = [[NSViewAnimation alloc] initWithDuration:0.25 animationCurve:NSAnimationEaseIn];
	NSRect scrollFrame = [[_textView enclosingScrollView] frame];
	
	[_showAnimation setViewAnimations:[NSArray arrayWithObjects:[NSDictionary dictionaryWithObjectsAndKeys:[self view],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(frame.origin.x, frame.origin.y-frame.size.height, frame.size.width, frame.size.height)],NSViewAnimationEndFrameKey, nil],[NSDictionary dictionaryWithObjectsAndKeys:[_textView enclosingScrollView],NSViewAnimationTargetKey,[NSValue valueWithRect:NSMakeRect(scrollFrame.origin.x, scrollFrame.origin.y, scrollFrame.size.width, scrollFrame.size.height-frame.size.height)],NSViewAnimationEndFrameKey, nil], nil]];
	 
	[_showAnimation setDelegate:self];
	
	[_showAnimation startAnimation];
}

- (void)animationDidEnd:(NSAnimation *)animation {
	if (animation == _closeAnimation) {
		[self autorelease];
		[[[self view] window] makeFirstResponder:_textView];
		[_closeAnimation release];
		_closeAnimation = nil;
		[[self view] removeFromSuperviewWithoutNeedingDisplay];
	}
	else if (animation == _showAnimation) {
		[[[self view] window] makeFirstResponder:_searchField];
		[_showAnimation release];
		_showAnimation = nil;
	}
	else {
		_replaceControlsVisible = !_replaceControlsVisible;
		[_toggleReplaceControlsAnimation release];
		_toggleReplaceControlsAnimation = nil;
	}
}
@end
