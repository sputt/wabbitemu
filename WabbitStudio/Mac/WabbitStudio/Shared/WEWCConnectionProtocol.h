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

static NSString *const kWabbitCodeConnectionName = @"org.revsoft.wabbitcode.connection";
static NSString *const kWabbitEmuConnectionName = @"org.revsoft.wabbitemu.connection";

@protocol WCProjectToWECalculatorConnection <NSObject>
- (oneway void)connectToWabbitCode;
@end

@protocol WECalculatorToWCProjectConnection <NSObject>
- (oneway void)connectToWabbitEmu;
@end
