//
//  NSTreeNode+WCExtensions.m
//  files
//
//  Created by William Towe on 5/4/09.
//  Copyright 2009 Revolution Software. All rights reserved.
//

#import "NSTreeNode+WCExtensions.h"


@implementation NSTreeNode (NSTreeNode_WCExtensions)
// returns all the descendant nodes of the receiver in a flat array
- (NSArray *)descendantNodes; {
	NSMutableArray *retval = [NSMutableArray array];
	for (NSTreeNode *node in [self childNodes]) {
		[retval addObject:node];
		if (![node isLeaf])
			[retval addObjectsFromArray:[node descendantNodes]];
	}
	return [[retval copy] autorelease];
}
// returns all the leaf nodes of the receiver in a flat array; i.e. where the count of childNodes is 0
- (NSArray *)descendantLeafNodes; {
	NSMutableArray *retval = [NSMutableArray array];
	for (NSTreeNode *node in [self childNodes]) {
		if (![node isLeaf])
			[retval addObjectsFromArray:[node descendantLeafNodes]];
		else
			[retval addObject:node];
	}
	return [[retval copy] autorelease];
}
// same as above, but includes self
- (NSArray *)descendantLeafNodesInclusive; {
	if ([self isLeaf])
		return [[NSArray arrayWithObject:self] arrayByAddingObjectsFromArray:[self descendantLeafNodes]];
	return [self descendantLeafNodes];
}
- (NSArray *)descendantGroupNodes; {
	NSMutableArray *retval = [NSMutableArray array];
	for (NSTreeNode *node in [self childNodes]) {
		if (![node isLeaf]) {
			[retval addObject:node];
			[retval addObjectsFromArray:[node descendantGroupNodes]];
		}
	}
	return [[retval copy] autorelease];
}
- (NSArray *)descendantGroupNodesInclusive; {
	if (![self isLeaf])
		return [[NSArray arrayWithObject:self] arrayByAddingObjectsFromArray:[self descendantGroupNodes]];
	return [self descendantGroupNodes];
}
- (BOOL)isDescendantOfNode:(NSTreeNode *)node; {
	return [[node descendantNodes] containsObject:self];
}
@end
