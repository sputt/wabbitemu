//
//  WCTooltipManager.m
//  WabbitStudio
//
//  Created by William Towe on 5/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTooltipManager.h"
#import "WCTooltip.h"
#import "WCTooltipView.h"
#import "NS(Attributed)String+Geometrics.h"
#import <QuartzCore/QuartzCore.h>


const CGFloat kTooltipDelay = 0.35;

static const CFTimeInterval kTooltipFadeDuration = 0.5;

@implementation WCTooltipManager

+ (WCTooltipManager *)sharedTooltipManager; {
	return [self sharedController];
}

- (void)showTooltip:(WCTooltip *)tooltip; {
	if (_tooltipPanel == nil) {
		if (![NSBundle loadNibNamed:@"WCTooltipPanel" owner:self]) {
			NSLog(@"Unable to load tooltip panel xib!");
			return;
		}
#ifdef DEBUG
		NSAssert(_tooltipPanel != nil, @"tooltipPanel cannot be nil!");
#endif
		
		CAAnimation *animation = [CABasicAnimation animation];
		[animation setDuration:kTooltipFadeDuration];
		[animation setDelegate:self];
		[_tooltipPanel setAnimations:[NSDictionary dictionaryWithObject:animation forKey:@"alphaValue"]];
	}
	
	if (_isShowingTooltip)
		[self hideTooltip];
	
	_isShowingTooltip = YES;
	
	NSSize maxSize;
	if ([tooltip attributedString] == nil)
		maxSize = [[tooltip string] sizeForWidth:FLT_MAX height:FLT_MAX attributes:[tooltip attributesForTooltip]];
	else
		maxSize = [[tooltip attributedString] sizeForWidth:FLT_MAX height:FLT_MAX];
	
	[_tooltipPanel setFrame:[_tooltipPanel frameRectForContentRect:NSMakeRect(0.0, 0.0, maxSize.width+kTooltipLeftMargin+kTooltipRightMargin, maxSize.height+kTooltipTopMargin+kTooltipBottomMargin)] display:NO];
	
	[_tooltipPanel setAlphaValue:1.0];
	[_tooltipPanel setFrameTopLeftPoint:[tooltip location]];
	[_tooltipView setTooltip:tooltip];
	[_tooltipPanel orderFront:nil];
}
- (void)hideTooltip; {
	if (!_isShowingTooltip)
		return;
	
	_isShowingTooltip = NO;
	
	[[_tooltipPanel animator] setAlphaValue:0.0];
}

- (void)animationDidStop:(CAAnimation *)animation finished:(BOOL)flag  {
	// Detect the end of fade out and close the window
	if(0.0 == [_tooltipPanel alphaValue] && flag)
		[_tooltipPanel orderOut:nil];
}
@end
