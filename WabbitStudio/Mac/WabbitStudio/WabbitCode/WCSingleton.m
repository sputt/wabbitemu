//
//  WCSingleton.m
//  WabbitStudio
//
//  Created by William Towe on 3/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSingleton.h"


static NSMutableDictionary *_dict = nil;

@interface WCSingleton (Private)
+ (id)_sharedInstanceCreateIfNecessary;
+ (id)_sharedInstanceDontCreate;
@end

@implementation WCSingleton

+ (void)initialize {
	if ([WCSingleton class] == self)
		_dict = [[NSMutableDictionary alloc] init];
}

+ (id)sharedController; {
	return [self _sharedInstanceCreateIfNecessary];
}
+ (id)sharedPerformer; {
	return [self _sharedInstanceCreateIfNecessary];
}

+ (id)_sharedInstanceCreateIfNecessary; {
	id retval = [_dict objectForKey:NSStringFromClass([self class])];
	
	// the instance hasn't been created yet, create it and put it in our dictionary
	if (!retval) {
		retval = [[[[self class] alloc] init] autorelease];
		
		[_dict setObject:retval forKey:NSStringFromClass([self class])];
	}
	
	return retval;
}

+ (id)_sharedInstanceDontCreate; {
	return [_dict objectForKey:NSStringFromClass([self class])];
}
@end
