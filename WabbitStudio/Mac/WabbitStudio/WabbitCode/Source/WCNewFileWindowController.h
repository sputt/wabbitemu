//
//  WCNewFileWindowController.h
//  WabbitStudio
//
//  Created by William Towe on 4/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>

@class BWAnchoredButtonBar,WCProject,WCFileTemplate;

@interface WCNewFileWindowController : NSWindowController <NSSplitViewDelegate,NSTableViewDelegate> {
@private
    IBOutlet NSImageView *_splitterHandleImageView;
	IBOutlet BWAnchoredButtonBar *_buttonBar;
	IBOutlet NSSplitView *_splitView;
	IBOutlet NSTableView *_tableView;
	IBOutlet NSArrayController *_templatesArrayController;
	
	NSMutableArray *_templates;
	WCProject *_project;
}
@property (readonly,nonatomic) WCProject *project;
@property (readonly,nonatomic) NSArray *templates;
@property (readonly,nonatomic) WCFileTemplate *selectedTemplate;

- (IBAction)create:(id)sender;

+ (void)presentNewFileSheetForProject:(WCProject *)project;

@end
