//
//  WCGradientTabView.m
//  WabbitStudio
//
//  Created by William Towe on 3/26/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCEditorTabView.h"
#import "NSView+WCExtensions.h"


@implementation WCEditorTabView
- (BOOL)shouldDrawBackgroundString {
	return ([self numberOfTabViewItems] == 0);
}
- (NSString *)backgroundString {
	return NSLocalizedString(@"No Open Editors", @"No Open Editors");
}

- (void)drawRect:(NSRect)rect {
	[super drawRect:rect];
	
	[self drawBackgroundString];
}

@end
