//
//  WCGroupHeaderTableView.m
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCGradientHeaderTableView.h"
#import "WCGradientBackgroundTextFieldCell.h"

@implementation WCGradientHeaderTableView

- (void)highlightSelectionInClipRect:(NSRect)clipRect {
	[super highlightSelectionInClipRect:clipRect];
	
	NSRange rowRange = [self rowsInRect:clipRect];
	
	for (NSUInteger row = rowRange.location; row < NSMaxRange(rowRange); row++) {
		WCGradientBackgroundTextFieldCell *cell = (WCGradientBackgroundTextFieldCell *)[self preparedCellAtColumn:0 row:row];
		if ([cell drawGradient]) {
			NSRect rowRect = [self rectOfRow:row];
			
			[[cell gradient] drawInRect:rowRect angle:90.0];
			[[cell borderColor] setFill];
			if (row != 0)
				NSRectFill(NSMakeRect(rowRect.origin.x, rowRect.origin.y, rowRect.size.width, 1.0));
			rowRect.origin.y += rowRect.size.height - 1.0;
			NSRectFill(NSMakeRect(rowRect.origin.x, rowRect.origin.y, rowRect.size.width, 1.0));
		}
	}
}

@end
