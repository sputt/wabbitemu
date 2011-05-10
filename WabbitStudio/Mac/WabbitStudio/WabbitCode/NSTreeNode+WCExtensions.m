//
//  NSTreeNode+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSTreeNode+WCExtensions.h"


@implementation NSTreeNode (NSTreeNode_WCExtensions)
- (NSArray *)descendantNodes; {
	NSMutableArray *retval = [NSMutableArray array];
	
	for (NSTreeNode *node in [self childNodes]) {
		[retval addObject:node];
		
		if (![node isLeaf])
			[retval addObjectsFromArray:[node descendantNodes]];
	}
	
	return [[retval copy] autorelease];
}
- (NSArray *)descendantNodesInclusive; {
	return [[self descendantNodes] arrayByAddingObject:self];
}
- (NSArray *)descendantLeafNodes; {
	NSMutableArray *retval = [NSMutableArray array];
	
	for (NSTreeNode *node in [self childNodes]) {
		if ([node isLeaf])
			[retval addObject:node];
		else
			[retval addObjectsFromArray:[node descendantLeafNodes]];
	}
	
	return [[retval copy] autorelease];
}
- (NSArray *)descendantLeafNodesInclusive; {
	if ([self isLeaf])
		return [NSArray arrayWithObject:self];
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
		return [[self descendantGroupNodes] arrayByAddingObject:self];
	return [NSArray array];
}

- (BOOL)isDescendantOfNode:(NSTreeNode *)node; {
	return [[node descendantNodes] containsObject:self];
}
@end
