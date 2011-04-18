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
#import "WCAddDocumentToProjectSheetController.h"
#import "WCDocumentController.h"


@implementation WCDocument

+ (BOOL)canConcurrentlyReadDocumentsOfType:(NSString *)typeName {
	if ([typeName isEqualToString:kWCFileAssemblyUTI] ||
		[typeName isEqualToString:kWCFileIncludeUTI])
		return YES;
	return NO;
}

- (id)init {
    self = [super init];
    if (self) {
         _file = [[WCFile alloc] initWithURL:nil];
    }
    
    return self;
}

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[_fileViewController release];
	[_file release];
    [super dealloc];
}

- (NSString *)windowNibName {
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

- (NSData *)dataOfType:(NSString *)typeName error:(NSError **)outError {	
    return [[[_file textStorage] string] dataUsingEncoding:[_file encoding]];
}

- (BOOL)readFromData:(NSData *)data ofType:(NSString *)typeName error:(NSError **)outError
{
    _file = [[WCFile alloc] initWithURL:[self fileURL]];
    return YES;
}

- (void)saveDocumentWithDelegate:(id)delegate didSaveSelector:(SEL)didSaveSelector contextInfo:(void *)contextInfo {
	[super saveDocumentWithDelegate:self didSaveSelector:@selector(document:didSave:info:) contextInfo:NULL];
}

- (void)document:(WCDocument *)document didSave:(BOOL)didSave info:(void *)info {
	if (didSave) {
		[[self file] setChangeCount:0];
		[[self file] setURL:[self fileURL]];
	}
}

- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item {
	if ([item action] == @selector(addDocumentToProject:)) {
		NSMenuItem *mItem = (NSMenuItem *)item;
		
		[mItem setTitle:[NSString stringWithFormat:NSLocalizedString(@"Add \"%@\" to Project\u2026", @"add document to project menu item title"),[self displayName]]];
		
		if ([[[WCDocumentController sharedDocumentController] projects] count] == 0)
			return NO;
		
		return YES;
	}
	return [super validateUserInterfaceItem:item];
}

@synthesize file=_file;
@synthesize project=_project;
@synthesize fileViewController=_fileViewController;

- (IBAction)addDocumentToProject:(id)sender; {
	[WCAddDocumentToProjectSheetController presentSheetForDocument:self];
}
@end
