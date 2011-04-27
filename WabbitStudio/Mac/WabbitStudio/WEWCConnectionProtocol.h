//
//  WEWCConnectionProtocol.h
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>


enum {
	WEWCConnectionStatusNone = 0,
	WEWCConnectionStatusError,
	WEWCConnectionStatusAvailable,
	WEWCConnectionStatusConnected
};
typedef NSUInteger WEWCConnectionStatus;

@protocol WCProjectToWECalculatorConnection <NSObject>

@end

@protocol WECalculatorToWCProjectConnection <NSObject>

@end
