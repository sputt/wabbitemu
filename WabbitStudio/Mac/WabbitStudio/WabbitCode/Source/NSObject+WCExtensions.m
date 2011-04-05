//
//  NSObject+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 3/26/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSObject+WCExtensions.h"

NSString* const kNSObjectSelectorKey = @"kNSObjectSelectorKey";
NSString* const kNSObjectNotificationNameKey = @"kNSObjectNotificationNameKey";
NSString* const kNSObjectNotificationObjectKey = @"kNSObjectNotificationObjectKey";

@implementation NSObject (NSObject_WCExtensions)
- (void)setupUserDefaultsObserving; {
	for (NSString *key in [self userDefaultsKeys])
		[[NSUserDefaultsController sharedUserDefaultsController] addObserver:self forKeyPath:[@"values." stringByAppendingString:key] options:NSKeyValueObservingOptionNew context:(void *)key];
}
- (void)cleanupUserDefaultsObserving; {
	for (NSString *key in [self userDefaultsKeys])
		[[NSUserDefaultsController sharedUserDefaultsController] removeObserver:self forKeyPath:[@"values." stringByAppendingString:key]];
}
// classes override this and return the user defaults keys they are interested in observing
- (NSArray *)userDefaultsKeys; {
	return nil;
}

- (void)setupNotificationObserving; {
	[self setupNotificationObservingWithDictionaries:[self notificationDictionaries]];
}
- (void)setupNotificationObservingWithDictionaries:(NSArray *)notificationDictionaries; {
	for (NSDictionary *dict in notificationDictionaries)
		[[NSNotificationCenter defaultCenter] addObserver:self selector:NSSelectorFromString([dict objectForKey:kNSObjectSelectorKey]) name:[dict objectForKey:kNSObjectNotificationNameKey] object:[dict objectForKey:kNSObjectNotificationObjectKey]];
}
- (void)cleanupNotificationObserving; {
	
}
// classes override this and return the dictionaries containing the information to setup their notifications
- (NSArray *)notificationDictionaries; {
	return nil;
}
@end
