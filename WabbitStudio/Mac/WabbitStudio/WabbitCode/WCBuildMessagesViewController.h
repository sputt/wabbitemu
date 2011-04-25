//
//  WCBuildMessagesViewController.h
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectNavigationViewController.h"


@interface WCBuildMessagesViewController : WCProjectNavigationViewController <NSOutlineViewDelegate> {
@private
    IBOutlet NSOutlineView *_outlineView;
}
@property (readonly,nonatomic) NSOutlineView *outlineView;

- (IBAction)buildMessagesOutlineViewSingleClick:(id)sender;
- (IBAction)buildMessagesOutlineViewDoubleClick:(id)sender;
@end
