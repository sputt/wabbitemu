//
//  WCTreeNode.h
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCObject.h"


extern NSString* const kWCTreeNodeChildNodesKey;
extern NSString* const kWCTreeNodeIsLeafNodeKey;

@interface WCTreeNode : WCObject <NSCoding,NSCopying,NSMutableCopying> {
@private
	__weak id _parentNode; // back pointer to our parent node; not retained
    NSMutableArray *_childNodes;
}
@property (assign, nonatomic) id parentNode;
@property (readonly) NSArray *childNodes;
@property (readonly) NSMutableArray *mutableChildNodes; // returns a proxy through mutableArrayValueForKey:
@property (readonly) BOOL isLeafNode;

- (NSUInteger)countOfChildNodes;
- (id)objectInChildNodesAtIndex:(NSUInteger)index;

- (void)addObjectToChildNodes:(id)object;
- (void)insertObject:(id)object inChildNodesAtIndex:(NSUInteger)index;
- (void)removeObjectFromChildNodesAtIndex:(NSUInteger)index;

- (NSArray *)descendantNodes;
- (NSArray *)descendantNodesInclusive;
- (NSArray *)descendantLeafNodes;
- (NSArray *)descendantLeafNodesInclusive;

- (BOOL)isDescendantOfNode:(WCTreeNode *)node;

@end
