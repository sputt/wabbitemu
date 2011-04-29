//
//  WCConnectionStatusToStringTransformer.m
//  WabbitStudio
//
//  Created by William Towe on 4/29/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCConnectionStatusToStringTransformer.h"
#import "WEWCConnectionProtocol.h"


@implementation WCConnectionStatusToStringTransformer

+ (BOOL)allowsReverseTransformation {
	return NO;
}
+ (Class)transformedValueClass {
	return [NSString class];
}
- (id)transformedValue:(NSNumber *)value {
	if (value == nil)
		return NSLocalizedString(@"WabbitEmu is not available",@"WabbitEmu is not available");
	
	switch ([value unsignedIntegerValue]) {
		case WEWCConnectionStatusNone:
			return NSLocalizedString(@"WabbitEmu is not available",@"WabbitEmu is not available");
		case WEWCConnectionStatusError:
			return NSLocalizedString(@"Waiting for WabbitEmu\u2026 ",@"Waiting for WabbitEmu with ellipsis");
		case WEWCConnectionStatusAvailable:
			return NSLocalizedString(@"WabbitEmu is available",@"WabbitEmu is available");
		case WEWCConnectionStatusConnected:
			return NSLocalizedString(@"Connected to WabbitEmu",@"Connected to WabbitEmu");
		default:
			return nil;
	}
}

@end
