//
//  WCIncludeDirectory.m
//  WabbitStudio
//
//  Created by William Towe on 3/29/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCIncludeDirectory.h"
#import "WCAlias.h"


@implementation WCIncludeDirectory

- (void)dealloc {
	[_alias release];
    [super dealloc];
}

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	_alias = [[coder decodeObjectForKey:@"alias"] retain];
	
	return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
	[coder encodeObject:[self alias] forKey:@"alias"];
	[super encodeWithCoder:coder];
}

- (id)copyWithZone:(NSZone *)zone {
	WCIncludeDirectory *copy = [super copyWithZone:zone];
	
	copy->_alias = [_alias retain];
	
	return copy;
}

- (id)mutableCopyWithZone:(NSZone *)zone {
	WCIncludeDirectory *copy = [super mutableCopyWithZone:zone];
	
	copy->_alias = [[WCAlias alloc] initWithURL:[self URL]];
	
	return copy;
}

- (NSString *)name {
	return [[self absolutePathForDisplay] lastPathComponent];
}

- (NSImage *)icon {
	return [NSImage imageNamed:@"Directory16x16"];
}

@synthesize alias=_alias;
@dynamic URL;
- (NSURL *)URL {
	return [[self alias] fileURL];
}
- (void)setURL:(NSURL *)URL {
	[[self alias] setFileURL:URL];
}
@dynamic absolutePathForDisplay;
- (NSString *)absolutePathForDisplay {
	return [[self alias] absolutePathForDisplay];
}

+ (id)includeDirectoryWithURL:(NSURL *)url; {
	return [[[[self class] alloc] initWithURL:url] autorelease];
}
- (id)initWithURL:(NSURL *)url; {
	if (!(self = [super initWithName:nil]))
		return nil;
	
	_alias = [[WCAlias alloc] initWithURL:url];
	
	return self;
}
@end
