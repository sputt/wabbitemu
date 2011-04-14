//
//  NSString+WCExtensions.h
//  WabbitStudio
//
//  Created by William Towe on 4/2/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSString.h>

@class WCProjectTemplate,WCFileTemplate;

@interface NSString (NSString_WCExtensions)
+ (NSString *)UUIDString;

- (NSString *)stringByReplacingPlaceholdersWithTemplate:(WCProjectTemplate *)projectTemplate;
- (NSString *)stringByReplacingPlaceholdersWithFileTemplate:(WCFileTemplate *)fileTemplate;
@end
