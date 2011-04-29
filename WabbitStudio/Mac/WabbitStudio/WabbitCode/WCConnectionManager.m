//
//  WCConnectionManager.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCConnectionManager.h"
#import "WCProject.h"
#import "WCDocumentController.h"


NSString *const kWCConnectionManagerDidConnectNotification = @"kWCConnectionManagerDidConnectNotification";
NSString *const kWCConnectionManagerDidDisconnectNotification = @"kWCConnectionManagerDidDisconnectNotification";

@interface WCConnectionManager ()
- (WCProject *)_projectForProjectIdentifier:(NSString *)projectIdentifier;
- (void)_addProject:(WCProject *)project;
@end

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
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_projectWillClose:) name:kWCProjectWillCloseNotification object:nil];
    
    return self;
}

@dynamic connectionProxy;
- (id<WCProjectToWECalculatorConnection>)connectionProxy {
	[self connectToWabbitEmu];
	return _connectionProxy;
}

@dynamic isConnectedToWabbitEmu;
- (BOOL)isConnectedToWabbitEmu {
	return ([self connectionProxy] != nil);
}

+ (WCConnectionManager *)sharedConnectionManager {
	return [self sharedController];
}

- (void)addProject:(WCProject *)project; {
	[self _addProject:project];
}

- (oneway void)connectToWabbitEmu {
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

- (oneway void)requestProjectNameForProjectWithIdentifier:(in bycopy NSString *)projectIdentifier; {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	
	WCProject *project = [self _projectForProjectIdentifier:projectIdentifier];
	
	if (project == nil)
		return;
	
	[project setConnectionStatus:WEWCConnectionStatusConnected];
	
	[[self connectionProxy] projectName:[[project displayName] stringByDeletingPathExtension] forProjectWithIdentifier:[project projectUUID]];
}

- (oneway void)removeProjectWithIdentifier:(in bycopy NSString *)projectIdentifier; {
	WCProject *project = [self _projectForProjectIdentifier:projectIdentifier];
	
	if (project == nil)
		return;
	
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	
	[_projectIdentifiersToProjects removeObjectForKey:projectIdentifier];
	
	if (![project isClosing])
		[project setConnectionStatus:WEWCConnectionStatusAvailable];
}

- (WCProject *)_projectForProjectIdentifier:(NSString *)projectIdentifier; {
	return [_projectIdentifiersToProjects objectForKey:projectIdentifier];
}

- (void)_addProject:(WCProject *)project; {
	if (_projectIdentifiersToProjects == nil)
		_projectIdentifiersToProjects = [[NSMutableDictionary alloc] init];
	[_projectIdentifiersToProjects setObject:project forKey:[project projectUUID]];
}

- (void)_projectWillClose:(NSNotification *)note {
	[self removeProjectWithIdentifier:[[note object] projectUUID]];
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
