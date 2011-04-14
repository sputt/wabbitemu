//
//  WCBuildTargetInfoSheetController.h
//  WabbitStudio
//
//  Created by William Towe on 3/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>


@class BWAnchoredButtonBar,WCProject;

@interface WCBuildTargetInfoSheetController : NSWindowController <NSControlTextEditingDelegate> {
@private
    IBOutlet BWAnchoredButtonBar *_buttonBar;
	IBOutlet NSPopUpButton *_inputFilePopUpButton;
	IBOutlet NSArrayController *_buildTargetsArrayController;
	IBOutlet NSTableView *_tableView;
	
	__weak WCProject *_project;
}
@property (readonly,nonatomic) WCProject *project;

+ (void)presentBuildTargetInfoSheetControllerForProject:(WCProject *)project;

+ (id)buildTargetInfoSheetControllerForProject:(WCProject *)project;
- (id)initWithProject:(WCProject *)project;

- (IBAction)addBuildTarget:(id)sender;
- (IBAction)removeBuildTarget:(id)sender;
- (IBAction)duplicateBuildTarget:(id)sender;
- (IBAction)makeActiveBuildTarget:(id)sender;
@end
