//
//  WCFileWindowController.h
//  WabbitStudio
//
//  Created by William Towe on 4/19/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>
#import "WCTabViewContextProtocol.h"

@class WCFile,WCProject,PSMTabBarControl,BWAnchoredButtonBar;

@interface WCFileWindowController : NSWindowController <WCTabViewContext,NSWindowDelegate> {
@private
	IBOutlet PSMTabBarControl *_tabBarControl;
	IBOutlet BWAnchoredButtonBar *_buttonBar;
	
    WCFile *_file;
}
@property (readonly,nonatomic) WCProject *project;
@property (readonly,nonatomic) WCFile *file;

+ (id)fileWindowControllerWithFile:(WCFile *)file;
- (id)initWithFile:(WCFile *)file;
@end
