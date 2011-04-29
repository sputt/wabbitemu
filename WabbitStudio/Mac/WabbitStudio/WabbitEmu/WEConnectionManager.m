//
//  WEConnectionManager.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEConnectionManager.h"
#import "WECalculator.h"
#import "WETransferSheetController.h"


NSString *const kWEConnectionManagerDidConnectNotification = @"kWEConnectionManagerDidConnectNotification";
NSString *const kWEConnectionManagerDidDisconnectNotification = @"kWEConnectionManagerDidDisconnectNotification";

@interface WEConnectionManager ()
- (WECalculator *)_calculatorForProjectIdentifier:(NSString *)projectIdentifier;
- (void)_pairCalculator:(WECalculator *)calculator withProjectIdentifier:(NSString *)projectIdentifier;
- (void)_unpairCalculatorWithProjectIdentifier:(NSString *)projectIdentifier;
- (void)_unpairAllCalculators;
@end

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
    
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_calculatorWillClose:) name:kWECalculatorWillCloseNotification object:nil];
	
    return self;
}

+ (WEConnectionManager *)sharedConnectionManager {
	return [self sharedController];
}

@dynamic connectionProxy;
- (id<WECalculatorToWCProjectConnection>)connectionProxy {
	[self connectToWabbitCode];
	return _connectionProxy;
}

@dynamic isConnectedToWabbitCode;
- (BOOL)isConnectedToWabbitCode {
	return ([self connectionProxy] != nil);
}

- (oneway void)connectToWabbitCode {
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

- (oneway void)transferFile:(in bycopy NSString *)filePath fromProjectWithIdentifier:(in bycopy NSString *)projectIdentifier; {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	
	WECalculator *calculator = [self _calculatorForProjectIdentifier:projectIdentifier];
	
	if (calculator == nil) {
		// find an available calculator
		for (WECalculator *document in [[NSDocumentController sharedDocumentController] documents]) {
			if ([document projectIdentifier] == nil) {
				calculator = document;
				[self _pairCalculator:calculator withProjectIdentifier:projectIdentifier];
				break;
			}
		}
	}
	
	if (calculator == nil) {
#ifdef DEBUG
		NSLog(@"no calculators available for connection");
#endif
		return;
	}
	
	[WETransferSheetController transferFiles:[NSArray arrayWithObjects:filePath, nil] toCalculator:calculator runAfterTransfer:YES];
}

- (oneway void)projectName:(in bycopy NSString *)projectName forProjectWithIdentifier:(in bycopy NSString *)projectIdentifier; {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	
	WECalculator *calculator = [self _calculatorForProjectIdentifier:projectIdentifier];
	
	if (calculator == nil)
		return;
	
	[calculator setStatusString:projectName];
}

- (oneway void)unpairCalculatorWithIdentifier:(in bycopy NSString *)projectIdentifier; {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	
	[[self connectionProxy] removeProjectWithIdentifier:projectIdentifier];
	[self _unpairCalculatorWithProjectIdentifier:projectIdentifier];
}

- (WECalculator *)_calculatorForProjectIdentifier:(NSString *)projectIdentifier; {
	return [_projectIdentifiersToCalculators objectForKey:projectIdentifier];
}
- (void)_pairCalculator:(WECalculator *)calculator withProjectIdentifier:(NSString *)projectIdentifier; {
	if (_projectIdentifiersToCalculators == nil)
		_projectIdentifiersToCalculators = [[NSMutableDictionary alloc] init];
	
	[_projectIdentifiersToCalculators setObject:calculator forKey:projectIdentifier];
	[calculator setProjectIdentifier:projectIdentifier];
	[calculator setConnectionStatus:WEWCConnectionStatusConnected];
}
- (void)_unpairCalculatorWithProjectIdentifier:(NSString *)projectIdentifier; {
	WECalculator *calculator = [self _calculatorForProjectIdentifier:projectIdentifier];
	
	if (calculator == nil)
		return;
	
	[calculator setProjectIdentifier:nil];
	[calculator setConnectionStatus:WEWCConnectionStatusAvailable];
	[_projectIdentifiersToCalculators removeObjectForKey:projectIdentifier];
}
- (void)_unpairAllCalculators; {
	for (WECalculator *calculator in [_projectIdentifiersToCalculators objectEnumerator]) {
		[calculator setProjectIdentifier:nil];
		[calculator setConnectionStatus:WEWCConnectionStatusAvailable];
	}
	
	[_projectIdentifiersToCalculators removeAllObjects];
}

- (void)_connectionDidDieNotification:(NSNotification *)note {
	[self _unpairAllCalculators];
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_connectionProxy release];
	_connectionProxy = nil;
	
#ifdef DEBUG
	NSLog(@"connection from WabbitCode to WabbitEmu died");
#endif
	[[NSNotificationCenter defaultCenter] postNotificationName:kWEConnectionManagerDidDisconnectNotification object:self];
}

- (void)_calculatorWillClose:(NSNotification *)note {
	[self unpairCalculatorWithIdentifier:[[note object] projectIdentifier]];
}
@end
