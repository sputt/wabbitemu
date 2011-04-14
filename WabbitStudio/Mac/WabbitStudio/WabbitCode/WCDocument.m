//
//  WCDocument.m
//  WabbitStudio
//
//  Created by William Towe on 4/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCDocument.h"
#import "WCFileViewController.h"
#import "WCFile.h"
#import "WCTextStorage.h"
#import "WCProject.h"
#import "WCTextView.h"


@implementation WCDocument

+ (BOOL)canConcurrentlyReadDocumentsOfType:(NSString *)typeName {
	if ([typeName isEqualToString:kWCFileAssemblyUTI] ||
		[typeName isEqualToString:kWCFileIncludeUTI])
		return YES;
	return NO;
}

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)dealloc
{
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[_fileViewController release];
	[_file release];
    [super dealloc];
}

- (NSString *)windowNibName
{
    // Implement this to return a nib to load OR implement -makeWindowControllers to manually create your controllers.
    return @"WCDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)windowController {
	[super windowControllerDidLoadNib:windowController];
	
	_fileViewController = [[WCFileViewController alloc] initWithFile:_file];
	[[_fileViewController view] setFrameSize:[[[windowController window] contentView] frame].size];
	[[[windowController window] contentView] addSubview:[_fileViewController view]];
	[[windowController window] makeFirstResponder:[_fileViewController textView]];
}

- (BOOL)hasUndoManager {
	return NO;
}

- (BOOL)isDocumentEdited {
	return [_file hasUnsavedChanges];
}

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError
{
    // Insert code here to write your document to data of the specified type. If the given outError != NULL, ensure that you set *outError when returning nil.

    // You can also choose to override -fileWrapperOfType:error:, -writeToURL:ofType:error:, or -writeToURL:ofType:forSaveOperation:originalContentsURL:error: instead.

    // For applications targeted for Panther or earlier systems, you should use the deprecated API -dataRepresentationOfType:. In this case you can also choose to override -fileWrapperRepresentationOfType: or -writeToFile:ofType: instead.
	
    return [[[_file textStorage] string] dataUsingEncoding:[_file encoding]];
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
    // Insert code here to read your document from the given data of the specified type.  If the given outError != NULL, ensure that you set *outError when returning NO.

    // You can also choose to override -readFromFileWrapper:ofType:error: or -readFromURL:ofType:error: instead. 
    
    // For applications targeted for Panther or earlier systems, you should use the deprecated API -loadDataRepresentation:ofType. In this case you can also choose to override -readFromFile:ofType: or -loadFileWrapperRepresentation:ofType: instead.
    _file = [[WCFile alloc] initWithURL:[self fileURL]];
    return YES;
}

- (void)saveDocument:(id)sender {
	[super saveDocument:sender];
	
	[[self file] setChangeCount:0];
}

@synthesize file=_file;
@synthesize project=_project;
@end
