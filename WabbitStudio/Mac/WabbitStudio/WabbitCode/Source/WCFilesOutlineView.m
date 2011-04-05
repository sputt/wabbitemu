//
//  WCFilesOutlineView.m
//  WabbitStudio
//
//  Created by William Towe on 3/21/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFilesOutlineView.h"


NSString* const WCFilesOutlineViewPboardType = @"WCFilesOutlineViewPboardType";

@implementation WCFilesOutlineView

- (id)initWithFrame:(NSRect)frameRect {
	if (!(self = [super initWithFrame:frameRect]))
		return nil;
	
	[self registerForDraggedTypes:[NSArray arrayWithObjects:WCFilesOutlineViewPboardType,NSFilenamesPboardType,nil]];
	[self setDraggingSourceOperationMask:(NSDragOperationCopy | NSDragOperationMove) forLocal:YES];
	[self setDraggingSourceOperationMask:NSDragOperationCopy forLocal:NO];
	
	return self;
}

- (id)initWithCoder:(NSCoder *)decoder {
	if (!(self = [super initWithCoder:decoder]))
		return nil;
	
	[self registerForDraggedTypes:[NSArray arrayWithObjects:WCFilesOutlineViewPboardType,NSFilenamesPboardType,nil]];
	[self setDraggingSourceOperationMask:(NSDragOperationCopy | NSDragOperationMove) forLocal:YES];
	[self setDraggingSourceOperationMask:NSDragOperationCopy forLocal:NO];
	
	return self;
}

- (void)mouseDown:(NSEvent *)event {
	if ([event type] == NSLeftMouseDown && [event clickCount] == 2) {
		[self sendAction:[self doubleAction] to:nil];
		return;
	}
	[super mouseDown:event];
}
@end
