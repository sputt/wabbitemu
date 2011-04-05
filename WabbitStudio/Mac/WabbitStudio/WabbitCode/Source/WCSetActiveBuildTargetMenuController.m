//
//  WCSetActiveBuildTargetMenuController.m
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSetActiveBuildTargetMenuController.h"
#import "WCProject.h"
#import "WCBuildTarget.h"


@implementation WCSetActiveBuildTargetMenuController

- (NSInteger)numberOfItemsInMenu:(NSMenu*)menu; {
	id currentDocument = [[NSDocumentController sharedDocumentController] currentDocument];
	
	if ([currentDocument isKindOfClass:[WCProject class]])
		return [[currentDocument buildTargets] count];
	return 0;
}
- (BOOL)menu:(NSMenu*)menu updateItem:(NSMenuItem*)item atIndex:(NSInteger)index shouldCancel:(BOOL)shouldCancel; {
	WCProject *currentDocument = (WCProject *)[[NSDocumentController sharedDocumentController] currentDocument];
	WCBuildTarget *bt = [[currentDocument buildTargets] objectAtIndex:index];
	
	[item setTitle:[bt name]];
	[item setImage:[bt icon]];
	[item setTarget:self];
	[item setAction:@selector(_setActiveBuildTarget:)];
	[item setRepresentedObject:bt];
	
	return YES;
}

- (void)_setActiveBuildTarget:(NSMenuItem *)item {
	WCProject *currentDocument = (WCProject *)[[NSDocumentController sharedDocumentController] currentDocument];
	
	[currentDocument setActiveBuildTarget:[item representedObject]];
}
@end
