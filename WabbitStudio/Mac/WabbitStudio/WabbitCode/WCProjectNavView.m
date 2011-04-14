//
//  WCProjectNavView.m
//  WabbitStudio
//
//  Created by William Towe on 4/9/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectNavView.h"
#import "WCDefines.h"
#import "WCProject.h"


@implementation WCProjectNavView

+ (Class)cellClass {
	return [NSActionCell class];
}

- (id)initWithFrame:(NSRect)frameRect {
	if (!(self = [super initWithFrame:frameRect]))
		return nil;
	
	_images = [[NSArray alloc] initWithObjects:[NSImage imageNamed:@"Directory16x16"],[NSImage imageNamed:@"Symbols16x16"],[NSImage imageNamed:@"Search16x16"],[NSImage imageNamed:@"ErrorsAndWarnings16x16"], nil];
	_selectors = [[NSArray alloc] initWithObjects:NSStringFromSelector(@selector(viewProject:)),NSStringFromSelector(@selector(viewSymbols:)),NSStringFromSelector(@selector(viewSearch:)),NSStringFromSelector(@selector(viewBuildMessages:)), nil];
	
	return self;
}

- (void)dealloc {
	[_images release];
    [super dealloc];
}

- (void)mouseDown:(NSEvent *)theEvent {
	
	NSRect bounds = [self bounds];
	NSRectArray rects = calloc([[self images] count], sizeof(NSRect));
	CGFloat startX = floor(NSWidth(bounds)/2.0) - floor(([[[self images] lastObject] size].width * 2 * [[self images] count])/2.0);
	//CGFloat startX = floor([[[self images] lastObject] size].width/2.0);
	//CGFloat startX = bounds.origin.x;
	NSUInteger index;
	
	for (index = 0; index < [[self images] count]; index++) {
		NSImage *image = [[self images] objectAtIndex:index];
		NSSize size = [image size];
		NSRect frame = NSMakeRect(startX + (size.width * 2 * index), bounds.origin.y, size.width * 2, NSHeight(bounds));
		
		rects[index] = frame;
	}
	
	NSPoint point = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	for (index = 0; index < [[self images] count]; index++) {
		if (NSPointInRect(point, rects[index])) {
			[self setSelectedIndex:index];
			break;
		}
	}
	
	// create a pool to flush each time through the cycle
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	// track!
	NSEvent *event = nil;
	while([event type] != NSLeftMouseUp) {
		[pool release];
		pool = [[NSAutoreleasePool alloc] init];
		
		event = [[self window] nextEventMatchingMask: NSLeftMouseDraggedMask | NSLeftMouseUpMask];

		NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];

		for (index = 0; index < [[self images] count]; index++) {
			if (NSPointInRect(p, rects[index])) {
				[self setSelectedIndex:index];
				break;
			}
		}
	}
	
	[self sendAction:[self action] to:[self target]];
	
	//[self sendAction:@selector(changeProjectView:) to:_project];
	[pool release];
	free(rects);
}

- (void)drawRect:(NSRect)dirtyRect {
	NSRect bounds = [self bounds];
	
	[[[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedWhite:0.95 alpha:1.0],0.0,[NSColor colorWithCalibratedWhite:0.83 alpha:1.0],11.5/23,[NSColor colorWithCalibratedWhite:0.95 alpha:1.0],11.5/23,[NSColor colorWithCalibratedWhite:0.92 alpha:1.0],1.0, nil] autorelease] drawInRect:bounds angle:90.0];
	CGFloat startX = floor(NSWidth(bounds)/2.0) - floor(([[[self images] lastObject] size].width * 2 * [[self images] count])/2.0);
	//CGFloat startX = floor([[[self images] lastObject] size].width/2.0);
	//CGFloat startX = bounds.origin.x;
	NSUInteger index;
	
	for (index = 0; index < [[self images] count]; index++) {
		NSImage *image = [[self images] objectAtIndex:index];
		NSSize size = [image size];
		NSRect frame = NSMakeRect(startX + (size.width * 2 * index), bounds.origin.y, size.width * 2, NSHeight(bounds));
		
		if (index == [self selectedIndex]) {
			[[[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedWhite:0.8 alpha:1.0],0.0,[NSColor colorWithCalibratedWhite:0.64 alpha:1.0],11.5/23,[NSColor colorWithCalibratedWhite:0.8 alpha:1.0],11.5/23,[NSColor colorWithCalibratedWhite:0.77 alpha:1.0],1.0, nil] autorelease] drawInRect:frame angle:90.0];
			//[[[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedRed:0.58 green:0.86 blue:0.98 alpha:1.0],0.0,[NSColor colorWithCalibratedRed:0.42 green:0.68 blue:0.9 alpha:1.0],11.5/23,[NSColor colorWithCalibratedRed:0.64 green:0.8 blue:0.94 alpha:1.0],11.5/23,[NSColor colorWithCalibratedRed:0.56 green:0.7 blue:0.9 alpha:1.0],1.0, nil] autorelease] drawInRect:frame angle:90.0];
			//[[[[NSGradient alloc] initWithStartingColor:[NSColor colorWithCalibratedWhite:0.85 alpha:1.0] endingColor:[NSColor colorWithCalibratedWhite:0.95 alpha:1.0]] autorelease] drawInRect:frame angle:90.0];
			[[NSColor colorWithCalibratedWhite:0.5 alpha:1.0] setFill];
			NSRectFill(NSMakeRect(frame.origin.x, frame.origin.y, 1.0, NSHeight(frame)));
			NSRectFill(NSMakeRect(frame.origin.x+NSWidth(frame), frame.origin.y, 1.0, NSHeight(frame)));
		}
		
		//[[NSColor colorWithCalibratedWhite:0.64 alpha:1.0] setFill];
		//NSRectFill(NSMakeRect(frame.origin.x, frame.origin.y, 1.0, NSHeight(frame)));
		//NSRectFill(NSMakeRect(frame.origin.x+NSWidth(frame), frame.origin.y, 1.0, NSHeight(frame)));
		
		[image setSize:NSMakeSize(16.0, 16.0)];
		size = [image size];
		[image drawInRect:WCCenteredRect(NSMakeRect(0.0, 0.0, size.width, size.height), frame) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
	}
	
	[[NSColor colorWithCalibratedWhite:0.33 alpha:1.0] setFill];
	NSRectFill(NSMakeRect(0.0, 0.0, NSWidth(bounds), 1.0));
}

@synthesize images=_images;
@synthesize selectors=_selectors;
@dynamic selectedIndex;
- (NSUInteger)selectedIndex {
	return _selectedIndex;
}
- (void)setSelectedIndex:(NSUInteger)selectedIndex {
	if (_selectedIndex == selectedIndex)
		return;
	
	_selectedIndex = selectedIndex;
	
	[self setNeedsDisplay:YES];
}
@end
