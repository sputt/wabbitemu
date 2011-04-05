//
//  NSTreeNode+WCExtensions.h
//  files
//
//  Created by William Towe on 5/4/09.
//  Copyright 2009 Revolution Software. All rights reserved.
//

#import <AppKit/NSTreeNode.h>


@interface NSTreeNode (NSTreeNode_WCExtensions)
- (NSArray *)descendantNodes;
- (NSArray *)descendantLeafNodes;
- (NSArray *)descendantLeafNodesInclusive;
- (NSArray *)descendantGroupNodes;
- (NSArray *)descendantGroupNodesInclusive;

- (BOOL)isDescendantOfNode:(NSTreeNode *)node;
@end
