//
//  WCConnectionManager.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCConnectionManager.h"


NSString *const kWCConnectionManagerDidConnectNotification = @"kWCConnectionManagerDidConnectNotification";
NSString *const kWCConnectionManagerDidDisconnectNotification = @"kWCConnectionManagerDidDisconnectNotification";

@implementation WCConnectionManager

- (id)init {
	if (!(self = [super init]))
		return nil;
	
	_connection = [[NSConnection alloc] init];
	[_connection setRootObject:self];
	if (![_connection registerName:kWabbitCodeConnectionName]) {
		NSLog(@"unable to register name %@ in %@, bailing",kWabbitCodeConnectionName,[self className]);
		[self release];
		return nil;
	}
	
	[self connectToWabbitEmu];
    
    return self;
}

+ (WCConnectionManager *)sharedConnectionManager {
	return [super sharedController];
}

- (void)connectToWabbitEmu {
	if (_connectionProxy == nil) {
		NSDistantObject *proxy = [NSConnection rootProxyForConnectionWithRegisteredName:kWabbitEmuConnectionName host:nil];
		if (!proxy)
			return;
		
		_connectionProxy = [proxy retain];
		[(id)_connectionProxy setProtocolForProxy:@protocol(WCProjectToWECalculatorConnection)];
		
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_connectionDidDieNotification:) name:NSConnectionDidDieNotification object:[proxy connectionForProxy]];
		
		[_connectionProxy connectToWabbitCode];
		
#ifdef DEBUG
		NSLog(@"connected from WabbitCode to WabbitEmu");
#endif
		[[NSNotificationCenter defaultCenter] postNotificationName:kWCConnectionManagerDidConnectNotification object:self];
	}
}

- (void)_connectionDidDieNotification:(NSNotification *)note {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_connectionProxy release];
	_connectionProxy = nil;
	
#ifdef DEBUG
	NSLog(@"connection from WabbitEmu to WabbitCode died");
#endif
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCConnectionManagerDidDisconnectNotification object:self];
}
@end
