//
//  WCDocumentController.m
//  WabbitStudio
//
//  Created by William Towe on 4/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCDocumentController.h"
#import "WCFile.h"
#import "WCProject.h"
#import "NSURL+WCExtensions.h"


@implementation WCDocumentController

- (id)openDocumentWithContentsOfURL:(NSURL *)absoluteURL display:(BOOL)displayDocument error:(NSError **)outError {
	NSString *type = [self typeForContentsOfURL:absoluteURL error:outError];
	if (![type isEqualToString:kWCFileIncludeUTI] && ![type isEqualToString:kWCFileAssemblyUTI])
		return [super openDocumentWithContentsOfURL:absoluteURL display:displayDocument error:outError];
	
	WCProject *mProject = nil;
	WCFile *mFile = nil;
	
	for (WCProject *project in [self projects]) {
		for (WCFile *file in [project textFiles]) {
			if ([[file URL] isEqualToFileURL:absoluteURL]) {
				mProject = project;
				mFile = file;
				break;
			}
		}
		
		if (mProject)
			break;
	}
	
	if (!mProject || !mFile)
		return [super openDocumentWithContentsOfURL:absoluteURL display:displayDocument error:outError];
	
	[[[[mProject windowControllers] lastObject] window] makeKeyAndOrderFront:nil];
	[mProject addFileViewControllerForFile:mFile inTabViewContext:[mProject currentTabViewContext]];
	return mProject;
}

@dynamic currentProject;
- (WCProject *)currentProject {
	id document = [self currentDocument];
	if ([document isKindOfClass:[WCProject class]])
		return document;
	return nil;
}
@dynamic projects;
- (NSArray *)projects {
	NSMutableArray *retval = [NSMutableArray array];
	
	for (id document in [self documents]) {
		if ([document isKindOfClass:[WCProject class]])
			[retval addObject:document];
	}
	
	return [[retval copy] autorelease];
}
@end
