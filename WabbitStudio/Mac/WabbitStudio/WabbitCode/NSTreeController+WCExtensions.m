//
//  NSTreeController+WCExtensions.m
//  files
//
//  Created by William Towe on 5/4/09.
//  Copyright 2009 Revolution Software. All rights reserved.
//

#import "NSTreeController+WCExtensions.h"
#import "NSArray+WCExtensions.h"
#import "NSTreeNode+WCExtensions.h"
//#import "AIArrayAdditions.h"


@implementation NSTreeController (NSTreeController_WCExtensions)
// returns an array of all the NSTreeNode objects maintained by the receiver
- (NSArray *)treeNodes; {
	NSMutableArray *nodes = [NSMutableArray array];
	
	for (NSTreeNode *node in [[self arrangedObjects] childNodes]) {
		[nodes addObject:node];
		if (![node isLeaf])
			[nodes addObjectsFromArray:[node descendantNodes]];
	}
	return [[nodes copy] autorelease];
}
// returns the selected NSTreeNode object, or if multiple selection is enabled, the first selected NSTreeNode object
- (NSTreeNode *)selectedNode; {
	return [[self selectedNodes] firstObject];
}
// returns the real model object from the above method
- (id)selectedRepresentedObject; {
	return [[self selectedNode] representedObject];
}
// returns the array of selected real model objects
- (NSArray *)selectedRepresentedObjects; {
	return [[self selectedNodes] valueForKey:@"representedObject"];
}
// returns the corresponding NSTreeNode for 'indexPath'
- (NSTreeNode *)treeNodeAtIndexPath:(NSIndexPath *)indexPath; {
	return [[self arrangedObjects] descendantNodeAtIndexPath:indexPath];
}
// returns an array of NSTreeNode objects given an array of NSIndexPath objects 'indexPaths'
- (NSArray *)treeNodesAtIndexPaths:(NSArray *)indexPaths; {
	NSMutableArray *retval = [NSMutableArray array];
	for (NSIndexPath *indexPath in indexPaths) {
		NSTreeNode *node = [self treeNodeAtIndexPath:indexPath];
		if (node)
			[retval addObject:node];
	}
	return [[retval copy] autorelease];
}
// returns the NSIndexPath for the real model object 'representedObject'
- (NSIndexPath *)indexPathForRepresentedObject:(id)representedObject; {
	for (NSTreeNode *node in [self treeNodes]) {
		if ([representedObject isEqual:[node representedObject]])
			return [node indexPath];
	}
	return nil;
}
// returns an array of NSIndexPath objects given an array of real model objects 'representedObjects'
- (NSArray *)indexPathsForRepresentedObjects:(NSArray *)representedObjects; {
	NSMutableArray *indexPaths = [NSMutableArray array];
	NSArray *nodes = [self treeNodes];
	
	for (id representedObject in representedObjects) {
		for (NSTreeNode *node in nodes) {
			if ([representedObject isEqual:[node representedObject]]) {
				[indexPaths addObject:[node indexPath]];
				break;
			}
		}
	}
	return [[indexPaths copy] autorelease];
}
// returns the corresponding NSTreeNode object for the real model object 'representedObject'
- (NSTreeNode *)treeNodeForRepresentedObject:(id)representedObject; {
	for (NSTreeNode *node in [self treeNodes]) {
		if ([representedObject isEqual:[node representedObject]])
			return node;
	}
	return nil;
}
// returns an array of corresponding NSTreeNode objects for the array of real model objects 'representedObjects'
- (NSArray *)treeNodesForRepresentedObjects:(NSArray *)representedObjects; {
	NSMutableArray *treeNodes = [NSMutableArray array];
	NSArray *nodes = [self treeNodes];
	
	for (id representedObject in representedObjects) {
		for (NSTreeNode *node in nodes) {
			if ([representedObject isEqual:[node representedObject]]) {
				[treeNodes addObject:node];
				break;
			}
		}
	}
	return [[treeNodes copy] autorelease];
}
// selects 'treeNode' using its index path
- (void)setSelectedTreeNode:(NSTreeNode *)treeNode; {
	[self setSelectedTreeNodes:[NSArray arrayWithObject:treeNode]];
}
// selects an array of NSTreeNode objects 'treeNodes' using their index paths
- (void)setSelectedTreeNodes:(NSArray *)treeNodes; {
	[self setSelectionIndexPaths:[treeNodes valueForKey:@"indexPath"]];
}
// selects the real model object 'representedObject'
- (void)setSelectedRepresentedObject:(id)representedObject; {
	[self setSelectedRepresentedObjects:[NSArray arrayWithObject:representedObject]];
}
// selects an array of real model objects 'representedObjects'
- (void)setSelectedRepresentedObjects:(NSArray *)representedObjects; {
	NSMutableArray *indexPaths = [NSMutableArray array];
	NSArray *nodes = [self treeNodes];
	
	for (id representedObject in representedObjects) {
		for (NSTreeNode *node in nodes) {
			if ([representedObject isEqual:[node representedObject]]) {
				[indexPaths addObject:[node indexPath]];
				break;
			}
		}
	}
	[self setSelectionIndexPaths:indexPaths];
}
/*
- (void)removeSelectedNodes; {
	[self removeObjectsAtArrangedObjectIndexPaths:[self selectionIndexPaths]];
}

- (void)removeTreeNodes:(NSArray *)treeNodes; {
	[self removeObjectsAtArrangedObjectIndexPaths:[treeNodes valueForKey:@"indexPath"]];
}

- (void)removeRepresentedObject:(id)representedObject; {
	[self removeRepresentedObjects:[NSArray arrayWithObject:representedObject]];
}

- (void)removeRepresentedObjects:(NSArray *)representedObjects; {
	NSMutableArray *indexPaths = [NSMutableArray array];
	NSArray *nodes = [self treeNodes];
	for (id object in representedObjects) {
		for (NSTreeNode *node in nodes) {
			if ([[node representedObject] isEqual:object]) {
				[indexPaths addObject:[node indexPath]];
				break;
			}
		}
	}
	[self removeObjectsAtArrangedObjectIndexPaths:indexPaths];
}
 */
@end
