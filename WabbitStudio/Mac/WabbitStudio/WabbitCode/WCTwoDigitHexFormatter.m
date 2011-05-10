//
//  WCTwoDigitHexFormatter.m
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTwoDigitHexFormatter.h"


@implementation WCTwoDigitHexFormatter

- (NSString *)stringForObjectValue:(id)object {	
	if ([object isKindOfClass:[NSNumber class]])
		return [NSString stringWithFormat:@"%02x",[object unsignedShortValue]];
	return [NSString stringWithFormat:@"%02x",[object integerValue]];
}
@end
