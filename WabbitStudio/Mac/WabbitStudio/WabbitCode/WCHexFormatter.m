//
//  WCHexFormatter.m
//  WabbitStudio
//
//  Created by William Towe on 4/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCHexFormatter.h"
#import "NSString+WCExtensions.h"


@implementation NSString (WCHexFormatter_Extensions)

- (NSString *)stringByRemovingInvalidHexDigits; {
	if (!self || [self length] == 0)
		return nil;
	
	static NSCharacterSet *allowedCharacters = nil;
	if (!allowedCharacters)
		allowedCharacters = [[NSCharacterSet characterSetWithCharactersInString:@"0123456789abcdefABCDEF"] retain];
	NSUInteger trimLength = 0, length = [self length];
	unichar buffer[length];
	
	for (NSUInteger index = 0; index < length; index++) {
		if ([allowedCharacters characterIsMember:[self characterAtIndex:index]])
			buffer[trimLength++] = [self characterAtIndex:index];
	}
	
	if (trimLength == 0)
		return nil;
	return [[[NSString alloc] initWithCharacters:buffer length:trimLength] autorelease];
}

@end

@implementation WCHexFormatter

- (NSString *)stringForObjectValue:(id)object {
	if ([object isKindOfClass:[NSNumber class]])
		return [NSString stringWithFormat:@"%04X",[object unsignedShortValue]];
	return [NSString stringWithFormat:@"%04X",[object integerValue]];
}

- (BOOL)getObjectValue:(id *)object forString:(NSString *)string errorDescription:(NSString **)error {
	string = [string stringByRemovingInvalidHexDigits];
	
	if (!string || [string length] == 0) {
		*object = [NSNumber numberWithUnsignedShort:0];
		return YES;
	}
	
	NSInteger index = [string length];
	u_int16_t total = 0, exponent = 0, base = 16;
	
	while (index > 0) {
		u_int8_t value = HexValueForCharacter([string characterAtIndex:--index]);
		total += value * (u_int16_t)powf(base, exponent++);
	}
	
	*object = [NSNumber numberWithUnsignedShort:total];
	return YES;
}

@end
