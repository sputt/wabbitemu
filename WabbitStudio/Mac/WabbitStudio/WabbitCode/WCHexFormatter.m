//
//  WCHexFormatter.m
//  WabbitStudio
//
//  Created by William Towe on 4/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCHexFormatter.h"
#import "NSString+WCExtensions.h"


@implementation WCHexFormatter

- (NSString *)stringForObjectValue:(id)object {
	if ([object isKindOfClass:[NSNumber class]])
		return [NSString stringWithFormat:@"$%04X",[object unsignedShortValue]];
	return [NSString stringWithFormat:@"$%04X",[object integerValue]];
}

- (BOOL)getObjectValue:(id *)object forString:(NSString *)string errorDescription:(NSString **)error {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
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
