//
//  NSOutlineView+WCExtensions.m
//  sourcebrowser
//
//  Created by William Towe on 1/8/09.
//  Copyright 2009 Revolution Software. All rights reserved.
//

#import "NSOutlineView+WCExtensions.h"
#import "NSTreeController+WCExtensions.h"
#import "WCFile.h"


@implementation NSOutlineView (WCExtensions)
// returns an array of UUIDs corresponding to expanded items
- (NSArray *)expandedItemUUIDs; {
	NSMutableArray *retval = [NSMutableArray array];
	NSUInteger row, numberOfRows = [self numberOfRows];
	for (row = 0; row < numberOfRows; row++) {
		NSTreeNode *node = [self itemAtRow:row];
		if ([self isItemExpanded:node])
			[retval addObject:[[node representedObject] UUID]];
	}
	return [[retval copy] autorelease];
}
// expands the items with the given UUIDs; this assumes we have an NSTreeController as our data source
- (void)expandItemsWithUUIDs:(NSArray *)UUIDs; {
	NSArray *nodes = [(NSTreeController *)[self dataSource] treeNodes];
	for (NSString *UUID in UUIDs) {
		for (NSTreeNode *node in nodes) {
			if ([[[node representedObject] UUID] isEqualToString:UUID]) {
				[self expandItem:node];
				break;
			}
		}
	}
}
@end
