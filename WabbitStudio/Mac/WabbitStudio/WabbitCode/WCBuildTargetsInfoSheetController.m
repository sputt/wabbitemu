//
//  WCBuildTargetInfoSheetController.m
//  WabbitStudio
//
//  Created by William Towe on 3/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBuildTargetsInfoSheetController.h"
#import "WCProject.h"
#import "WCBuildTarget.h"
#import "WCFile.h"
#import "NSPopUpButton+WCExtensions.h"
#import "NSArray+WCExtensions.h"
#import "WCDefines.h"
#import "WCPreferencesController.h"

#import <BWToolkitFramework/BWToolkitFramework.h>


@interface WCBuildTargetsInfoSheetController (Private)
- (void)_setupInputFilePopUpButton;
- (WCBuildTarget *)_selectedBuildTarget;
@end

@implementation WCBuildTargetsInfoSheetController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	_project = nil;
    [super dealloc];
}

- (NSString *)windowNibName {
	return @"WCBuildTargetsInfoSheet";
}

- (void)windowDidLoad {
    [super windowDidLoad];
    
    [_buttonBar setIsAtBottom:NO];
	[_buttonBar setIsResizable:NO];
	
	[self _setupInputFilePopUpButton];
	
	[_buildTargetsArrayController setSelectedObjects:[NSArray arrayWithObjects:[[self project] activeBuildTarget], nil]];
}

- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor {
	return ([[fieldEditor string] length] > 0);
}

@synthesize project=_project;

+ (void)presentBuildTargetInfoSheetControllerForProject:(WCProject *)project; {
	id controller = [self buildTargetInfoSheetControllerForProject:project];
	
	[NSApp beginSheet:[controller window] modalForWindow:[project windowForSheet] modalDelegate:self didEndSelector:@selector(_sheetDidEnd:code:info:) contextInfo:(void *)[controller retain]];
}

+ (id)buildTargetInfoSheetControllerForProject:(WCProject *)project; {
	return [[[[self class] alloc] initWithProject:project] autorelease];
}
- (id)initWithProject:(WCProject *)project; {
	if (!(self = [super initWithWindowNibName:[self windowNibName]]))
		return nil;
	
	_project = project;
	
	return self;
}

- (IBAction)addBuildTarget:(id)sender; {
	WCBuildTarget *bt = nil;
	
	if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesNewBuildTargetsCopyActiveBuildTargetKey])
		bt = [[[[self project] activeBuildTarget] mutableCopy] autorelease];
	else
		bt = [WCBuildTarget buildTargetWithOutputType:WCBuildTargetOutputTypeBinary outputName:NSLocalizedString(@"New Target", @"new build target default name")];
	
	[(NSArrayController *)[_tableView dataSource] insertObject:bt atArrangedObjectIndex:0];
	
	[_tableView editColumn:1 row:0 withEvent:nil select:YES];
}
- (IBAction)removeBuildTarget:(id)sender; {
	WCBuildTarget *bt = [[(NSArrayController *)[_tableView dataSource] selectedObjects] lastObject];
	
	if ([bt isActive]) {
		NSBeep();
		return;
	}
	
	NSAlert *alert = [NSAlert alertWithMessageText:[NSString stringWithFormat:NSLocalizedString(@"Delete \"%@\"", @"remove build target alert message text"),[bt name]] defaultButton:NS_LOCALIZED_STRING_DELETE alternateButton:NS_LOCALIZED_STRING_CANCEL otherButton:nil informativeTextWithFormat:[NSString stringWithFormat:NSLocalizedString(@"Are you sure you want to delete \"%@\"? This operation cannot be undone.", @"remove build target alert informative text"),[bt name]]];
	
	[alert beginSheetModalForWindow:[self window] modalDelegate:self didEndSelector:@selector(_deleteBuildTargetAlertDidEnd:code:info:) contextInfo:NULL];
}

- (IBAction)duplicateBuildTarget:(id)sender; {
	WCBuildTarget *bt = [[(NSArrayController *)[_tableView dataSource] selectedObjects] lastObject];
	WCBuildTarget *copy = [[bt mutableCopy] autorelease];

	[(NSArrayController *)[_tableView dataSource] insertObject:copy atArrangedObjectIndex:0];
	
	[_tableView editColumn:1 row:0 withEvent:nil select:YES];
}
- (IBAction)makeActiveBuildTarget:(id)sender; {
	WCBuildTarget *bt = [[(NSArrayController *)[_tableView dataSource] selectedObjects] lastObject];
	
	[[self project] setActiveBuildTarget:bt];
}

+ (void)_sheetDidEnd:(NSWindow *)sheet code:(NSInteger)code info:(void *)info {
	/*id controller =*/ [(id)info autorelease];
	
	
}

- (void)_deleteBuildTargetAlertDidEnd:(NSAlert *)alert code:(NSInteger)code info:(void *)info {
	if (code != NSAlertDefaultReturn)
		return;
	
	[[[self project] mutableBuildTargets] removeObject:[[(NSArrayController *)[_tableView dataSource] selectedObjects] lastObject]];
}

- (void)_setupInputFilePopUpButton; {
	NSMenu *menu = [[[NSMenu alloc] initWithTitle:@""] autorelease];
	
	[menu addItemWithTitle:NSLocalizedString(@"No Input File", @"build target no input file selected") action:@selector(_changeInputFile:) keyEquivalent:@""];
	[menu addItem:[NSMenuItem separatorItem]];
	
	for (WCFile *file in [[[self project] textFiles] sortedArrayUsingDescriptors:[NSArray arrayWithObjects:[[[NSSortDescriptor alloc] initWithKey:@"name" ascending:YES] autorelease], nil]]) {
		NSMenuItem *item = [menu addItemWithTitle:[file name] action:@selector(_changeInputFile:) keyEquivalent:@""];
		
		[item setImage:[file icon]];
		[item setRepresentedObject:file];
	}
	
	[_inputFilePopUpButton setMenu:menu];
	
	WCFile *ifile = [[[self project] textFiles] fileMatchingAlias:[[self _selectedBuildTarget] inputFileAlias]];
	
	if (!ifile)
		[_inputFilePopUpButton selectItemAtIndex:0];
	else
		[_inputFilePopUpButton selectItemWithRepresentedObject:ifile];
	
}

- (IBAction)_changeInputFile:(NSMenuItem *)item {
	[[self _selectedBuildTarget] setInputFileAlias:[(WCFile *)[item representedObject] alias]];
}

- (WCBuildTarget *)_selectedBuildTarget; {
	return [[_buildTargetsArrayController selectedObjects] lastObject];
}
@end
