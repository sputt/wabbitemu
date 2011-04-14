//
//  WCTreeNode.m
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"


NSString* const kWCTreeNodeChildNodesKey = @"childNodes";

@interface WCTreeNode ()
@property (readwrite,assign,nonatomic) id parentNode;

- (void)insertObject:(id)object inChildNodesAtIndex:(NSUInteger)index;
@end

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
	if ([key isEqualToString:@"isLeaf"])
		return [[super keyPathsForValuesAffectingValueForKey:key] setByAddingObject:kWCTreeNodeChildNodesKey];
	return [super keyPathsForValuesAffectingValueForKey:key];
}
#pragma mark NSCoding
- (id)initWithCoder:(NSCoder *)coder {
	if (![super initWithCoder:coder])
		return nil;
	
	for (id node in [coder decodeObjectForKey:kWCTreeNodeChildNodesKey])
		[self insertObject:node inChildNodesAtIndex:[_childNodes count]];
	
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
		[copy insertObject:[[node mutableCopy] autorelease] inChildNodesAtIndex:[cnodes count]];
	
	return copy;
}
#pragma mark *** Public Methods ***
- (BOOL)isDescendantOfNode:(WCTreeNode *)node; {
	return [[node descendantNodes] containsObject:self];
}
#pragma mark NSKeyValueCoding
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
@dynamic isLeaf;
- (BOOL)isLeaf {
	return ([self countOfChildNodes] == 0)?YES:NO;
}
@dynamic countOfChildNodes;
- (NSUInteger)countOfChildNodes {
	return [_childNodes count];
}
@dynamic descendantNodes;
- (NSArray *)descendantNodes; {
	NSMutableArray *retval = [NSMutableArray array];
	
	for (WCTreeNode *node in [self childNodes]) {
		[retval addObject:node];
		
		if (![node isLeaf])
			[retval addObjectsFromArray:[node descendantNodes]];
	}
	
	return [[retval copy] autorelease];
}
@dynamic descendantNodesInclusive;
- (NSArray *)descendantNodesInclusive; {
	return [[self descendantNodes] arrayByAddingObject:self];
}
@dynamic descendantLeafNodes;
- (NSArray *)descendantLeafNodes; {
	NSMutableArray *retval = [NSMutableArray array];
	
	for (WCTreeNode *node in [self childNodes]) {
		if ([node isLeaf])
			[retval addObject:node];
		else
			[retval addObjectsFromArray:[node descendantLeafNodes]];
	}
	
	return [[retval copy] autorelease];
}
@dynamic descendantLeafNodesInclusive;
- (NSArray *)descendantLeafNodesInclusive; {
	if ([self isLeaf])
		return [NSArray arrayWithObject:self];
	return [self descendantLeafNodes];
}
@dynamic descendantGroupNodes;
- (NSArray *)descendantGroupNodes; {
	NSMutableArray *retval = [NSMutableArray array];
	
	for (WCTreeNode *node in [self childNodes]) {
		if (![node isLeaf]) {
			[retval addObject:node];
			[retval addObjectsFromArray:[node descendantGroupNodes]];
		}
	}
	
	return [[retval copy] autorelease];
}
@dynamic descendantGroupNodesInclusive;
- (NSArray *)descendantGroupNodesInclusive; {
	if (![self isLeaf])
		return [[self descendantGroupNodes] arrayByAddingObject:self];
	return [NSArray array];
}
@end
