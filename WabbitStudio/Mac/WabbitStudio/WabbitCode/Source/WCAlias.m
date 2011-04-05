//
//  WCAlias.m
//  WabbitStudio
//
//  Created by William Towe on 3/29/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCAlias.h"
#import "NDAlias.h"


@implementation WCAlias

- (void)dealloc {
	[_alias release];
	[_cachedUTI release];
    [super dealloc];
}

- (NSDictionary *)dictionaryRepresentation {
	NSMutableDictionary *retval = [[[super dictionaryRepresentation] mutableCopy] autorelease];
	
	//[retval setObject:[self absolutePath] forKey:@"aliasURL"];
	//[retval setObject:[self aliasBookmarkData] forKey:@"aliasBookmarkData"];
	
	return [[retval copy] autorelease];
}

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	_alias = [[coder decodeObjectForKey:@"alias"] retain];
	
	return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
	[coder encodeObject:_alias forKey:@"alias"];
	[super encodeWithCoder:coder];
}

- (id)copyWithZone:(NSZone *)zone {
	WCAlias *copy = [super copyWithZone:zone];
	
	copy->_alias = [_alias retain];
	copy->_cachedUTI = [_cachedUTI retain];
	
	return copy;
}

@dynamic UTI;
- (NSString *)UTI {
	if (!_cachedUTI)
		_cachedUTI = [[[NSWorkspace sharedWorkspace] typeOfFile:[self absolutePath] error:NULL] copy];
	return _cachedUTI;
}
@dynamic absolutePath;
- (NSString *)absolutePath {
	return [[self fileURL] path];
}
@dynamic absolutePathForDisplay;
- (NSString *)absolutePathForDisplay {
	return [[self absolutePath] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
}
@dynamic fileURL;
- (NSURL *)fileURL {
	return [_alias URL];
}
- (void)setFileURL:(NSURL *)fileURL {
	[_alias setURL:fileURL];
}

+ (id)aliasWithURL:(NSURL *)url; {
	return [[[[self class] alloc] initWithURL:url] autorelease];
}
- (id)initWithURL:(NSURL *)url; {
	if (!(self = [super initWithName:nil]))
		return nil;
	
	_alias = [[NDAlias alloc] initWithURL:url];
	
	return self;
}
@end
