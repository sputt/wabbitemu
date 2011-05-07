//
//  WCDefines.m
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCDefines.h"

const NSRange WCEmptyRange = (NSRange){0,0};
const NSRange WCNotFoundRange = (NSRange){NSNotFound,0};

const NSSize WCSmallSize = (NSSize){16.0,16.0};
const NSSize WCMiniSize = (NSSize){14.0,14.0};
const NSSize WCMediumSize = (NSSize){32.0,32.0};

NSString *const kWECalculatorProgramUTI = @"org.revsoft.wabbitemu.program";
NSString *const kWECalculatorApplicationUTI = @"org.revsoft.wabbitemu.application";
NSString *const kWECalculatorSavestateUTI = @"org.revsoft.wabbitemu.savestate";
NSString *const kWECalculatorRomUTI = @"org.revsoft.wabbitemu.rom";

@implementation WCDefines

@end
