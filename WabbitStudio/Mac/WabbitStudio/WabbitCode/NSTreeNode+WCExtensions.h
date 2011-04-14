//
//  NSTreeNode+WCExtensions.h
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSTreeNode.h>


@interface NSTreeNode (NSTreeNode_WCExtensions)
- (NSArray *)descendantNodes;
- (NSArray *)descendantNodesInclusive;
- (NSArray *)descendantLeafNodes;
- (NSArray *)descendantLeafNodesInclusive;
- (NSArray *)descendantGroupNodes;
- (NSArray *)descendantGroupNodesInclusive;

- (BOOL)isDescendantOfNode:(NSTreeNode *)node;
@end
