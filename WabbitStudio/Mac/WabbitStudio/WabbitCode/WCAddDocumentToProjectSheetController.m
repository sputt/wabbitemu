//
//  WCAddDocumentToProjectSheetController.m
//  WabbitStudio
//
//  Created by William Towe on 4/16/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCAddDocumentToProjectSheetController.h"
#import "WCDocument.h"
#import "WCDocumentController.h"
#import "WCProject.h"
#import "NSPopUpButton+WCExtensions.h"
#import "NSResponder+WCExtensions.h"
#import "WCGeneralPerformer.h"


@interface WCAddDocumentToProjectSheetController ()
@property (readonly,nonatomic) NSPopUpButton *popUpButton;
- (id)_initWithDocument:(WCDocument *)documentToAdd;
- (void)_updatePopUpButton;
@end

@implementation WCAddDocumentToProjectSheetController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[[NSNotificationCenter defaultCenter] removeObserver:self];
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

- (void)approveModalSheetAction:(id)sender {
	if (![[WCGeneralPerformer sharedPerformer] addDocument:_documentToAdd toProject:[[[self popUpButton] selectedItem] representedObject]]) {
		NSBeep();
		return;
	}
	
	[[self window] close];
	[[NSApplication sharedApplication] endSheet:[self window] returnCode:NSOKButton];
}

+ (void)presentSheetForDocument:(WCDocument *)documentToAdd; {
	if (![documentToAdd fileURL]) {
		[documentToAdd saveDocument:nil];
		return;
	}
	
	WCAddDocumentToProjectSheetController *controller = [[[self class] alloc] _initWithDocument:documentToAdd];
	
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
