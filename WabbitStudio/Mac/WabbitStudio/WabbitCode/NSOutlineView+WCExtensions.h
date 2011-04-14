//
//  NSOutlineView+WCExtensions.h
//  sourcebrowser
//
//  Created by William Towe on 1/8/09.
//  Copyright 2009 Revolution Software. All rights reserved.
//

#import <AppKit/NSOutlineView.h>


@interface NSOutlineView (WCExtensions)
- (NSArray *)expandedItemUUIDs;
- (void)expandItemsWithUUIDs:(NSArray *)UUIDs;
@end
