//
//  AIArrayAdditions.h
//  AIUtilities.framework
//
//  Created by Evan Schoenberg on 2/15/05.
//  Copyright 2005 The Adium Team. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface NSArray (AIArrayAdditions)
- (BOOL)containsObjectIdenticalTo:(id)obj;
+ (NSArray *)arrayNamed:(NSString *)name forClass:(Class)inClass;
- (NSComparisonResult)compare:(NSArray *)other;
- (BOOL)validateAsPropertyList;
@end

@interface NSMutableArray (ESArrayAdditions)
- (void)addObjectsFromArrayIgnoringDuplicates:(NSArray *)inArray;
- (void)moveObject:(id)object toIndex:(NSUInteger)newIndex;
- (void)moveObjects:(NSArray *)objects toIndexes:(NSIndexSet *)newIndexes;
- (void)setObject:(id)object atIndex:(NSUInteger)index;
@end
