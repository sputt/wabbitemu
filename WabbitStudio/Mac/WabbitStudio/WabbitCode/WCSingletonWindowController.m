//
//  WCSingletonWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 3/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSingletonWindowController.h"

static NSMutableDictionary *_dict = nil;

@interface WCSingletonWindowController (Private)
+ (id)_sharedInstanceCreateIfNecessary;
+ (id)_sharedInstanceDontCreate;
@end

@implementation WCSingletonWindowController

+ (void)initialize {
	if ([WCSingletonWindowController class] != self)
		return;
	
	_dict = [[NSMutableDictionary alloc] init];
}

- (id)init {
	return [self initWithWindowNibName:[self windowNibName]];
}

+ (id)sharedWindowController {
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

@dynamic windowTitle;

- (void)presentWindowController; {
	[NSApp runModalForWindow:[self window]];
}

+ (id)_sharedInstanceDontCreate; {
	return [_dict objectForKey:NSStringFromClass([self class])];
}

@end
