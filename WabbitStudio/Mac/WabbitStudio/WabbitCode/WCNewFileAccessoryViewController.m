//
//  WCNewFileAccessoryViewController.m
//  WabbitStudio
//
//  Created by William Towe on 4/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCNewFileAccessoryViewController.h"
#import "WCDefines.h"
#import "WCNewFileWindowController.h"
#import "WCFileTemplate.h"
#import "WCProject.h"
#import "NSTreeNode+WCExtensions.h"
#import "WCFile.h"
#import "WCProjectFilesOutlineViewController.h"
#import "NSTreeController+WCExtensions.h"
#import "NSURL+WCExtensions.h"
#import "NSResponder+WCExtensions.h"
#import "WCGeneralPerformer.h"


@interface WCNewFileAccessoryViewController (Private)
- (id)initWithNewFileWindowController:(WCNewFileWindowController *)controller;
@end

@implementation WCNewFileAccessoryViewController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	_controller = nil;
    [super dealloc];
}

- (void)loadView {
	[super loadView];
	
	[_groupPopUpButton removeAllItems];
	
	NSMenu *menu = [[[NSMenu alloc] initWithTitle:@""] autorelease];
	
	NSTreeNode *root = [[[(NSTreeController *)[[[[_controller project] projectFilesOutlineViewController] outlineView] dataSource] arrangedObjects] childNodes] lastObject]; 
	
	for (NSTreeNode *node in [root descendantGroupNodesInclusive]) {
		NSMenuItem *item = [menu addItemWithTitle:[[node representedObject] name] action:NULL keyEquivalent:@""];
		
		[item setImage:[[node representedObject] icon]];
		[item setIndentationLevel:[[node indexPath] length] - 1];
		[item setRepresentedObject:[node representedObject]];
	}
	
	[_groupPopUpButton setMenu:menu];
	[_groupPopUpButton selectItemAtIndex:0];
}

- (void)panel:(id)sender didChangeToDirectoryURL:(NSURL *)url {
	for (NSMenuItem *item in [[_groupPopUpButton menu] itemArray]) {
		if ([url isEqualToFileURL:[[item representedObject] URL]]) {
			[_groupPopUpButton selectItem:item];
			return;
		}
	}
}

@dynamic selectedGroup;
- (WCFile *)selectedGroup {
	return [[_groupPopUpButton selectedItem] representedObject];
}

+ (void)presentSheetForNewFileWindowController:(WCNewFileWindowController *)controller; {
	WCNewFileAccessoryViewController *mController = [[[self class] alloc] initWithNewFileWindowController:controller];
	NSSavePanel *panel = [NSSavePanel savePanel];
	WCFileTemplate *fTemplate = [controller selectedTemplate];
	
	//[panel setPrompt:NS_LOCALIZED_STRING_CREATE];
	[panel setRequiredFileType:[[NSWorkspace sharedWorkspace] typeOfFile:[[[fTemplate templateURL] URLByAppendingPathComponent:[fTemplate templateFileName]] path] error:NULL]];
	[panel setAccessoryView:[mController view]];
	[panel setDirectoryURL:[[[controller project] fileURL] URLByDeletingLastPathComponent]];
	[panel setDelegate:mController];
	
	[panel beginSheetModalForWindow:[controller window] completionHandler:^(NSInteger result) {
		[mController autorelease];
		[panel close];
		if (result != NSFileHandlingPanelOKButton)
			return;
		
		[fTemplate setProjectName:[[controller project] displayName]];
		
		NSError *error = nil;
		WCFile *file = [[WCGeneralPerformer sharedPerformer] createFileAtURL:[panel URL] withTemplate:fTemplate error:&error];
		
		if (!file) {
			if (error)
				[NSApp presentError:error];
			
			[[controller window] orderOut:nil];
			[NSApp endSheet:[controller window] returnCode:NSCancelButton];
			return;
		}
		
		WCFile *group = [mController selectedGroup];
		
		[[group mutableChildNodes] insertObject:file atIndex:0];
		[(NSTreeController *)[[[[controller project] projectFilesOutlineViewController] outlineView] dataSource] setSelectedRepresentedObject:file];
		
		[[controller window] orderOut:nil];
		[NSApp endSheet:[controller window] returnCode:NSOKButton];
	}];
}
								
- (id)initWithNewFileWindowController:(WCNewFileWindowController *)controller; {
	if (!(self = [super initWithNibName:@"WCNewFileAccessoryView" bundle:nil]))
		return nil;
	
	_controller = controller;
	
	return self;
}
@end
