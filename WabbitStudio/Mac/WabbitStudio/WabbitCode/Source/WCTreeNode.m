//
//  WCTreeNode.m
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"


NSString* const kWCTreeNodeChildNodesKey = @"childNodes";
NSString* const kWCTreeNodeIsLeafNodeKey = @"isLeafNode";

@implementation WCTreeNode
#pragma mark *** Subclass Overrides ***
- (void)dealloc {
	_parentNode = nil;
	[_childNodes release];
    [super dealloc];
}

- (id)init {
    self = [super init];
    if (self) {
        //_childNodes = [[NSMutableArray alloc] init];
    }
    
    return self;
}

- (NSDictionary *)dictionaryRepresentation {
	// first grab super's dictionary
	NSMutableDictionary *retval = [NSMutableDictionary dictionaryWithDictionary:[super dictionaryRepresentation]];
	
	// add our keys to it
	[retval addEntriesFromDictionary:[NSDictionary dictionaryWithObjectsAndKeys:[[self childNodes] valueForKeyPath:kWCObjectDictionaryRepresentationKey],kWCTreeNodeChildNodesKey, nil]];
	
	// return a copy
	return [[retval copy] autorelease];
}
#pragma mark *** Protocol Overrides ***
#pragma mark NSKeyValueObserving
+ (NSSet *)keyPathsForValuesAffectingValueForKey:(NSString *)key {
	if ([key isEqualToString:kWCTreeNodeIsLeafNodeKey])
		return [[super keyPathsForValuesAffectingValueForKey:key] setByAddingObject:kWCTreeNodeChildNodesKey];
	return [super keyPathsForValuesAffectingValueForKey:key];
}
#pragma mark NSCoding
- (id)initWithCoder:(NSCoder *)coder {
	if (![super initWithCoder:coder])
		return nil;
	
	for (id node in [coder decodeObjectForKey:kWCTreeNodeChildNodesKey])
		[self insertObject:node inChildNodesAtIndex:[self countOfChildNodes]];
	
	return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
	[super encodeWithCoder:coder];
	
	[coder encodeObject:self.childNodes forKey:kWCTreeNodeChildNodesKey];
}
#pragma mark NSCopying
- (id)copyWithZone:(NSZone *)zone {
	WCTreeNode *copy = [super copyWithZone:zone];
	
	copy->_parentNode = _parentNode;
	copy->_childNodes = [_childNodes retain];
	
	return copy;
}
#pragma mark NSMutableCopying
- (id)mutableCopyWithZone:(NSZone *)zone {
	WCTreeNode *copy = [super mutableCopyWithZone:zone];
	
	copy->_parentNode = _parentNode;
	NSMutableArray *cnodes = [[NSMutableArray alloc] initWithCapacity:[_childNodes count]];
	copy->_childNodes = cnodes;
	for (WCTreeNode *node in _childNodes)
		[copy addObjectToChildNodes:[[node mutableCopy] autorelease]];
	
	return copy;
}
#pragma mark *** Public Methods ***
- (NSArray *)descendantNodes; {
	NSMutableArray *retval = [NSMutableArray array];
	
	for (WCTreeNode *node in [self childNodes]) {
		[retval addObject:node];
		
		if (![node isLeafNode])
			[retval addObjectsFromArray:[node descendantNodes]];
	}
	
	return [[retval copy] autorelease];
}

- (NSArray *)descendantNodesInclusive; {
	return [[self descendantNodes] arrayByAddingObject:self];
}

- (NSArray *)descendantLeafNodes; {
	NSMutableArray *retval = [NSMutableArray array];
	
	for (WCTreeNode *node in [self childNodes]) {
		if (![node isLeafNode])
			[retval addObjectsFromArray:[node descendantLeafNodes]];
		else
			[retval addObject:node];
	}
	
	return [[retval copy] autorelease];
}
- (NSArray *)descendantLeafNodesInclusive; {
	if ([self isLeafNode])
		return [NSArray arrayWithObject:self];
	return [self descendantLeafNodes];
}

- (BOOL)isDescendantOfNode:(WCTreeNode *)node; {
	return [[node descendantNodes] containsObject:self];
}
#pragma mark NSKeyValueCoding
- (NSUInteger)countOfChildNodes; {
	return [_childNodes count];
}
- (id)objectInChildNodesAtIndex:(NSUInteger)index; {
	return [_childNodes objectAtIndex:index];
}

- (void)addObjectToChildNodes:(id)object; {
	[self insertObject:object inChildNodesAtIndex:[self countOfChildNodes]];
}
- (void)insertObject:(id)object inChildNodesAtIndex:(NSUInteger)index; {
	if (!_childNodes)
		_childNodes = [[NSMutableArray alloc] init];
	
	[object setParentNode:self];
	
	[_childNodes insertObject:object atIndex:index];
}
- (void)removeObjectFromChildNodesAtIndex:(NSUInteger)index; {
	[[_childNodes objectAtIndex:index] setParentNode:nil];
	
	[_childNodes removeObjectAtIndex:index];
}
#pragma mark Accessors
@synthesize parentNode=_parentNode;
@synthesize childNodes=_childNodes;
@dynamic mutableChildNodes;
- (NSMutableArray *)mutableChildNodes {
	if (!_childNodes)
		_childNodes = [[NSMutableArray alloc] init];
	
	return [self mutableArrayValueForKey:kWCTreeNodeChildNodesKey];
}
@dynamic isLeafNode;
- (BOOL)isLeafNode {
	return ([self countOfChildNodes] == 0)?YES:NO;
}
@end
