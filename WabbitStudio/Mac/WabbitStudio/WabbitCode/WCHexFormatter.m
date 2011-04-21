//
//  WCHexFormatter.m
//  WabbitStudio
//
//  Created by William Towe on 4/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCHexFormatter.h"


static inline u_int16_t valueForCharacter(unichar character) {
	switch (character) {
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'a':
		case 'A':
			return 10;
		case 'b':
		case 'B':
			return 11;
		case 'c':
		case 'C':
			return 12;
		case 'd':
		case 'D':
			return 13;
		case 'e':
		case 'E':
			return 14;
		case 'f':
		case 'F':
			return 15;
		default:
			return 0;
	}
}

static inline NSString* stringWithOnlyHexDigitsFromString(NSString *string) {
	if (!string || [string length] == 0)
		return nil;
	
	static NSCharacterSet *allowedCharacters = nil;
	if (!allowedCharacters)
		allowedCharacters = [[NSCharacterSet characterSetWithCharactersInString:@"0123456789abcdefABCDEF"] retain];
	NSUInteger newIndex = 0, length = [string length];
	unichar buffer[length];
	unichar new[length];
	
	[string getCharacters:buffer];
	
	for (NSUInteger index = 0; index < length; index++) {
		if ([allowedCharacters characterIsMember:buffer[index]])
			new[newIndex++] = buffer[index];
	}
	
	if (newIndex == 0)
		return nil;
	return [[[NSString alloc] initWithCharacters:new length:newIndex] autorelease];
}

@implementation WCHexFormatter

- (NSString *)stringForObjectValue:(id)object {
	if ([object isKindOfClass:[NSNumber class]])
		return [NSString stringWithFormat:@"$%04x",[object unsignedShortValue]];
	return [NSString stringWithFormat:@"$%04x",[object integerValue]];
}

- (BOOL)getObjectValue:(id *)object forString:(NSString *)string errorDescription:(NSString **)error {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	string = stringWithOnlyHexDigitsFromString(string);
	
	if (!string || [string length] == 0) {
		*object = [NSNumber numberWithUnsignedShort:0];
		return YES;
	}
	
	NSUInteger index = [string length];
	u_int16_t total = 0, exponent = 0, base = 16;
	
	while (index > 0) {
		u_int16_t value = valueForCharacter([string characterAtIndex:--index]);
		total += value * (u_int16_t)powf(base, exponent++);
	}
	
	*object = [NSNumber numberWithUnsignedShort:total];
	return YES;
}

@end
