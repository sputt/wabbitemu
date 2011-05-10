//
//  WCUnsavedFilesWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 4/10/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCUnsavedFilesWindowController.h"
#import "WCProject.h"
#import "WCFile.h"
#import "NSResponder+WCExtensions.h"
#import "WCDocumentController.h"

@interface WCUnsavedFilesWindowController ()
- (id)_initWithProject:(WCProject *)project;
@end

@implementation WCUnsavedFilesWindowController

- (NSString *)windowNibName {
	return @"WCUnsavedFilesWindow";
}

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	_project = nil;
	[_unsavedFiles release];
    [super dealloc];
}

+ (NSUInteger)runModalForProject:(WCProject *)project; {
	WCUnsavedFilesWindowController *controller = [[[WCUnsavedFilesWindowController alloc] _initWithProject:project] autorelease];
	
	if (![[controller unsavedFiles] count])
		return NSOKButton;
	
	NSUInteger result = [NSApp runModalForWindow:[controller window]];
	
	return result;
}

@synthesize unsavedFiles=_unsavedFiles;
@synthesize project=_project;

- (IBAction)saveSelected:(id)sender; {
	for (WCFile *file in [(NSArrayController *)[_tableView dataSource] selectedObjects])
		[file saveFile:NULL];
	
	[self approveModalWindowAction:nil];
}

- (id)_initWithProject:(WCProject *)project; {
	if (!(self = [super initWithWindowNibName:[self windowNibName]]))
		return nil;
	
	if (project)
		_unsavedFiles = [[project unsavedTextFiles] retain];
	else {
		NSMutableArray *files = [NSMutableArray array];
		for (WCProject *p in [[WCDocumentController sharedDocumentController] projects])
			[files addObjectsFromArray:[p unsavedTextFiles]];
		_unsavedFiles = [files copy];
	}
	
	_project = project;
	
	return self;
}
@end
