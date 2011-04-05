//
//  WCObject.m
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCObject.h"
#import <AppKit/NSImage.h>

NSString* const kWCObjectDictionaryRepresentationKey = @"dictionaryRepresentation";

static NSString* const kWCObjectNameKey = @"name";
static NSString* const kWCObjectClassKey = @"class";

@implementation WCObject
#pragma mark *** Subclass Overrides ***
- (void)dealloc {
	[_name release];
	[_icon release];
    [super dealloc];
}
#pragma mark *** Protocol Overrides ***
#pragma mark NSCoding
- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super init]))
		return nil;
	
	_name = [[coder decodeObjectForKey:kWCObjectNameKey] copy];
	
	return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
	[coder encodeObject:[self name] forKey:kWCObjectNameKey];
}
#pragma mark NSCopying
- (id)copyWithZone:(NSZone *)zone {
	WCObject *copy = [[[self class] alloc] init];
	
	copy->_name = [_name retain];
	copy->_icon = [_icon retain];
	
	return copy;
}
#pragma mark NSMutableCopying
- (id)mutableCopyWithZone:(NSZone *)zone {
	WCObject *copy = [[[self class] alloc] init];
	
	copy->_name = [_name copy];
	copy->_icon = [_icon retain];
	
	return copy;
}
#pragma mark *** Public Methods ***
+ (id)objectWithName:(NSString *)name; {
	return [self objectWithName:name icon:nil];
}
- (id)initWithName:(NSString *)name; {
	return [self initWithName:name icon:nil];
}

+ (id)objectWithName:(NSString *)name icon:(NSImage *)icon; {
	return [[[[self class] alloc] initWithName:name icon:icon] autorelease];
}
- (id)initWithName:(NSString *)name icon:(NSImage *)icon; {
	if (!(self = [super init]))
		return nil;
	
	_name = [name copy];
	_icon = [icon retain];
	
	return self;
}
#pragma mark Accessors
@synthesize name=_name;
@synthesize icon=_icon;

@dynamic dictionaryRepresentation;
- (NSDictionary *)dictionaryRepresentation {
	// this is the base class, just return a new dictionary with our values
	return [NSDictionary dictionaryWithObjectsAndKeys:[self className],kWCObjectClassKey,[self name],kWCObjectNameKey, nil];
}

@end
