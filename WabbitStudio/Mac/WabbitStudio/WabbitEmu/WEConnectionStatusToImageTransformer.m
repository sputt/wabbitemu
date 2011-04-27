//
//  WEConnectionStatusToImageTransformer.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEConnectionStatusToImageTransformer.h"
#import "WEWCConnectionProtocol.h"


@implementation WEConnectionStatusToImageTransformer

+ (Class)transformedValueClass {
	return [NSImage class];
}

+ (BOOL)allowsReverseTransformation {
	return NO;
}

- (id)transformedValue:(NSNumber *)value {
	if (value == nil)
		return [NSImage imageNamed:NSImageNameStatusNone];
	
	switch ([value unsignedIntegerValue]) {
		case WEWCConnectionStatusNone:
			return [NSImage imageNamed:NSImageNameStatusNone];
		case WEWCConnectionStatusError:
			return [NSImage imageNamed:NSImageNameStatusUnavailable];
		case WEWCConnectionStatusAvailable:
			return [NSImage imageNamed:NSImageNameStatusPartiallyAvailable];
		case WEWCConnectionStatusConnected:
			return [NSImage imageNamed:NSImageNameStatusAvailable];
		default:
			return nil;
	}
}

@end
