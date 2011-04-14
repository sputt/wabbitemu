//
//  WCNewProjectWindowController.h
//  WabbitStudio
//
//  Created by William Towe on 3/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSingletonWindowController.h"

@class BWAnchoredButtonBar;

@interface WCNewProjectWindowController : WCSingletonWindowController <NSSplitViewDelegate,NSTableViewDelegate> {
@private
	IBOutlet NSImageView *_splitterHandleImageView;
	IBOutlet BWAnchoredButtonBar *_buttonBar;
	IBOutlet NSSplitView *_splitView;
	IBOutlet NSTableView *_tableView;
	IBOutlet NSArrayController *_templatesArrayController;
	
	NSMutableArray *_templates;
}

@property (readonly,nonatomic) NSArray *templates;
@property (readonly,nonatomic) BOOL canCreateProject;

- (IBAction)createProject:(id)sender;
- (IBAction)newProjectFromFolder:(id)sender;
- (IBAction)cancel:(id)sender;
- (IBAction)reloadTemplates:(id)sender;
@end
