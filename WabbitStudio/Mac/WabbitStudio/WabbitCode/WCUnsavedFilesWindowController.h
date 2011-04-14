//
//  WCUnsavedFilesWindowController.h
//  WabbitStudio
//
//  Created by William Towe on 4/10/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>
#import <AppKit/NSTableView.h>

@class WCProject;

@interface WCUnsavedFilesWindowController : NSWindowController {
@private
	IBOutlet NSTableView *_tableView;
	
	__weak WCProject *_project;
    NSArray *_unsavedFiles;
}
@property (retain,nonatomic) NSArray *unsavedFiles;
@property (assign,nonatomic) WCProject *project;

+ (NSUInteger)runModalForProject:(WCProject *)project;

- (IBAction)saveSelected:(id)sender;
@end
