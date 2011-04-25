//
//  WCAddFilesToProjectViewController.m
//  WabbitStudio
//
//  Created by William Towe on 3/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCAddFilesToProjectViewController.h"
#import "WCPreferencesController.h"


@implementation WCAddFilesToProjectViewController

- (id)init {
	return [self initWithNibName:@"WCAddFilesToProjectView" bundle:nil];
}

- (void)dealloc {
	NSLog(@"dealloc called in %@ class",[self className]);
    [super dealloc];
}

- (IBAction)selectTextEncoding:(NSPopUpButton *)sender; {
	[[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithUnsignedInteger:(NSUInteger)[sender selectedTag]] forKey:kWCPreferencesFilesTextEncodingKey];
}
@end
