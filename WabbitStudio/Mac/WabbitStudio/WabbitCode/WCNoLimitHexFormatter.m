//
//  WCEightDigitHexFormatter.m
//  WabbitStudio
//
//  Created by William Towe on 5/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCNoLimitHexFormatter.h"


@implementation WCNoLimitHexFormatter

- (NSString *)stringForObjectValue:(id)object {
	if ([object isKindOfClass:[NSNumber class]])
		return [NSString stringWithFormat:@"%x",[object unsignedIntValue]];
	return [NSString stringWithFormat:@"%x",[object integerValue]];
}

@end
