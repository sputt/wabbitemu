//
//  WCProjectWindow.m
//  WabbitStudio
//
//  Created by William Towe on 4/10/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectWindow.h"
#import "WCProject.h"
#import "WCFile.h"

#import <PSMTabBarControl/PSMTabBarControl.h>


@implementation WCProjectWindow

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[super dealloc];
}

- (IBAction)performClose:(id)sender {
	id document = [[self windowController] document];
	
	if (![document isKindOfClass:[WCProject class]]) {
		[super performClose:sender];
		return;
	}
	
	if (![[[document tabBarControl] tabView] numberOfTabViewItems]) {
		[super performClose:sender];
		return;
	}
	
	if ([document tabView:[[document tabBarControl] tabView] shouldCloseTabViewItem:[[[document tabBarControl] tabView] selectedTabViewItem]])
		[[[document tabBarControl] tabView] removeTabViewItem:[[[document tabBarControl] tabView] selectedTabViewItem]];
}

@end
