//
//  WCBuildDefine.m
//  WabbitStudio
//
//  Created by William Towe on 3/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBuildDefine.h"


@interface WCBuildDefine (Private)
- (NSString *)_legalNameOrValueFromString:(NSString *)string;
@end

@implementation WCBuildDefine

- (void)dealloc {
	[_defineValue release];
	[super dealloc];
}

- (NSString *)description {
	if ([self defineValue])
		return [NSString stringWithFormat:@"%@=%@", [self name], [self defineValue]];
	return [NSString stringWithFormat:@"%@", [self name]];
}

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	_defineValue = [[coder decodeObjectForKey:@"defineValue"] copy];
	
	return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
	[coder encodeObject:[self defineValue] forKey:@"defineValue"];
	[super encodeWithCoder:coder];
}

- (id)copyWithZone:(NSZone *)zone {
	return [[[self class] allocWithZone:zone] initWithName:[self name] value:[self defineValue]];
}

- (void)setName:(NSString *)value {
	if ([[self name] isEqualToString:value])
		return;
	
	NSString *legalValue = [self _legalNameOrValueFromString:value];
	if (!legalValue)
		return;
	
	[super setName:legalValue];
}

- (NSDictionary *)dictionaryRepresentation {
	// first grab super's dictionary
	NSMutableDictionary *retval = [[[super dictionaryRepresentation] mutableCopy] autorelease];
	
	// add our keys to it
	
	if ([self defineValue])
		[retval setObject:[self defineValue] forKey:@"defineValue"];
	
	// return a copy
	return [[retval copy] autorelease];
}

@dynamic defineValue;
- (NSString *)defineValue {
	return _defineValue;
}
- (void)setValue:(NSString *)value {
	if (_defineValue == value)
		return;
	
	NSString *legalValue = [self _legalNameOrValueFromString:value];
	if (!legalValue)
		return;
	
	[_defineValue release];
	_defineValue = [legalValue copy];
}

@dynamic processedDefine;
- (NSString *)processedDefine {
	if ([self defineValue])
		return [NSString stringWithFormat:@"-D%@=%@", [self name], [self defineValue]];
	return [NSString stringWithFormat:@"-D%@", [self name]];
}

+ (id)defineWithName:(NSString *)name; {
	return [self defineWithName:name value:nil];
}

+ (id)defineWithName:(NSString *)name value:(NSString *)value; {
	return [[[[self class] alloc] initWithName:name value:value] autorelease];
}
- (id)initWithName:(NSString *)name value:(NSString *)value; {
	if (!(self = [super initWithName:name]))
		return nil;
	
	NSString *legalValue = [self _legalNameOrValueFromString:value];
	
	if (legalValue)
		_defineValue = [legalValue copy];
	
	return self;
}

- (NSString *)_legalNameOrValueFromString:(NSString *)string; {
	static NSCharacterSet *legalCharacters = nil;
	if (!legalCharacters) {
		NSMutableCharacterSet *letters = [[[NSCharacterSet letterCharacterSet] mutableCopy] autorelease];
		[letters formUnionWithCharacterSet:[NSCharacterSet characterSetWithCharactersInString:@"_"]];
		legalCharacters = [letters copy];
	}
	
	NSUInteger index, bIndex, length = [string length];
	unichar *buffer = (unichar *)calloc(length, sizeof(unichar));
	
	for (index = 0, bIndex = 0; index < length; index++) {
		unichar c = [string characterAtIndex:index];
		
		if ([legalCharacters characterIsMember:c])
			buffer[bIndex++] = c;
	}
	
	if (!bIndex)
		return nil;
	
	return [[[[NSString alloc] initWithCharactersNoCopy:buffer length:bIndex freeWhenDone:YES] autorelease] uppercaseString];
}

@end
