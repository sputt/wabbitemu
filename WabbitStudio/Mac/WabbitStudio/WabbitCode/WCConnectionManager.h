//
//  WCConnectionManager.h
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSingleton.h"
#import "WEWCConnectionProtocol.h"


extern NSString *const kWCConnectionManagerDidConnectNotification;
extern NSString *const kWCConnectionManagerDidDisconnectNotification;

@interface WCConnectionManager : WCSingleton <WECalculatorToWCProjectConnection> {
@private
	NSConnection *_connection;
    id <WCProjectToWECalculatorConnection> _connectionProxy;
}

+ (WCConnectionManager *)sharedConnectionManager;
@end
