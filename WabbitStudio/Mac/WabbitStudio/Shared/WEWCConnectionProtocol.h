//
//  WEWCConnectionProtocol.h
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>


typedef struct _WECalculatorState {
	u_int16_t AF, BC, DE, HL;
	u_int16_t AFP, BCP, DEP, HLP;
	u_int16_t IX, IY;
	u_int16_t PC, SP;
	
} WECalculatorState;

enum {
	WEWCConnectionStatusNone = 0,
	WEWCConnectionStatusError,
	WEWCConnectionStatusAvailable,
	WEWCConnectionStatusConnected
};
typedef NSUInteger WEWCConnectionStatus;

static NSString *const kWabbitCodeConnectionName = @"org.revsoft.wabbitcode.connection";
static NSString *const kWabbitEmuConnectionName = @"org.revsoft.wabbitemu.connection";

@class WECalculator;

@protocol WCProjectToWECalculatorConnection <NSObject>
@required
- (oneway void)connectToWabbitCode;
- (oneway void)transferFile:(in bycopy NSString *)filePath fromProjectWithIdentifier:(in bycopy NSString *)projectIdentifier;
- (oneway void)projectName:(in bycopy NSString *)projectName forProjectWithIdentifier:(in bycopy NSString *)projectIdentifier;
- (oneway void)unpairCalculatorWithIdentifier:(in bycopy NSString *)projectIdentifier;
@end

@protocol WECalculatorToWCProjectConnection <NSObject>
@required
- (oneway void)connectToWabbitEmu;
- (oneway void)requestProjectNameForProjectWithIdentifier:(in bycopy NSString *)projectIdentifier;
- (oneway void)removeProjectWithIdentifier:(in bycopy NSString *)projectIdentifier;
@end
