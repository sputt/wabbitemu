//
//  WEConnectionManager.h
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSingleton.h"
#import "WEWCConnectionProtocol.h"


@class WECalculator;

extern NSString *const kWEConnectionManagerDidConnectNotification;
extern NSString *const kWEConnectionManagerDidDisconnectNotification;

@interface WEConnectionManager : WCSingleton <WCProjectToWECalculatorConnection,NSConnectionDelegate> {
@private
	NSConnection *_connection;
    id <WECalculatorToWCProjectConnection> _connectionProxy;
	NSMutableDictionary *_projectIdentifiersToCalculators;
}
@property (readonly,nonatomic) id <WECalculatorToWCProjectConnection> connectionProxy;
@property (readonly,nonatomic) BOOL isConnectedToWabbitCode;

+ (WEConnectionManager *)sharedConnectionManager;

@end
