//
//  WCFilesTreeController.h
//  WabbitStudio
//
//  Created by William Towe on 3/21/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSTreeController.h>


@class WCProject;

@interface WCFilesTreeController : NSTreeController <NSOutlineViewDataSource> {
@private
    IBOutlet WCProject *_project;
}

@end
