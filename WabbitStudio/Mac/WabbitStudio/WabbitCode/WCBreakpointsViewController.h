//
//  WCBreakpointsViewController.h
//  WabbitStudio
//
//  Created by William Towe on 4/19/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectNavigationViewController.h"

@class WCBreakpoint;

@interface WCBreakpointsViewController : WCProjectNavigationViewController <NSOutlineViewDelegate,NSOutlineViewDataSource> {
@private
    IBOutlet NSOutlineView *_outlineView;
	
	WCBreakpoint *_breakpoint;
}
@property (readonly,nonatomic) NSOutlineView *outlineView;
@end
