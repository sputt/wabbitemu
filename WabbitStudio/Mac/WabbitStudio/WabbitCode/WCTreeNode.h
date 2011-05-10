//
//  WCTreeNode.h
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCObject.h"


extern NSString* const kWCTreeNodeChildNodesKey;

@interface WCTreeNode : WCObject <NSCoding,NSCopying,NSMutableCopying,WCPlistRepresentation> {
@private
	__weak id _parentNode; // back pointer to our parent node; not retained
    NSMutableArray *_childNodes;
}
@property (readonly,nonatomic) id parentNode;
@property (readonly,nonatomic) NSArray *childNodes;
@property (readonly,nonatomic) NSMutableArray *mutableChildNodes; // returns a proxy through mutableArrayValueForKey:
@property (readonly,nonatomic) BOOL isLeaf;
@property (readonly,nonatomic) NSUInteger countOfChildNodes;
@property (readonly,nonatomic) NSArray *descendantNodes;
@property (readonly,nonatomic) NSArray *descendantNodesInclusive;
@property (readonly,nonatomic) NSArray *descendantLeafNodes;
@property (readonly,nonatomic) NSArray *descendantLeafNodesInclusive;
@property (readonly,nonatomic) NSArray *descendantGroupNodes;
@property (readonly,nonatomic) NSArray *descendantGroupNodesInclusive;

- (BOOL)isDescendantOfNode:(WCTreeNode *)node;

@end
