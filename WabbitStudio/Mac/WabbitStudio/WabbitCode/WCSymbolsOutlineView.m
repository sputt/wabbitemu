//
//  WCSymbolsOutlineView.m
//  WabbitStudio
//
//  Created by William Towe on 4/10/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSymbolsOutlineView.h"
#import "NSView+WCExtensions.h"


@implementation WCSymbolsOutlineView

- (BOOL)shouldDrawBackgroundString {
	return ([self numberOfRows] == 0);
}
- (NSString *)backgroundString {
	return NSLocalizedString(@"No Matching Symbols", @"No Matching Symbols");
}

- (void)drawBackgroundInClipRect:(NSRect)clipRect {
	[super drawBackgroundInClipRect:clipRect];
	
	[self drawBackgroundString];
}

@end
