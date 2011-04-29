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

@class WCProject;

@interface WCConnectionManager : WCSingleton <WECalculatorToWCProjectConnection> {
@private
	NSConnection *_connection;
    id <WCProjectToWECalculatorConnection> _connectionProxy;
	NSMutableDictionary *_projectIdentifiersToProjects;
}

@property (readonly,nonatomic) id <WCProjectToWECalculatorConnection> connectionProxy;
@property (readonly,nonatomic) BOOL isConnectedToWabbitEmu;

+ (WCConnectionManager *)sharedConnectionManager;

- (void)addProject:(WCProject *)project;
@end
