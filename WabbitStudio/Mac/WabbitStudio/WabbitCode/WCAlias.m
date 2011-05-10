//
//  WCAlias.m
//  WabbitStudio
//
//  Created by William Towe on 3/29/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCAlias.h"
#import "NDAlias.h"
#import "NSFileManager+WCExtensions.h"

@interface WCAlias ()
@property (readonly,nonatomic) NDAlias *alias;

- (void)_privateSetup;
@end

@implementation WCAlias

- (void)dealloc {
	[_alias release];
	[_cachedUTI release];
	[_cachedURL release];
    [super dealloc];
}

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	_alias = [[coder decodeObjectForKey:@"alias"] retain];
	_needsToRecacheValues = YES;
	
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
	copy->_cachedURL = [_cachedURL retain];
	copy->_cachedIsDirectory = _cachedIsDirectory;
	
	return copy;
}

- (NSDictionary *)plistRepresentation {
	NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithDictionary:[super plistRepresentation]];
	
	[dict setObject:[NSKeyedArchiver archivedDataWithRootObject:[self alias]] forKey:@"alias"];
	
	return [[dict copy] autorelease];
}

@dynamic UTI;
- (NSString *)UTI {
	if (_needsToRecacheValues)
		[self _privateSetup];
		
	return _cachedUTI;
}
@dynamic absolutePath;
- (NSString *)absolutePath {
	return [[self URL] path];
}
@dynamic absolutePathForDisplay;
- (NSString *)absolutePathForDisplay {
	return [[self absolutePath] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
}
@dynamic URL;
- (NSURL *)URL {
	if (_needsToRecacheValues)
		[self _privateSetup];
		
	return _cachedURL;
}
- (void)setURL:(NSURL *)fileURL {
	if (!_alias)
		_alias = [[NDAlias alloc] initWithURL:fileURL];
	
	[[self alias] setURL:fileURL];
	_needsToRecacheValues = YES;
}
@synthesize alias=_alias;
@dynamic isDirectory;
- (BOOL)isDirectory {
	if (_needsToRecacheValues)
		[self _privateSetup];
	
	return [[NSFileManager defaultManager] directoryExistsAtURL:[self URL]];
}

+ (id)aliasWithURL:(NSURL *)url; {
	return [[[[self class] alloc] initWithURL:url] autorelease];
}
- (id)initWithURL:(NSURL *)url; {
	if (!(self = [super initWithName:nil]))
		return nil;
	
	_alias = [[NDAlias alloc] initWithURL:url];
	_needsToRecacheValues = YES;
	
	return self;
}

- (void)_privateSetup; {
	_needsToRecacheValues = NO;
	
	[_cachedURL release];
	_cachedURL = nil;
	[_cachedUTI release];
	_cachedUTI = nil;
	
	_cachedURL = [[[self alias] URL] retain];
	_cachedUTI = [[[NSWorkspace sharedWorkspace] typeOfFile:[self absolutePath] error:NULL] retain];
	//_cachedIsDirectory = [[NSFileManager defaultManager] directoryExistsAtURL:_cachedURL];
}
@end
