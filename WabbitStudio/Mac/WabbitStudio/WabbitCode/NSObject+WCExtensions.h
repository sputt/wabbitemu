//
//  NSObject+WCExtensions.h
//  WabbitStudio
//
//  Created by William Towe on 3/26/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>


extern NSString *const kNSObjectSelectorKey;
extern NSString *const kNSObjectNotificationNameKey;
extern NSString *const kNSObjectNotificationObjectKey;

@interface NSObject (NSObject_WCExtensions)
- (void)setupUserDefaultsObserving;
- (void)cleanupUserDefaultsObserving;
- (NSArray *)userDefaultsKeys;

- (void)setupNotificationObserving;
- (void)setupNotificationObservingWithDictionaries:(NSArray *)notificationDictionaries;
- (void)cleanupNotificationObserving;
- (NSArray *)notificationDictionaries;
@end
