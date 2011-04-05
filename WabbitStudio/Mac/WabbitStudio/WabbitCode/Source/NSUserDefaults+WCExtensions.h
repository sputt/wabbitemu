//
//  NSUserDefaults+WCExtensions.h
//  WabbitStudio
//
//  Created by William Towe on 3/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSUserDefaults.h>


@interface NSUserDefaults (NSUserDefaults_WCExtensions)
- (NSUInteger)unsignedIntegerForKey:(NSString *)key;
- (NSColor *)colorForKey:(NSString *)key;
- (NSFont *)fontForKey:(NSString *)key;
@end
