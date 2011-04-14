//
//  WCProjectBuildTargetPopUpButtonCell.m
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectBuildTargetPopUpButtonCell.h"


@implementation WCProjectBuildTargetPopUpButtonCell

- (void)selectItem:(NSMenuItem *)item {
	if ([item action] == @selector(editBuildTargets:))
		return;
	
	[super selectItem:item];
}

@end
