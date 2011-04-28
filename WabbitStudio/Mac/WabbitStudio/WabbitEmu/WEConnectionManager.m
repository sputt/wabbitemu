//
//  WEConnectionManager.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEConnectionManager.h"


NSString *const kWEConnectionManagerDidConnectNotification = @"kWEConnectionManagerDidConnectNotification";
NSString *const kWEConnectionManagerDidDisconnectNotification = @"kWEConnectionManagerDidDisconnectNotification";

@implementation WEConnectionManager

- (id)init {
	if (!(self = [super init]))
		return nil;
	
	_connection = [[NSConnection alloc] init];
	[_connection setRootObject:self];
	if (![_connection registerName:kWabbitEmuConnectionName]) {
		NSLog(@"unable to register name %@ in %@, bailing",kWabbitEmuConnectionName,[self className]);
		[self release];
		return nil;
	}
	
	[self connectToWabbitCode];
    
    return self;
}

+ (WEConnectionManager *)sharedConnectionManager {
	return [super sharedController];
}

- (void)connectToWabbitCode {
	if (_connectionProxy == nil) {
		NSDistantObject *proxy = [NSConnection rootProxyForConnectionWithRegisteredName:kWabbitCodeConnectionName host:nil];
		if (!proxy)
			return;
		
		_connectionProxy = [proxy retain];
		[(id)_connectionProxy setProtocolForProxy:@protocol(WECalculatorToWCProjectConnection)];
		
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_connectionDidDieNotification:) name:NSConnectionDidDieNotification object:[proxy connectionForProxy]];
		
		[_connectionProxy connectToWabbitEmu];
#ifdef DEBUG
		NSLog(@"connected from WabbitEmu to WabbitCode");
#endif
		[[NSNotificationCenter defaultCenter] postNotificationName:kWEConnectionManagerDidConnectNotification object:self];
	}
}

- (void)_connectionDidDieNotification:(NSNotification *)note {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_connectionProxy release];
	_connectionProxy = nil;
	
#ifdef DEBUG
	NSLog(@"connection from WabbitCode to WabbitEmu died");
#endif
	[[NSNotificationCenter defaultCenter] postNotificationName:kWEConnectionManagerDidDisconnectNotification object:self];
}

@end
