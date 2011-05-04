//
//  RSNavigatorControl.m
//  WabbitStudio
//
//  Created by William Towe on 5/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSNavigatorControl.h"
#import "NSGradient+WCExtensions.h"
#import "WCDefines.h"


@implementation RSNavigatorControl

+ (Class)cellClass {
	return [NSActionCell class];
}

- (id)initWithFrame:(NSRect)frameRect {
	if (!(self = [super initWithFrame:frameRect]))
		return nil;
	
	_selectedItemIndex = NSNotFound;
	_pressedItemIndex = NSNotFound;
	
	return self;
}

- (id)initWithCoder:(NSCoder *)decoder {
	if (!(self = [super initWithCoder:decoder]))
		return nil;
	
	_selectedItemIndex = NSNotFound;
	_pressedItemIndex = NSNotFound;
	
	return self;
}

- (void)dealloc {
	_swapView = nil;
	_dataSource = nil;
    [super dealloc];
}

- (void)mouseDown:(NSEvent *)theEvent {
	NSRect bounds = [self bounds];
	NSUInteger numberOfItems = [[self dataSource] numberOfItemsInNavigatorControl:self];
	CGFloat itemWidth = [[self dataSource] itemWidthForNavigatorControl:self];
	NSRectArray rectsForItems = malloc(numberOfItems*sizeof(NSRect));
	CGFloat totalItemWidth = (numberOfItems*itemWidth);
	CGFloat startX = NSMinX(bounds) + floor(NSWidth(bounds)/2.0) - floor(totalItemWidth/2.0);
	NSUInteger itemIndex;
	
	NSPoint fLocation = [self convertPointFromBase:[theEvent locationInWindow]];
	
	for (itemIndex = 0; itemIndex < numberOfItems; itemIndex++) {
		rectsForItems[itemIndex] = NSMakeRect(startX+(itemIndex*itemWidth), NSMinY(bounds), itemWidth, NSHeight(bounds));
		
		if (NSPointInRect(fLocation, rectsForItems[itemIndex]))
			[self setPressedItemIndex:itemIndex];
	}
	
	// create a pool to flush each time through the cycle
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	// track!
	NSEvent *event = nil;
	while([event type] != NSLeftMouseUp) {
		[pool drain];
		pool = [[NSAutoreleasePool alloc] init];
		
		event = [[self window] nextEventMatchingMask: NSLeftMouseDraggedMask | NSLeftMouseUpMask];
		
		fLocation = [self convertPointFromBase:[event locationInWindow]];
		
		for (itemIndex = 0; itemIndex < numberOfItems; itemIndex++) {
			if (NSPointInRect(fLocation, rectsForItems[itemIndex])) {
				[self setPressedItemIndex:itemIndex];
				break;
			}
		}
	}
	
	if ([self pressedItemIndex] != NSNotFound)
		[self setSelectedItemIndex:[self pressedItemIndex]];
	
	[self setPressedItemIndex:NSNotFound];
	[pool drain];
	free(rectsForItems);
}

- (void)drawRect:(NSRect)dirtyRect {
	NSRect bounds = [self bounds];
	
	[[NSGradient unifiedNormalGradient] drawInRect:bounds angle:90.0];
	
	if ([self dataSource] == nil)
		return;
	
	NSUInteger numberOfItems = [[self dataSource] numberOfItemsInNavigatorControl:self];
	CGFloat itemWidth = [[self dataSource] itemWidthForNavigatorControl:self];
	CGFloat totalItemWidth = (numberOfItems*itemWidth);
	CGFloat startX = NSMinX(bounds) + floor(NSWidth(bounds)/2.0) - floor(totalItemWidth/2.0);
	NSSize imageSize = [[self dataSource] imageSizeForNavigatorControl:self];
	NSUInteger itemIndex;
	
	for (itemIndex = 0; itemIndex < numberOfItems; itemIndex++) {
		NSRect rectForItem = NSMakeRect(startX + (itemIndex*itemWidth), NSMinY(bounds), itemWidth, NSHeight(bounds));
		NSImage *imageForItem = [[self dataSource] navigatorControl:self imageForItemAtIndex:itemIndex];
		
		if ([self selectedItemIndex] == itemIndex) {
			[[NSGradient unifiedPressedGradient] drawInRect:rectForItem angle:90.0];
			[[NSColor lightGrayColor] setFill];
			NSRectFill(NSMakeRect(NSMinX(rectForItem), NSMinY(rectForItem), 1.0, NSHeight(rectForItem)));
			NSRectFill(NSMakeRect(NSMinX(rectForItem)+NSWidth(rectForItem), NSMinY(rectForItem), 1.0, NSHeight(rectForItem)));
		}
		else if ([self pressedItemIndex] == itemIndex) {
			[[NSGradient unifiedSelectedGradient] drawInRect:rectForItem angle:90.0];
			[[NSColor lightGrayColor] setFill];
			NSRectFill(NSMakeRect(NSMinX(rectForItem), NSMinY(rectForItem), 1.0, NSHeight(rectForItem)));
			NSRectFill(NSMakeRect(NSMinX(rectForItem)+NSWidth(rectForItem), NSMinY(rectForItem), 1.0, NSHeight(rectForItem)));
		}
		
		[imageForItem setSize:imageSize];
		[imageForItem drawInRect:WCCenteredRectWithSize(imageSize, rectForItem) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
	}
	
	[[NSColor lightGrayColor] setFill];
	NSRectFill(NSMakeRect(NSMinX(bounds), NSMinY(bounds), NSWidth(bounds), 1.0));
}

@dynamic dataSource;
- (id<RSNavigatorDataSource>)dataSource {
	return _dataSource;
}
- (void)setDataSource:(id<RSNavigatorDataSource>)dataSource {
	if (_dataSource == dataSource)
		return;
	
	_dataSource = dataSource;
	
	[self setNeedsDisplay:YES];
}
@dynamic selectedItemIndex;
- (NSUInteger)selectedItemIndex {
	return _selectedItemIndex;
}
- (void)setSelectedItemIndex:(NSUInteger)selectedItemIndex {
	if (_selectedItemIndex == selectedItemIndex)
		return;
	
	_selectedItemIndex = selectedItemIndex;
	
	NSView *newView = [[self dataSource] navigatorControl:self viewForItemAtIndex:[self selectedItemIndex]];
	
	[newView setFrameSize:[[self swapView] frame].size];
	
	if ([[[self swapView] subviews] count] == 0)
		[[self swapView] addSubview:newView];
	else
		[[self swapView] replaceSubview:[[[self swapView] subviews] lastObject] with:newView];
	
	[self setNeedsDisplay:YES];
}
@dynamic pressedItemIndex;
- (NSUInteger)pressedItemIndex {
	return _pressedItemIndex;
}
- (void)setPressedItemIndex:(NSUInteger)pressedItemIndex {
	if (_pressedItemIndex == pressedItemIndex)
		return;
	
	_pressedItemIndex = pressedItemIndex;
	
	[self setNeedsDisplay:YES];
}
@synthesize swapView=_swapView;

@end
