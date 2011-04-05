//
//  WCAddFilesToProjectViewController.m
//  WabbitStudio
//
//  Created by William Towe on 3/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCAddFilesToProjectViewController.h"


@implementation WCAddFilesToProjectViewController

- (id)init {
	return [self initWithNibName:@"WCAddFilesToProjectView" bundle:nil];
}

- (void)dealloc {
	NSLog(@"dealloc called in %@ class",[self className]);
    [super dealloc];
}

@end
