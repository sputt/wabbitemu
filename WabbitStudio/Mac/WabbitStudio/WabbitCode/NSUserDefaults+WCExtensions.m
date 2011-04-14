//
//  NSUserDefaults+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 3/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSUserDefaults+WCExtensions.h"


@implementation NSUserDefaults (NSUserDefaults_WCExtensions)
- (NSUInteger)unsignedIntegerForKey:(NSString *)key; {
	return [[self objectForKey:key] unsignedIntegerValue];
}
- (NSColor *)colorForKey:(NSString *)key; {
	return (NSColor *)[NSKeyedUnarchiver unarchiveObjectWithData:[self objectForKey:key]];
}
- (NSFont *)fontForKey:(NSString *)key; {
	return (NSFont *)[NSKeyedUnarchiver unarchiveObjectWithData:[self objectForKey:key]];
}
@end
