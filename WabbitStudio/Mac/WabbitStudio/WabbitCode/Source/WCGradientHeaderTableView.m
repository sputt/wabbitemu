//
//  WCGroupHeaderTableView.m
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCGradientHeaderTableView.h"
#import "WCGradientTextFieldCell.h"

static NSGradient *_gradient = nil;
static NSColor *_borderColor = nil;

@implementation WCGradientHeaderTableView

+ (void)initialize {
	if ([WCGradientHeaderTableView class] != self)
		return;
	
	_gradient = [[NSGradient alloc] initWithStartingColor:[NSColor whiteColor] endingColor:[NSColor lightGrayColor]];
	_borderColor = [[NSColor darkGrayColor] retain];
}

- (void)highlightSelectionInClipRect:(NSRect)clipRect {
	[super highlightSelectionInClipRect:clipRect];
	
	NSRange rowRange = [self rowsInRect:clipRect];
	
	for (NSUInteger row = rowRange.location; row < NSMaxRange(rowRange); row++) {
		if ([(WCGradientTextFieldCell *)[self preparedCellAtColumn:0 row:row] drawGradient]) {
			NSRect rowRect = [self rectOfRow:row];
			
			[_gradient drawInRect:rowRect angle:90.0];
			[_borderColor setFill];
			if (row != 0)
				NSRectFill(NSMakeRect(rowRect.origin.x, rowRect.origin.y, rowRect.size.width, 1.0));
			rowRect.origin.y += rowRect.size.height - 1.0;
			NSRectFill(NSMakeRect(rowRect.origin.x, rowRect.origin.y, rowRect.size.width, 1.0));
		}
	}
}

@end
