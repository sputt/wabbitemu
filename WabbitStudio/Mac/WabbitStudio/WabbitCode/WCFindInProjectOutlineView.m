//
//  WCFindInProjectOutlineView.m
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFindInProjectOutlineView.h"
#import "NSView+WCExtensions.h"


@implementation WCFindInProjectOutlineView

- (BOOL)shouldDrawBackgroundString {
	return ([self numberOfRows] == 0);
}
- (NSString *)backgroundString {
	return NSLocalizedString(@"No Search Results", @"No Search Results");
}

- (void)drawRect:(NSRect)rect {
	[super drawRect:rect];
	
	[self drawBackgroundString];
}

@end
