//
//  WCBuildMessagesOutlineView.m
//  WabbitStudio
//
//  Created by William Towe on 4/2/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBuildMessagesOutlineView.h"
#import "NSView+WCExtensions.h"

@implementation WCBuildMessagesOutlineView

- (BOOL)shouldDrawBackgroundString {
	return ([self numberOfRows] == 0);
}
- (NSString *)backgroundString {
	return NSLocalizedString(@"No Build Messages", @"No Build Messages");
}

- (void)drawBackgroundInClipRect:(NSRect)clipRect {
	[super drawBackgroundInClipRect:clipRect];
	
	[self drawBackgroundString];
}

@end
