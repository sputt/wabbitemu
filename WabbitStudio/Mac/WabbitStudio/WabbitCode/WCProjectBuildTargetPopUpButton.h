//
//  WCProjectBuildTargetPopUpButton.h
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSPopUpButton.h>

@class WCProject;

@interface WCProjectBuildTargetPopUpButton : NSPopUpButton {
@private
    __weak WCProject *_project;
}
@property (readonly,nonatomic) WCProject *project;

- (id)initWithProject:(WCProject *)project;
@end
