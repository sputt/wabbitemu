//
//  WCAddDocumentToProjectSheet.m
//  WabbitStudio
//
//  Created by William Towe on 4/16/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCAddDocumentToProjectSheet.h"
#import "WCDocument.h"
#import "WCDocumentController.h"
#import "WCProject.h"
#import "WCProjectFile.h"
#import "WCProjectFilesOutlineViewController.h"
#import "NSTreeController+WCExtensions.h"
#import "NSWindow-NoodleEffects.h"
#import "WCTextStorage.h"
#import "WCFileViewController.h"
#import "WCTextView.h"
#import "NSPopUpButton+WCExtensions.h"
#import "NSResponder+WCExtensions.h"


@interface WCAddDocumentToProjectSheet ()
@property (readonly,nonatomic) NSPopUpButton *popUpButton;
- (id)_initWithDocument:(WCDocument *)documentToAdd;
- (void)_updatePopUpButton;
@end

@implementation WCAddDocumentToProjectSheet

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	_documentToAdd = nil;
    [super dealloc];
}

- (void)windowDidLoad {
    [super windowDidLoad];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_projectWillClose:) name:kWCProjectWillCloseNotification object:nil];
	
	[self _updatePopUpButton];
}

- (NSString *)windowNibName {
	return @"WCAddDocumentToProjectSheet";
}

+ (void)presentSheetForDocument:(WCDocument *)documentToAdd; {
	WCAddDocumentToProjectSheet *controller = [[[self class] alloc] _initWithDocument:documentToAdd];
	
	[[NSApplication sharedApplication] beginSheet:[controller window] modalForWindow:[documentToAdd windowForSheet] modalDelegate:controller didEndSelector:@selector(_sheetDidEnd:code:info:) contextInfo:NULL];
}

- (id)_initWithDocument:(WCDocument *)documentToAdd; {
	if (!(self = [super initWithWindowNibName:[self windowNibName]]))
		return nil;
	
	_documentToAdd = documentToAdd;
	
	return self;
}

@synthesize popUpButton=_popUpButton;

- (void)_sheetDidEnd:(NSWindow *)sheet code:(NSInteger)code info:(void *)info {
	[self autorelease];
	if (code == NSCancelButton)
		return;
	
	[[self window] orderOut:nil];
	
	WCProject *project = [[[self popUpButton] selectedItem] representedObject];
	
	// saving before hand just makes it easier, we can close the document instance without any problem
	[_documentToAdd saveDocument:nil];
	
	[[[_documentToAdd file] textStorage] removeLayoutManager:[[[_documentToAdd fileViewController] textView] layoutManager]];
	[[[_documentToAdd file] undoManager] removeAllActions];
	[[[[[_documentToAdd fileViewController] textView] enclosingScrollView] verticalRulerView] setClientView:nil];
	[[[project projectFile] mutableChildNodes] insertObject:[_documentToAdd file] atIndex:0];
	[(NSTreeController *)[[[project projectFilesOutlineViewController] outlineView] dataSource] setSelectedRepresentedObject:[_documentToAdd file]];
	
	[project viewProject:nil];
	
	NSRect rect = [[[project projectFilesOutlineViewController] outlineView] convertRectToBase:[[[project projectFilesOutlineViewController] outlineView] frameOfCellAtColumn:0 row:[[[project projectFilesOutlineViewController] outlineView] rowForItem:[(NSTreeController *)[[[project projectFilesOutlineViewController] outlineView] dataSource] treeNodeForRepresentedObject:[_documentToAdd file]]]]];
	rect.origin = [[project windowForSheet] convertBaseToScreen:rect.origin];
	
	if (!NSIsEmptyRect(rect)) {
		[[_documentToAdd windowForSheet] zoomOffToRect:rect];
	}
	
	[_documentToAdd close];
	
	[project noteNumberOfFilesChanged];
}

- (void)_projectWillClose:(NSNotification *)note {
	[[self popUpButton] removeItemWithRepresentedObject:[note object]];
	
	if ([[self popUpButton] numberOfItems] == 0) {
		[[self window] close];
		[[NSApplication sharedApplication] endSheet:[self window] returnCode:NSCancelButton];
		return;
	}
	
	[[self popUpButton] selectItemAtIndex:0];
}

- (void)_updatePopUpButton; {
	NSMenu *menu = [[[NSMenu alloc] initWithTitle:@""] autorelease];
	
	for (WCProject *project in [[WCDocumentController sharedDocumentController] projects]) {
		NSMenuItem *item = [menu addItemWithTitle:[project displayName] action:NULL keyEquivalent:@""];
		
		[item setImage:[[project projectFile] icon]];
		[item setRepresentedObject:project];
	}
	
	[[self popUpButton] setMenu:menu];
	[[self popUpButton] selectItemAtIndex:0];
}
@end
