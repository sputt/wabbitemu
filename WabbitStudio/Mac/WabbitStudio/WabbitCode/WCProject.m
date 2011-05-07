//
//  WCProject.m
//  WabbitCode
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProject.h"
#import "NSArray+WCExtensions.h"
#import "WCProjectFile.h"
#import "WCBadgedTextFieldCell.h"
#import "NSFileManager+WCExtensions.h"
#import "WCGeneralPerformer.h"
#import "NSOutlineView+WCExtensions.h"
#import "WCAddFilesToProjectViewController.h"
#import "WCFileViewController.h"
#import "NSTreeController+WCExtensions.h"
#import "WCSymbolScanner.h"
#import "WCTextView.h"
#import "WCSymbol.h"
#import "WCTextStorage.h"
#import "WCBuildMessage.h"
#import "WCBuildTarget.h"
#import "WCBuildTargetsInfoSheetController.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "WCAlias.h"
#import "WCBuildDefine.h"
#import "WCIncludeDirectory.h"
#import "WCProjectFilesOutlineViewController.h"
#import "WCBuildMessagesViewController.h"
#import "WCSymbolsViewController.h"
#import "WCSymbol.h"
#import "WCNewFileWindowController.h"
#import "WCFindInProjectViewController.h"
#import "WCProjectBuildTargetPopUpButton.h"
#import "WCFindInProjectResult.h"
#import "WCProjectNavView.h"
#import "NSAlert-OAExtensions.h"
#import "WCUnsavedFilesWindowController.h"
#import "CTBadge.h"
#import "NSTextView+WCExtensions.h"
#import "WCSyntaxHighlighter.h"
#import "WCBreakpointsViewController.h"
#import "WCBreakpoint.h"
#import "WCFileWindowController.h"
#import "NSString+WCExtensions.h"
#import "WCDebuggerWindowController.h"
#import "WETransferSheetController.h"
#import "RSCalculator.h"
#import "RSDebuggerDetailsViewController.h"
#import "RSDebuggerMemoryViewController.h"


#import <PSMTabBarControl/PSMTabBarControl.h>
#import <BWToolkitFramework/BWAnchoredButtonBar.h>
#import <BWToolkitFramework/BWAnchoredButton.h>

NSString *const kWCProjectFileKey = @"file";
NSString *const kWCProjectVersionKey = @"version";
NSString *const kWCProjectBuildTargetsKey = @"buildTargets";

const CGFloat kWCProjectVersion = 1.5;

NSString *const kWCProjectDataFileName = @"project.wcodedata";
NSString *const kWCProjectWrapperExtension = @"wcodeproj";

NSString *const kWCProjectUTI = @"org.revsoft.wabbitcode.project";

NSString *const kWCProjectActiveBuildTargetDidChangeNotification = @"kWCProjectActiveBuildTargetDidChangeNotification";
NSString *const kWCProjectNumberOfBuildTargetsDidChangeNotification = @"kWCProjectNumberOfBuildTargetsDidChangeNotification";
NSString *const kWCProjectNumberOfFilesDidChangeNotification = @"kWCProjectNumberOfFilesDidChangeNotification";
NSString *const kWCProjectWillCloseNotification = @"kWCProjectWillCloseNotification";

NSString *const kWCProjectSettingsProjectFilesOutlineViewExpandedItemUUIDsKey = @"projectFilesOutlineViewExpandedItemUUIDs";
NSString *const kWCProjectSettingsProjectWindowFrameKey = @"projectWindowFrame";
NSString *const kWCProjectSettingsLeftVerticalSplitViewDividerPositionKey = @"leftVerticalSplitViewDividerPosition";
NSString *const kWCProjectSettingsOpenFileUUIDsKey = @"projectOpenFileUUIDs";
NSString *const kWCProjectSettingsSelectedFileUUIDKey = @"projectSelectedFileUUID";
NSString *const kWCProjectSettingsFileSettingsDictionaryKey = @"projectFileSettingsDictionary";
NSString *const kWCProjectSettingsFileSettingsFileSeparateEditorWindowFrameKey = @"projectFileSettingsFileSeparateWindowFrame";
NSString *const kWCProjectSettingsRomOrSavestateAliasKey = @"projectSettingsRomOrSavestateAlias";

static NSString *const kWCProjectErrorDomain = @"org.revsoft.wabbitcode.project.error";
static const NSInteger kWCProjectDataFileMovedErrorCode = 1001;
static const NSInteger kWCProjectDataFileIncorrectFormatErrorCode = 1002;
static const NSInteger kWCProjectDataFileOldVersionErrorCode = 1003;

static NSImage *_appIcon = nil;

@interface WCProject ()
@property (readwrite,retain,nonatomic) WCProjectFile *projectFile;
@property (readwrite,retain,nonatomic) NSSet *absoluteFilePaths;
@property (readwrite,retain,nonatomic) NSString *codeListing;
@property (assign,nonatomic) BOOL shouldRunAfterBuilding;
@property (assign,nonatomic) BOOL shouldDebugAfterBuilding;
@property (readwrite,retain,nonatomic) WCAlias *romOrSavestateAlias;
@property (readwrite,assign,nonatomic) WCFile *currentBreakpointFile;
@property (readwrite,assign,nonatomic) NSUInteger currentBreakpointLineNumber;
@property (readwrite,assign,nonatomic) WCFile *programCounterFile;
@property (readwrite,assign,nonatomic) NSUInteger programCounterLineNumber;

- (void)_addBuildMessageForString:(NSString *)string;

- (void)_updateProjectSettings;
- (void)_applyProjectSettings;

- (void)_deleteObjects:(NSArray *)objects deleteFiles:(BOOL)deleteFiles;

- (WCFileWindowController *)_openSeparateEditorForFile:(WCFile *)file;

- (void)_createDockIconBadgesIfNecessary;
@end

@implementation WCProject
#pragma mark -
#pragma mark *** Subclass Overrides ***
+ (void)initialize {
	if ([WCProject class] != self)
		return;
	
	_appIcon = [[NSImage imageNamed:NSImageNameApplicationIcon] copy];
}

+ (BOOL)canConcurrentlyReadDocumentsOfType:(NSString *)typeName {
	if ([typeName isEqualToString:kWCProjectUTI])
		return YES;
	return NO;
}

+ (NSSet *)keyPathsForValuesAffectingShouldAnimate {
    return [NSSet setWithObjects:@"isBuilding",@"isLoadingRom", nil];
}

- (NSUndoManager *)undoManager {
	return nil;
}

- (BOOL)hasUndoManager {
	return NO;
}

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_debuggerMemoryViewController release];
	[_debuggerDetailsViewController release];
	[_calculator removeObserver:self forKeyPath:@"programCounter"];
	[_calculator release];
	[_romOrSavestateAlias release];
	if (_buildTask != nil)
		[_buildTask terminate];
	[_buildTask release];
	[_secondaryStatusString release];
	[_statusString release];
	[_currentAddFilesToProjectViewController release];
	[_codeListing release];
	[_errorBadge release];
	[_warningBadge release];
	_currentViewController = nil;
	[_cachedAbsoluteFilePaths release];
	[_breakpointsViewController release];
	[_projectBreakpoint release];
	[_projectFilesOutlineViewController release];
	[_buildMessagesViewController release];
	[_buildMessages release];
	[_symbolsViewController release];
	[_findInProjectViewController release];
	[_projectSettings release];
	[_openFiles release];
	[_filesToFileViewControllers release];
	[_buildTargets release];
	[_projectFile release];
	[super dealloc];
}

- (NSString *)windowNibName {
	return @"WCProject";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)controller {
	[super windowControllerDidLoadNib:controller];
	
	[controller setShouldCloseDocument:YES];
	
	[_tabBarControl setCanCloseOnlyTab:YES];
	[_tabBarControl setStyleNamed:@"Unified"];
	[_tabBarControl setHideForSingleTab:NO];
	[_tabBarControl setUseOverflowMenu:YES];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileHasUnsavedChanges:) name:kWCFileHasUnsavedChangesNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileNameDidChange:) name:kWCFileNameDidChangeNotification object:nil];

	[_rightButtonBar setIsAtBottom:YES];
	[_rightButtonBar setIsResizable:NO];
	[_debuggerButtonBar setIsAtBottom:YES];
	[_debuggerButtonBar setIsResizable:NO];
	
	[self setCurrentViewController:[self projectFilesOutlineViewController]];
	
	[_rightNavigatorControl setSelectedItemIndex:0];
	
	[self _applyProjectSettings];
	
	NSString *identifier = [NSString stringWithFormat:@"%@ToolbarIdentifier",[self className]];
	NSToolbar *toolbar = [[[NSToolbar alloc] initWithIdentifier:identifier] autorelease];
	
	[toolbar setAllowsUserCustomization:YES];
	[toolbar setDisplayMode:NSToolbarDisplayModeIconAndLabel];
	[toolbar setSizeMode:NSToolbarSizeModeRegular];
	[toolbar setDelegate:self];
	
#ifndef DEBUG
	[toolbar setAutosavesConfiguration:YES];
#endif
	
	[[controller window] setToolbar:toolbar];
}

#pragma mark Reading and Writing
- (BOOL)readFromFileWrapper:(NSFileWrapper *)fileWrapper ofType:(NSString *)typeName error:(NSError **)outError {
	// we should only ever be asked to open project file types
#ifdef DEBUG
	NSAssert([typeName isEqualToString:kWCProjectUTI], @"passed document type other than project to WCProject!");
#endif
	
	NSDictionary *wrapperDict = [fileWrapper fileWrappers];
	NSFileWrapper *dataWrapper = [wrapperDict objectForKey:kWCProjectDataFileName];
	
	// the data file was moved
	if (!dataWrapper) {
		if (outError) {
			NSError *error = [NSError errorWithDomain:kWCProjectErrorDomain code:kWCProjectDataFileMovedErrorCode userInfo:nil];
			
			*outError = error;
		}
		return NO;
	}
	
	NSData *data = [dataWrapper regularFileContents];
	NSDictionary *dict = [NSKeyedUnarchiver unarchiveObjectWithData:data];
	
	// couldn't create dict from wrapper data or it's in the wrong format
	if (!dict) {
		if (outError) {
			NSError *error = [NSError errorWithDomain:kWCProjectErrorDomain code:kWCProjectDataFileIncorrectFormatErrorCode userInfo:nil];
			
			*outError = error;
		}
		return NO;
	}
	
	CGFloat version = [[dict objectForKey:kWCProjectVersionKey] floatValue];
	
	// older version that we can't read
	if (version < kWCProjectVersion) {
		if (outError) {
			NSError *error = [NSError errorWithDomain:kWCProjectErrorDomain code:kWCProjectDataFileOldVersionErrorCode userInfo:nil];
			
			*outError = error;
		}
		return NO;
	}
	
	WCProjectFile *projectFile = [dict objectForKey:kWCProjectFileKey];
	
	_projectFile = [projectFile retain];
	[_projectFile setProject:self];
	
	_buildTargets = [[NSMutableArray alloc] init];
	
	[_buildTargets addObjectsFromArray:[dict objectForKey:kWCProjectBuildTargetsKey]];
	
	for (WCBuildTarget *bt in _buildTargets)
		[bt setProject:self];
	
	for (WCFile *file in [self textFiles])
		[[file symbolScanner] scanSymbols];
	
	NSString *settingsFileName = [NSUserName() stringByAppendingPathExtension:@"wcodesettings"];
	NSFileWrapper *settingsWrapper = [wrapperDict objectForKey:settingsFileName];
	
	if (settingsWrapper) {
		NSData *settingsData = [settingsWrapper regularFileContents];
		
		if (settingsData) {
			NSMutableDictionary *settingsDict = [NSPropertyListSerialization propertyListWithData:settingsData options:NSPropertyListMutableContainers format:NULL error:NULL];
			
			_projectSettings = [settingsDict retain];
		}
	}
	else
		_projectSettings = [[NSMutableDictionary alloc] init];
		
	return YES;
}

- (NSFileWrapper *)fileWrapperOfType:(NSString *)typeName error:(NSError **)outError {
	return [[WCGeneralPerformer sharedPerformer] fileWrapperForProject:self error:outError];	
}
#pragma mark IBActions
- (void)saveDocument:(id)sender {
	[self saveProjectFile];
	
	WCTextView *textView = [[self currentTabViewContext] selectedTextView];
	WCFile *file = [textView file];
	
	if (!file)
		return;
	
	if ([textView isCoalescingUndo])
		[textView breakUndoCoalescing];
	
	NSError *error = nil;
	if (![file saveFile:&error] && error)
		[self presentError:error];
}
- (void)revertDocumentToSaved:(id)sender {
	NSAlert *alert = [NSAlert alertWithMessageText:[NSString stringWithFormat:NSLocalizedString(@"Do you want to revert to the most recently saved version of the document \"%@?\"", @"revert alert message text"),[[[[[[self currentTabViewContext] tabBarControl] tabView] selectedTabViewItem] identifier] name]]  defaultButton:NSLocalizedString(@"Revert", @"Revert") alternateButton:NS_LOCALIZED_STRING_CANCEL otherButton:nil informativeTextWithFormat:NSLocalizedString(@"This operation cannot be undone. Your current changes will be lost.", @"revert alert informative text")];
	
	[alert setAlertStyle:NSCriticalAlertStyle];
	
	[alert beginSheetModalForWindow:[[self currentTabViewContext] tabWindow] completionHandler:^(NSAlert *mAlert, NSInteger result) {
		if (result != NSAlertDefaultReturn)
			return;
		
		WCFile *file = [[[[[self currentTabViewContext] tabBarControl] tabView] selectedTabViewItem] identifier];
		NSError *error = nil;
		
		if (![file resetFile:&error] && error != nil)
			[self presentError:error];
	}];
}
- (void)printDocument:(id)sender {
	NSLog(@"%@ not implemented in %@",NSStringFromSelector(_cmd),[self className]);
	NSBeep();
}
#pragma mark -
#pragma mark *** Protocol Overrides ***
#pragma mark NSKeyValueObserving
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
	if ([keyPath isEqualToString:@"programCounter"] && context == self)
		[self jumpToProgramCounter];
	else
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
}
#pragma mark NSToolbarDelegate

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar {
	return [NSArray arrayWithObjects:kWCProjectToolbarBuildTargetPopUpButtonItemIdentifier,kWCProjectToolbarStatusViewItemIdentifier,kWCProjectToolbarBuildItemIdentifier,kWCProjectToolbarBuildAndRunItemIdentifier,kWCProjectToolbarBuildAndDebugItemIdentifer,NSToolbarSeparatorItemIdentifier,NSToolbarFlexibleSpaceItemIdentifier,nil];
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar {
	return [NSArray arrayWithObjects:kWCProjectToolbarBuildTargetPopUpButtonItemIdentifier,NSToolbarFlexibleSpaceItemIdentifier,kWCProjectToolbarStatusViewItemIdentifier,NSToolbarFlexibleSpaceItemIdentifier,kWCProjectToolbarBuildItemIdentifier,kWCProjectToolbarBuildAndRunItemIdentifier,kWCProjectToolbarBuildAndDebugItemIdentifer,nil];
}

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSString *)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag {
	return [[WCGeneralPerformer sharedPerformer] toolbar:toolbar itemForItemIdentifier:itemIdentifier willBeInsertedIntoToolbar:flag inProject:self];
}

#pragma mark NSOpenSavePanelDelegate
- (BOOL)panel:(id)sender shouldEnableURL:(NSURL *)url {
	return (![[self absoluteFilePaths] containsObject:[url path]]);
}
#pragma mark NSSplitViewDelegate
- (BOOL)splitView:(NSSplitView *)splitView shouldAdjustSizeOfSubview:(NSView *)view {
	if ([[splitView subviews] firstObject] == view || [[splitView subviews] lastObject] == view)
		return NO;
	return YES;
}

#define kWCProjectSplitViewLeftMin 175.0
#define kWCProjectSplitViewRightMin 350.0

- (CGFloat)splitView:(NSSplitView *)splitView constrainMaxCoordinate:(CGFloat)proposedMaximumPosition ofSubviewAt:(NSInteger)dividerIndex {
	if (dividerIndex == 0)
		return proposedMaximumPosition - kWCProjectSplitViewRightMin;
	return proposedMaximumPosition;
}

- (CGFloat)splitView:(NSSplitView *)splitView constrainMinCoordinate:(CGFloat)proposedMinimumPosition ofSubviewAt:(NSInteger)dividerIndex {
	if (dividerIndex == 0)
		return proposedMinimumPosition + kWCProjectSplitViewLeftMin;
	return proposedMinimumPosition;
}

- (NSRect)splitView:(NSSplitView *)splitView additionalEffectiveRectOfDividerAtIndex:(NSInteger)dividerIndex {
	if (dividerIndex == 1)
		return [splitView convertRect:[_rightSplitterHandleImageView frame] fromView:_rightSplitterHandleImageView];
	return [splitView convertRectToBase:[[(id)[self currentViewController] splitterHandleImageView] frame]];
}

#pragma mark NSUserInterfaceValidations
- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)item {
	if ([item action] == @selector(rename:)) {
		if (![[[[self projectFilesOutlineViewController] selectedNode] representedObject] canEditName])
			return NO;
		return YES;
	}
	else if ([item action] == @selector(openInSeparateEditor:)) {
		for (NSTreeNode *node in [[self projectFilesOutlineViewController] selectedNodes]) {
			if (![[node representedObject] isTextFile])
				return NO;
		}
		return YES;
	}
	else if ([item action] == @selector(addFilesToProject:)) {
		
		if ([(id <NSObject>)item isKindOfClass:[NSMenuItem class]])
			[(NSMenuItem *)item setTitle:[NSString stringWithFormat:NSLocalizedString(@"Add Files to \"%@\"\u2026", @"project add files to project menu item title"),[self displayName]]];
		
		return YES;
	}
	else if ([item action] == @selector(nextTab:)) {
		if ([[[[self currentTabViewContext] tabBarControl] tabView] numberOfTabViewItems] <= 1)
			return NO;
		return YES;
	}
	else if ([item action] == @selector(previousTab:)) {
		if ([[[[self currentTabViewContext] tabBarControl] tabView] numberOfTabViewItems] <= 1)
			return NO;
		return YES;
	}
	else if ([item action] == @selector(group:)) {
		for (WCFile *file in [(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] selectedRepresentedObjects]) {
			if ([file isKindOfClass:[WCProjectFile class]])
				return NO;
		}
		return YES;
	}
	else if ([item action] == @selector(ungroup:)) {
		for (WCFile *file in [(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] selectedRepresentedObjects]) {
			if ([file isKindOfClass:[WCProjectFile class]] ||
				![file countOfChildNodes])
				return NO;
		}
		return YES;
	}
	else if ([item action] == @selector(delete:)) {
		if ([self currentViewController] == [self breakpointsViewControllerDontCreate]) {
			for (WCBreakpoint *breakpoint in [[self breakpointsViewController] selectedObjects]) {
				if ([breakpoint breakpointType] != WCBreakpointTypeLine)
					return NO;
			}
			return YES;
		}
		
		for (WCFile *file in [[self projectFilesOutlineViewController] selectedObjects])
			if ([file isKindOfClass:[WCProjectFile class]])
				return NO;
		return YES;
	}
	else if ([item action] == @selector(saveDocumentAs:)) {
		return NO;
	}
	else if ([item action] == @selector(revertDocumentToSaved:)) {
		if ([[[[self currentTabViewContext] tabBarControl] tabView] numberOfTabViewItems] == 0)
			return NO;
		return YES;
	}
	return [super validateUserInterfaceItem:item];
}
#pragma mark NSWindowDelegate
- (BOOL)windowShouldClose:(id)sender {
	if (![[self unsavedTextFiles] count])
		return YES;
	
	return ([WCUnsavedFilesWindowController runModalForProject:self] == NSOKButton);
}

- (void)windowWillClose:(NSNotification *)note {
	_isClosing = YES;
	
	// post our notification
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCProjectWillCloseNotification object:self];
	
	// save the project file before removing the tabs
	[self saveProjectFile];
	
	// otherwise crashes ensue, PSMTabBarControl retains things too long
	for (NSTabViewItem *item in [[[self tabBarControl] tabView] tabViewItems])
		[[[self tabBarControl] tabView] removeTabViewItem:item];
}
#pragma mark PSMTabBarControlDelegate
- (BOOL)tabView:(NSTabView *)tabView shouldCloseTabViewItem:(NSTabViewItem *)tabViewItem; {
	if ([[self openFiles] countForObject:[tabViewItem identifier]] > 1 || ![[tabViewItem identifier] hasUnsavedChanges])
		return YES;
	
	NSAlert *alert = [NSAlert alertWithMessageText:[NSString stringWithFormat:NSLocalizedString(@"Do you want to save the changes you made in the document \"%@\"?", @"unsaved file alert message text"),[[tabViewItem identifier] name]] defaultButton:NSLocalizedString(@"Save", @"Save") alternateButton:NSLocalizedString(@"Don't Save", @"Don't Save") otherButton:NS_LOCALIZED_STRING_CANCEL informativeTextWithFormat:NSLocalizedString(@"Your changes will be lost if you don't save them.", @"unsaved file alert informative text")];
	
	[alert setAlertStyle:NSCriticalAlertStyle];
	
	[alert beginSheetModalForWindow:[[tabViewItem tabView] window] completionHandler:^(NSAlert *mAlert, NSInteger result) {
		if (result == NSAlertOtherReturn)
			return;
		else if (result == NSAlertDefaultReturn)
			[[tabViewItem identifier] saveFile:NULL];
		else {
			[[tabViewItem identifier] resetFile:NULL];
		}
		
		[[tabViewItem tabView] removeTabViewItem:tabViewItem];
		
		if ([[[tabView window] windowController] isKindOfClass:[WCFileWindowController class]])
			[[tabView window] close];
	}];
	
	return NO;
}
- (void)tabView:(NSTabView *)tabView didCloseTabViewItem:(NSTabViewItem *)tabViewItem; {
	[self removeFileViewControllerForFile:[tabViewItem identifier] inTabViewContext:self];
}
- (void)tabView:(NSTabView *)tabView didSelectTabViewItem:(NSTabViewItem *)tabViewItem {
	WCFileViewController *controller = [self fileViewControllerForFile:[tabViewItem identifier] inTabViewContext:self selectTab:NO];
	
	[[[controller textView] syntaxHighlighter] performSyntaxHighlighting];
}

- (BOOL)tabView:(NSTabView *)tabView shouldDragTabViewItem:(NSTabViewItem *)tabViewItem fromTabBar:(PSMTabBarControl *)tabBarControl; {
	if ([tabView numberOfTabViewItems] == 1)
		return NO;
	return YES;
}
- (BOOL)tabView:(NSTabView *)tabView shouldDropTabViewItem:(NSTabViewItem *)tabViewItem inTabBar:(PSMTabBarControl *)tabBarControl; {
	return YES;
}
- (BOOL)tabView:(NSTabView *)tabView shouldAllowTabViewItem:(NSTabViewItem *)tabViewItem toLeaveTabBar:(PSMTabBarControl *)tabBarControl; {
	return NO;
}
- (void)tabView:(NSTabView*)tabView didDropTabViewItem:(NSTabViewItem *)tabViewItem inTabBar:(PSMTabBarControl *)tabBarControl; {
	
}

- (NSString *)tabView:(NSTabView *)tabView toolTipForTabViewItem:(NSTabViewItem *)tabViewItem; {
	return [(WCFile *)[tabViewItem identifier] absolutePathForDisplay];
}
#pragma mark WCTabViewContext
- (NSWindow *)tabWindow; {
	return [[[self windowControllers] lastObject] window];
}
- (PSMTabBarControl *)tabBarControl; {
	return _tabBarControl;
}
- (WCTextView *)selectedTextView; {
	if (![[[self tabBarControl] tabView] numberOfTabViewItems])
		return nil;
	
	WCTextView *retval = nil;
	for (NSView *view in [[[[[self tabBarControl] tabView] selectedTabViewItem] view] subviews]) {
		if ([view isKindOfClass:[NSScrollView class]])
			retval = (WCTextView *)[(NSScrollView *)view documentView];
	}
	return retval;
}
#pragma mark RSCalculatorOwner

- (void)handleBreakpointCallback; {
	[self setIsDebugging:YES];
	[[self calculator] setIsDebugging:YES];
	[self jumpToProgramCounter];
}

- (void)transferFinished; {
	for (WCBreakpoint *breakpoint in [self allBreakpoints]) {
		if ([breakpoint isActive])
			[[self calculator] setBreakpointOfType:RSBreakpointTypeNormal atAddress:[breakpoint address]];
	}
	
	calc_run_seconds([[self calculator] calc], 1.0);
	// 2nd
	[[self calculator] simulateKeyPress:48];
	// 0 for catalog
	[[self calculator] simulateKeyPress:29];
	// down
	[[self calculator] simulateKeyPress:125];
	[[self calculator] simulateKeyPress:125];
	[[self calculator] simulateKeyPress:125];
	[[self calculator] simulateKeyPress:125];
	[[self calculator] simulateKeyPress:125];
	[[self calculator] simulateKeyPress:125];
	// return
	[[self calculator] simulateKeyPress:36];
	// programs
	[[self calculator] simulateKeyPress:38];
	// return
	[[self calculator] simulateKeyPress:36];
	// return
	[[self calculator] simulateKeyPress:36 lastKeyPressInSeries:YES];
	
	[[self debuggerWindowController] showWindow:nil];
}
#pragma mark RSNavigatorControlDataSource

- (NSUInteger)numberOfItemsInNavigatorControl:(RSNavigatorControl *)navigatorControl {
	if (navigatorControl == _rightNavigatorControl) {
		return (_isClosing)?0:2;
	}
	return 0;
}

- (NSSize)imageSizeForNavigatorControl:(RSNavigatorControl *)navigatorControl {
	return WCSmallSize;
}

- (CGFloat)itemWidthForNavigatorControl:(RSNavigatorControl *)navigatorControl {
	return (WCSmallSize.width*2);
}

- (NSImage *)navigatorControl:(RSNavigatorControl *)navigatorControl imageForItemAtIndex:(NSUInteger)itemIndex {
	if (navigatorControl == _rightNavigatorControl) {
		if (itemIndex == 0)
			return [NSImage imageNamed:NSImageNameInfo];
		else
			return [NSImage imageNamed:@"Memory16x16"];
	}
	return nil;
}

- (NSView *)navigatorControl:(RSNavigatorControl *)navigatorControl viewForItemAtIndex:(NSUInteger)itemIndex {
	if (navigatorControl == _rightNavigatorControl) {
		if (itemIndex == 0) {
			if (_debuggerDetailsViewController == nil)
				_debuggerDetailsViewController = [[RSDebuggerDetailsViewController alloc] initWithCalculator:[self calculator]];
			
			return [_debuggerDetailsViewController view];
		}
		else {
			if (_debuggerMemoryViewController == nil)
				_debuggerMemoryViewController = [[RSDebuggerMemoryViewController alloc] initWithCalculator:[self calculator]];
			
			return [_debuggerMemoryViewController view];
		}
	}
	return nil;
}

#pragma mark -
#pragma mark *** Public Methods ***
- (NSArray *)symbolsForSymbolName:(NSString *)name; {
	NSMutableArray *retval = [NSMutableArray array];
	
	for (WCFile *file in [self textFiles]) {
		[retval addObjectsFromArray:[[file symbolScanner] symbolsForSymbolName:name]];
	}
	
	return [[retval copy] autorelease];
}
- (void)jumpToObject:(id <WCJumpToObject>)object; {
	[self jumpToObjects:[NSArray arrayWithObject:object]];
}
- (void)jumpToObjects:(NSArray *)objects; {
	for (id <WCJumpToObject> object in objects) {
		if (![object shouldJumpToObject])
			continue;
		
		WCFileViewController *controller = [self addFileViewControllerForFile:[object jumpToFile] inTabViewContext:[self currentTabViewContext]];
		
#ifdef DEBUG
		NSAssert(controller != nil, @"controller in jumpToObjects cannot be nil!");
#endif
		
		[[controller textView] setSelectedRangeSafely:[object jumpToRange] scrollRangeToVisible:YES];
	}
}
- (void)saveProjectFile; {
	[self _updateProjectSettings];
	[super saveDocument:nil];
}

- (void)noteNumberOfFilesChanged {
	[self setAbsoluteFilePaths:[NSSet setWithArray:[[[self projectFile] descendantNodes] valueForKeyPath:@"absolutePath"]]];
	
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCProjectNumberOfFilesDidChangeNotification object:self];
}

- (void)jumpToProgramCounter; {
	[self updateCurrentBreakpointFileAndLineNumber];
	[self updateProgramCounterFileAndLineNumber];
	[self addFileViewControllerForFile:[self programCounterFile] inTabViewContext:[self currentTabViewContext]];
	[[[self currentTabViewContext] selectedTextView] setSelectedLineNumber:[self programCounterLineNumber] scrollRangeToVisible:YES];
	[[[[[self currentTabViewContext] selectedTextView] enclosingScrollView] verticalRulerView] setNeedsDisplay:YES];
}

- (void)updateCurrentBreakpointFileAndLineNumber; {
	[self updateCurrentBreakpointFile];
	[self updateCurrentBreakpointLineNumber];
}

- (void)updateCurrentBreakpointFile; {
	if (![self isDebugging]) {
		[self setCurrentBreakpointFile:nil];
		return;
	}
	
	// update the current breakpoint file
	uint16_t pc = [[self calculator] programCounter];
	for (WCFile *file in [self textFiles]) {
		for (WCBreakpoint *breakpoint in [file allBreakpoints]) {
			if ([breakpoint address] == pc) {
#ifdef DEBUG
				NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
				[self setCurrentBreakpointFile:file];
				return;
			}
		}
	}
	[self setCurrentBreakpointFile:nil];
}
- (void)updateCurrentBreakpointLineNumber; {
	if ([self currentBreakpointFile] == nil) {
		[self setCurrentBreakpointLineNumber:0];
		return;
	}
	
	// update the current breakpoint line number
	uint16_t pc = [[self calculator] programCounter];
	for (WCFile *file in [self textFiles]) {
		for (WCBreakpoint *breakpoint in [file allBreakpoints]) {
			if ([breakpoint address] == pc) {
				NSUInteger lineNumber = [breakpoint lineNumber];
				
				if (lineNumber == [breakpoint symbolLineNumber])
					lineNumber++;
				
				[self setCurrentBreakpointLineNumber:lineNumber];
				return;
			}
		}
	}
	[self setCurrentBreakpointLineNumber:0];
}

- (void)updateProgramCounterFileAndLineNumber; {
	[self updateProgramCounterFile];
	[self updateProgramCounterLineNumber];
}

- (void)updateProgramCounterFile {
	// update the program counter file
	uint8_t page = 0; // this will need to be changed to handle flash applications
	uint16_t address = [[self calculator] programCounter];
	NSString *findString = [NSString stringWithFormat:@"%02u:%04X",page,address];
	NSRange fRange = [[self codeListing] rangeOfString:findString options:NSLiteralSearch];
	
	if (fRange.location == NSNotFound) {
#ifdef DEBUG
		NSLog(@"could not find current program counter %04X in listing file for project %@",address,[self displayName]);
#endif
		NSBeep();
		[self setProgramCounterFile:nil];
		return;
	}
	
	NSRange pRange = [[self codeListing] rangeOfString:@"Listing for file \"" options:NSLiteralSearch|NSBackwardsSearch range:NSMakeRange(0, NSMaxRange(fRange))];
	NSString *pString = [[self codeListing] substringWithRange:[[self codeListing] lineRangeForRange:pRange]];
	NSScanner *scanner = [NSScanner scannerWithString:pString];
	NSString *fPath;
	
	if (![scanner scanUpToString:@"\"" intoString:NULL]) {
		NSBeep();
		[self setProgramCounterFile:nil];
		return;
	}
	
	[scanner setScanLocation:[scanner scanLocation] + 1];
	
	if (![scanner scanUpToString:@"\"" intoString:&fPath]) {
		NSBeep();
		[self setProgramCounterFile:nil];
		return;
	}
	
	// find the file that matches the path
	for (WCFile *file in [self textFiles]) {
		if ([[file absolutePathForDisplay] isEqualToString:fPath]) {
			[self setProgramCounterFile:file];
			return;
		}
	}
#ifdef DEBUG
	NSLog(@"could not find file for path %@",fPath);
#endif
	[self setProgramCounterFile:nil];
}

- (void)updateProgramCounterLineNumber {
	// find where the listing for our file begins
	NSString *fileString = [NSString stringWithFormat:@"Listing for file \"%@\"",[[self programCounterFile] absolutePathForDisplay]];
	NSRange fileRange = [[self codeListing] rangeOfString:fileString options:NSLiteralSearch];
	
	uint8_t page = 0; // this will need to be changed to handle flash applications
	uint16_t address = [[self calculator] programCounter];
	NSString *pcString = [NSString stringWithFormat:@"%02u:%04X",page,address];
	NSRange pcRange = [[self codeListing] rangeOfString:pcString options:NSLiteralSearch|NSBackwardsSearch range:NSMakeRange(NSMaxRange(fileRange), [[self codeListing] length]-NSMaxRange(fileRange))];
	NSString *lineString = [[self codeListing] substringWithRange:[[self codeListing] lineRangeForRange:pcRange]];
	NSInteger lineNumber;
	NSScanner *pcScanner = [NSScanner scannerWithString:lineString];
	[pcScanner setCharactersToBeSkipped:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
	
	if (![pcScanner scanInteger:&lineNumber]) {
		[self setProgramCounterLineNumber:0];
		return;
	}
	
	[self setProgramCounterLineNumber:--lineNumber];
}

#pragma mark NSKeyValueCoding
- (NSUInteger)countOfBuildTargets {
	return [_buildTargets count];
}
- (id)objectInBuildTargetsAtIndex:(NSUInteger)index {
	return [_buildTargets objectAtIndex:index];
}
- (void)insertObject:(id)object inBuildTargetsAtIndex:(NSUInteger)index; {	
	[_buildTargets insertObject:object atIndex:index];
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCProjectNumberOfBuildTargetsDidChangeNotification object:self];
}
- (void)removeObjectFromChildNodesAtIndex:(NSUInteger)index; {	
	[_buildTargets removeObjectAtIndex:index];
	[[NSNotificationCenter defaultCenter] postNotificationName:kWCProjectNumberOfBuildTargetsDidChangeNotification object:self];
}

- (NSUInteger)countOfBuildMessages {
	return [_buildMessages count];
}
- (id)objectInBuildMessagesAtIndex:(NSUInteger)index {
	return [_buildMessages objectAtIndex:index];
}
- (void)insertObject:(id)object inBuildMessagesAtIndex:(NSUInteger)index; {
	if (!_buildMessages)
		_buildMessages = [[NSMutableArray alloc] init];
	[_buildMessages insertObject:object atIndex:index];
}
- (void)removeObjectFromBuildMessagesAtIndex:(NSUInteger)index; {	
	[_buildMessages removeObjectAtIndex:index];
}
- (void)removeAllBuildMessages; {
	[[self mutableBuildMessages] removeAllObjects];
	for (WCFile *file in [self textFiles])
		[file removeAllBuildMessages];
}
#pragma mark Tabs
- (WCFileViewController *)addFileViewControllerForFile:(WCFile *)file inTabViewContext:(id <WCTabViewContext>)tabViewContext; {
#ifdef DEBUG
	NSParameterAssert(file != nil);
	NSParameterAssert(tabViewContext != nil);
#endif
	
	WCFileViewController *retval = [self fileViewControllerForFile:file inTabViewContext:tabViewContext selectTab:YES];
	
	if (retval == nil) {
		// if the current context is a single window editor, we need to open a new window
		if ([tabViewContext isKindOfClass:[WCFileWindowController class]] &&
			[[[tabViewContext tabBarControl] tabView] numberOfTabViewItems] != 0 /*&&
			([[[NSApplication sharedApplication] currentEvent] modifierFlags] & NSAlternateKeyMask) != 0*/)
			return [self fileViewControllerForFile:file inTabViewContext:[self _openSeparateEditorForFile:file] selectTab:NO];
		
		retval = [WCFileViewController fileViewControllerWithFile:file inTabViewContext:tabViewContext];
		
#ifdef DEBUG
		NSParameterAssert(retval != nil);
#endif
		
		NSTabViewItem *item = [[[NSTabViewItem alloc] initWithIdentifier:file] autorelease];
		
		[item setLabel:[file name]];
		[item setView:[retval view]];
		[item setInitialFirstResponder:[retval textView]];
		
		[[[tabViewContext tabBarControl] tabView] addTabViewItem:item];
		[[[tabViewContext tabBarControl] tabView] selectTabViewItem:item];
		
		NSMutableArray *controllers = [[self filesToFileViewControllers] objectForKey:file];
		
		if (!controllers) {
			controllers = [NSMutableArray arrayWithCapacity:1];
			
			[[self filesToFileViewControllers] setObject:controllers forKey:file];
		}
		
#ifdef DEBUG
		NSParameterAssert(controllers != nil);
#endif
		
		[controllers addObject:retval];
		
		// increase the open count for this file, so we know when to prompt the user to save before closing said file
		[[self openFiles] addObject:file];
	}
	return retval;
}
- (WCFileViewController *)fileViewControllerForFile:(WCFile *)file inTabViewContext:(id <WCTabViewContext>)tabViewContext selectTab:(BOOL)selectTab; {
#ifdef DEBUG
	NSParameterAssert(file != nil);
	NSParameterAssert(tabViewContext != nil);
#endif
	for (WCFileViewController *controller in [[self filesToFileViewControllers] objectForKey:file]) {
		if ([controller tabViewContext] == tabViewContext) {
			if (selectTab) {
				for (NSTabViewItem *item in [[[tabViewContext tabBarControl] tabView] tabViewItems]) {
					if ([item identifier] == file) {
						[[[tabViewContext tabBarControl] tabView] selectTabViewItem:item];
						break;
					}
				}
			}
			return controller;
		}
	}
	return nil;
}
- (void)removeFileViewControllerForFile:(WCFile *)file inTabViewContext:(id <WCTabViewContext>)tabViewContext; {
#ifdef DEBUG
	NSParameterAssert(file != nil);
	NSParameterAssert(tabViewContext != nil);
#endif
	
	WCFileViewController *controller = [self fileViewControllerForFile:file inTabViewContext:tabViewContext selectTab:NO];

#ifdef DEBUG
	NSParameterAssert(controller != nil);
#endif
	
	if ([[self openFiles] countForObject:file] == 1)
		[[file undoManager] removeAllActions];
	
	[[file textStorage]	removeLayoutManager:[[controller textView] layoutManager]];
	[[[self filesToFileViewControllers] objectForKey:file] removeObject:controller];
	[[self openFiles] removeObject:file];
}
#pragma mark Accessors
@synthesize projectFile=_projectFile;
@synthesize buildTargets=_buildTargets;
@dynamic mutableBuildTargets;
- (NSMutableArray *)mutableBuildTargets {
	return [self mutableArrayValueForKey:@"buildTargets"];
}
@dynamic filesToFileViewControllers;
- (NSMapTable *)filesToFileViewControllers {
	if (!_filesToFileViewControllers)
		_filesToFileViewControllers = [[NSMapTable mapTableWithStrongToStrongObjects] retain];
	
	return _filesToFileViewControllers;
}
@dynamic textFiles;
- (NSArray *)textFiles {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCFile *file in [[self projectFile] descendantLeafNodes]) {
		if ([file isTextFile])
			[retval addObject:file];
	}
	return [[retval copy] autorelease];
}
@dynamic symbols;
- (NSArray *)symbols {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCFile *file in [self textFiles])
		[retval addObjectsFromArray:[[file symbolScanner] symbols]];
	return [[retval copy] autorelease];
}
@dynamic labelSymbols;
- (NSArray *)labelSymbols {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCFile *file in [self textFiles]) {
		[retval addObjectsFromArray:[[[file symbolScanner] labelNamesToSymbols] allValues]];
	}
	return [[retval copy] autorelease];
}
@dynamic equateSymbols;
- (NSArray *)equateSymbols {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCFile *file in [self textFiles]) {
		[retval addObjectsFromArray:[[[file symbolScanner] equateNamesToSymbols] allValues]];
	}
	return [[retval copy] autorelease];
}
@dynamic defineSymbols;
- (NSArray *)defineSymbols {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCFile *file in [self textFiles]) {
		[retval addObjectsFromArray:[[[file symbolScanner] defineNamesToSymbols] allValues]];
	}
	return [[retval copy] autorelease];
}
@dynamic macroSymbols;
- (NSArray *)macroSymbols {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCFile *file in [self textFiles]) {
		[retval addObjectsFromArray:[[[file symbolScanner] macroNamesToSymbols] allValues]];
	}
	return [[retval copy] autorelease];
}
@dynamic labelNamesToSymbols;
- (NSDictionary *)labelNamesToSymbols {
	NSMutableDictionary *retval = [NSMutableDictionary dictionary];
	for (WCFile *file in [self textFiles]) {
		[retval addEntriesFromDictionary:[[file symbolScanner] labelNamesToSymbols]];
	}
	return [[retval copy] autorelease];
}
@dynamic equateNamesToSymbols;
- (NSDictionary *)equateNamesToSymbols {
	NSMutableDictionary *retval = [NSMutableDictionary dictionary];
	for (WCFile *file in [self textFiles]) {
		[retval addEntriesFromDictionary:[[file symbolScanner] equateNamesToSymbols]];
	}
	return [[retval copy] autorelease];
}
@dynamic defineNamesToSymbols;
- (NSDictionary *)defineNamesToSymbols {
	NSMutableDictionary *retval = [NSMutableDictionary dictionary];
	for (WCFile *file in [self textFiles]) {
		[retval addEntriesFromDictionary:[[file symbolScanner] defineNamesToSymbols]];
	}
	return [[retval copy] autorelease];
}
@dynamic macroNamesToSymbols;
- (NSDictionary *)macroNamesToSymbols {
	NSMutableDictionary *retval = [NSMutableDictionary dictionary];
	for (WCFile *file in [self textFiles]) {
		[retval addEntriesFromDictionary:[[file symbolScanner] macroNamesToSymbols]];
	}
	return [[retval copy] autorelease];
}
@dynamic activeBuildTarget;
- (WCBuildTarget *)activeBuildTarget {
	WCBuildTarget *retval = nil;
	for (WCBuildTarget *bt in [self buildTargets]) {
		if ([bt isActive]) {
			retval = bt;
			break;
		}
	}
	
	if (!retval && [[self buildTargets] count]) {
		WCBuildTarget *bt = [[self buildTargets] lastObject];
		[bt setIsActive:YES];
		retval = bt;
	}
	return retval;
}
- (void)setActiveBuildTarget:(WCBuildTarget *)target {
	if ([self activeBuildTarget] == target)
		return;
	
	for (WCBuildTarget *bt in [self buildTargets])
		[bt setIsActive:NO];
	
	[target setIsActive:YES];

	[[NSNotificationCenter defaultCenter] postNotificationName:kWCProjectActiveBuildTargetDidChangeNotification object:self];
}
@dynamic openFiles;
- (NSCountedSet *)openFiles {
	if (!_openFiles)
		_openFiles = [[NSCountedSet alloc] init];
	return _openFiles;
}
@dynamic unsavedTextFiles;
- (NSArray *)unsavedTextFiles {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCFile *file in [self openFiles]) {
		if ([file hasUnsavedChanges])
			[retval addObject:file];
	}
	return [[retval copy] autorelease];
}
@synthesize projectSettings=_projectSettings;
@synthesize isBuilding=_isBuilding;
@dynamic currentViewController;
- (WCProjectNavigationViewController *)currentViewController {
	return _currentViewController;
}
- (void)setCurrentViewController:(WCProjectNavigationViewController *)controller {
	if (_currentViewController == controller) {
		return;
	}
	
	if (!_currentViewController) {
		[[controller view] setFrameSize:[_swapView frame].size];
		[_swapView addSubview:[controller view]];
	}
	else {
		[[controller view] setFrameSize:[[_currentViewController view] frame].size];
		[_swapView replaceSubview:[_currentViewController view] with:[controller view]];
	}
	
	_currentViewController = controller;
	
	// to counteract the BWAnchoredButtonBar, which sets itself as the delegate
	[_splitView setDelegate:self];
	[[[_currentViewController view] window] makeFirstResponder:[_currentViewController firstResponder]];	
}
@dynamic projectFilesOutlineViewController;
- (WCProjectFilesOutlineViewController *)projectFilesOutlineViewController {
	if (!_projectFilesOutlineViewController)
		_projectFilesOutlineViewController = [[WCProjectFilesOutlineViewController projectNavigationViewControllerWithProject:self] retain];
	return _projectFilesOutlineViewController;
}
@dynamic buildMessagesViewController;
- (WCBuildMessagesViewController *)buildMessagesViewController {
	if (!_buildMessagesViewController)
		_buildMessagesViewController = [[WCBuildMessagesViewController projectNavigationViewControllerWithProject:self] retain];
	
	return _buildMessagesViewController;
}
@synthesize buildMessages=_buildMessages;
@dynamic mutableBuildMessages;
- (NSMutableArray *)mutableBuildMessages {
	if (!_buildMessages)
		_buildMessages = [[NSMutableArray alloc] init];
	return [self mutableArrayValueForKey:@"buildMessages"];
}
@dynamic symbolsViewController;
- (WCSymbolsViewController *)symbolsViewController {
	if (!_symbolsViewController)
		_symbolsViewController = [[WCSymbolsViewController projectNavigationViewControllerWithProject:self] retain];
	return _symbolsViewController;
}
@dynamic findInProjectViewController;
- (WCFindInProjectViewController *)findInProjectViewController {
	if (!_findInProjectViewController)
		_findInProjectViewController = [[WCFindInProjectViewController projectNavigationViewControllerWithProject:self] retain];
	return _findInProjectViewController;
}
@synthesize findInProjectViewControllerDontCreate=_findInProjectViewController;
@dynamic breakpointsViewController;
- (WCBreakpointsViewController *)breakpointsViewController {
	if (!_breakpointsViewController)
		_breakpointsViewController = [[WCBreakpointsViewController projectNavigationViewControllerWithProject:self] retain];
	return _breakpointsViewController;
}
@synthesize breakpointsViewControllerDontCreate=_breakpointsViewController;
@dynamic currentTabViewContext;
- (id <WCTabViewContext>)currentTabViewContext {
	if ([[[[NSApplication sharedApplication] keyWindow] windowController] isKindOfClass:[WCFileWindowController class]])
		return [[[NSApplication sharedApplication] keyWindow] windowController];
	return self;
}
@dynamic fileWindowControllers;
- (NSArray *)fileWindowControllers {
	NSMutableArray *retval = [NSMutableArray array];
	for (id controller in [self windowControllers]) {
		if ([controller isKindOfClass:[WCFileWindowController class]])
			[retval addObject:controller];
	}
	return [[retval copy] autorelease];
}
@dynamic projectBreakpoint;
- (WCBreakpoint *)projectBreakpoint {
	if (!_projectBreakpoint) {
		_projectBreakpoint = [[WCBreakpoint alloc] initWithLineNumber:0 file:[self projectFile]];
		[_projectBreakpoint setBreakpointType:WCBreakpointTypeProject];
		
		for (WCFile *file in [self textFiles]) {
			if ([[file allBreakpoints] count] == 0)
				continue;
			
			WCBreakpoint *parent = [WCBreakpoint breakpointWithLineNumber:0 inFile:file];
			[parent setBreakpointType:WCBreakpointTypeFile];
			
			[[_projectBreakpoint mutableChildNodes] addObject:parent];
			[[parent mutableChildNodes] addObjectsFromArray:[file allBreakpoints]];
		}
	}
	return _projectBreakpoint;
}
@synthesize statusString=_statusString;
@synthesize secondaryStatusString=_secondaryStatusString;
@synthesize buildStatus=_buildStatus;
@synthesize totalErrors=_totalErrors;
@synthesize totalWarnings=_totalWarnings;
@synthesize isClosing=_isClosing;
@dynamic romOrSavestateAlias;
- (WCAlias *)romOrSavestateAlias {
	// see if the alias is stored in our project settings
	if (_romOrSavestateAlias == nil) {
		NSData *aliasData = [[self projectSettings] objectForKey:kWCProjectSettingsRomOrSavestateAliasKey];
		if (aliasData != nil) {
			WCAlias *alias = [NSKeyedUnarchiver unarchiveObjectWithData:aliasData];
			if (alias != nil)
				[self setRomOrSavestateAlias:alias];
		}
	}
	return _romOrSavestateAlias;
}
- (void)setRomOrSavestateAlias:(WCAlias *)romOrSavestateAlias {
	if (_romOrSavestateAlias == romOrSavestateAlias)
		return;
	
	[_romOrSavestateAlias release];
	_romOrSavestateAlias = [romOrSavestateAlias retain];
}
@dynamic debuggerWindowController;
- (WCDebuggerWindowController *)debuggerWindowController {
	WCDebuggerWindowController *retval = nil;
	for (id controller in [self windowControllers]) {
		if ([controller isKindOfClass:[WCDebuggerWindowController class]]) { 
			retval = controller;
			break;
		}
	}
	
	if (retval == nil) {
		retval = [[[WCDebuggerWindowController alloc] init] autorelease];
		[self addWindowController:retval];
	}
	
	return retval;
}
@dynamic calculator;
- (RSCalculator *)calculator {
	if (_calculator == nil) {
		_calculator = [[RSCalculator alloc] initWithOwner:self breakpointSelector:@selector(handleBreakpointCallback)];
		
		[_calculator addObserver:self forKeyPath:@"programCounter" options:NSKeyValueObservingOptionNew context:(void *)self];
	}
	return _calculator;
}
@dynamic shouldAnimate;
- (BOOL)shouldAnimate {
	return ([self isBuilding]);
}
@dynamic allBreakpoints;
- (NSArray *)allBreakpoints {
	NSMutableArray *retval = [NSMutableArray array];
	
	for (WCFile *file in [self textFiles])
		[retval addObjectsFromArray:[file allBreakpoints]];
	
	return [[retval copy] autorelease];
}
@synthesize isDebugging=_isDebugging;
@synthesize currentBreakpointFile=_currentBreakpointFile;
@synthesize currentBreakpointLineNumber=_currentBreakpointLineNumber;
@synthesize programCounterFile=_programCounterFile;
@synthesize programCounterLineNumber=_programCounterLineNumber;
#pragma mark IBActions
- (IBAction)addFilesToProject:(id)sender; {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	[panel setAllowsMultipleSelection:YES];
	[panel setCanChooseDirectories:YES];
	[panel setPrompt:NS_LOCALIZED_STRING_ADD];
	_currentAddFilesToProjectViewController = [[WCAddFilesToProjectViewController alloc] initWithNibName:@"WCAddFilesToProjectView" bundle:nil];
	[panel setAccessoryView:[_currentAddFilesToProjectViewController view]];
	[panel setDelegate:self];
	
	[self setAbsoluteFilePaths:[NSSet setWithArray:[[[self projectFile] descendantNodes] valueForKeyPath:@"absolutePath"]]];
	
	[panel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
		[_currentAddFilesToProjectViewController autorelease];
		if (result == NSFileHandlingPanelCancelButton)
			return;
		
		WCFile *file = [(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] selectedRepresentedObject];
		NSUInteger index = 0;
		
		if (![file isDirectory]) {
			index = [[[file parentNode] childNodes] indexOfObject:file];
			file = [file parentNode];
		}
			
		[[WCGeneralPerformer sharedPerformer] addFileURLs:[panel URLs] toFile:file atIndex:index];
		
		// update our cached file paths
		[self setAbsoluteFilePaths:[NSSet setWithArray:[[[self projectFile] descendantNodes] valueForKeyPath:@"absolutePath"]]];
		
		[[NSNotificationCenter defaultCenter] postNotificationName:kWCProjectNumberOfFilesDidChangeNotification object:self];
		
		_currentAddFilesToProjectViewController = nil;
	}];
}

- (IBAction)newFile:(id)sender; {
	[WCNewFileWindowController presentNewFileSheetForProject:self];
}

- (IBAction)build:(id)sender; {
	if ([self isBuilding]) {
		NSBeep();
		return;
	}
	
	// check to see if the project has any unsaved files
	NSMutableArray *ufiles = [NSMutableArray array];
	for (WCFile *file in [self openFiles]) {
		if ([file hasUnsavedChanges])
			[ufiles addObject:file];
	}
	
	if ([ufiles count]) {
		WCPreferencesBuildingForUnsavedFiles choice = [[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesBuildingForUnsavedFilesKey];
		
		switch (choice) {
			case WCPreferencesBuildingForUnsavedFilesAlwaysSave:
				for (WCFile *file in ufiles)
					[file saveFile:NULL];
				break;
			case WCPreferencesBuildingForUnsavedFilesPromptBeforeSaving:
				if ([WCUnsavedFilesWindowController runModalForProject:self] == NSCancelButton) {
					[self setShouldRunAfterBuilding:NO];
					[self setShouldDebugAfterBuilding:NO];
					return;
				}
				break;
			case WCPreferencesBuildingForUnsavedFilesNeverSave:
			default:
				break;
		}
	}
	
	WCBuildTarget *bt = [self activeBuildTarget];
	
	if (![bt inputFileAlias]) {
		NSAlert *alert = [NSAlert alertWithMessageText:NSLocalizedString(@"No Input File", @"active build target has no input file alert message text") defaultButton:NSLocalizedString(@"Edit\u2026", @"Edit\u2026") alternateButton:NS_LOCALIZED_STRING_CANCEL otherButton:nil informativeTextWithFormat:NSLocalizedString(@"The active build target \"%@\" does not have an input file assigned to it. Would you like to edit the target now?", @"active build target has no input file alert informative text"),[bt name]];
		
		[alert beginSheetModalForWindow:[self windowForSheet] modalDelegate:self didEndSelector:@selector(_activeBuildTargetHasNoInputFileAlertDidEnd:code:info:) contextInfo:NULL];
		[self setShouldRunAfterBuilding:NO];
		[self setShouldDebugAfterBuilding:NO];
		return;
	}
	
	NSString *outputDirectory = nil;
	WCPreferencesBuildingLocation choice = [[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWCPreferencesBuildingLocationKey];
	
	switch (choice) {
		case WCPreferencesBuildingLocationProjectFolder: {
			NSURL *projectDirectory = [[self fileURL] URLByDeletingLastPathComponent];
			projectDirectory = [[projectDirectory URLByAppendingPathComponent:NSLocalizedString(@"build", @"build")] URLByAppendingPathComponent:[bt name]];
			
			if (![[NSFileManager defaultManager] createDirectoryAtPath:[projectDirectory path] withIntermediateDirectories:YES attributes:nil error:NULL]) {
				if (![projectDirectory checkResourceIsReachableAndReturnError:NULL]) {
					NSLog(@"could not create output directory and it does not already exist");
					[self setShouldRunAfterBuilding:NO];
					[self setShouldDebugAfterBuilding:NO];
					return;
				}
			}
			
			outputDirectory = [projectDirectory path];
		}
			break;
		case WCPreferencesBuildingLocationCustom: {
			NSURL *projectDirectory = [NSKeyedUnarchiver unarchiveObjectWithData:[[NSUserDefaults standardUserDefaults] objectForKey:kWCPreferencesBuildingCustomLocationKey]];
			projectDirectory = [[projectDirectory URLByAppendingPathComponent:[[[self displayName] stringByDeletingPathExtension] stringByAppendingFormat:@"-%@",[[self projectFile] UUID]]] URLByAppendingPathComponent:[bt name]];
			
			if (![[NSFileManager defaultManager] createDirectoryAtPath:[projectDirectory path] withIntermediateDirectories:YES attributes:nil error:NULL]) {
				if (![projectDirectory checkResourceIsReachableAndReturnError:NULL]) {
					NSLog(@"could not create output directory and it does not already exist");
					[self setShouldRunAfterBuilding:NO];
					[self setShouldDebugAfterBuilding:NO];
					return;
				}
			}
			
			outputDirectory = [projectDirectory path];
		}
			break;
		default:
			break;
	}
	
#ifdef DEBUG
    NSAssert(outputDirectory != nil, @"output directory cannot be nil!");
#endif
	
	NSString *inputFile = [[[bt inputFileAlias] URL] path];
	NSString *outputName = [bt outputName];
	NSString *outputExtension = [bt outputExtension];
	
#ifdef DEBUG
    NSAssert(outputName != nil && outputExtension != nil, @"outputName and outputExtension cannot be nil!");
#endif
	
	[self removeAllBuildMessages];
	
	NSString *outputFile = [outputDirectory stringByAppendingPathComponent:[outputName stringByAppendingPathExtension:outputExtension]];
	NSMutableArray *args = [NSMutableArray array];
	
	if ([bt generateCodeListing])
		[args addObject:@"-T"];
	if ([bt generateLabelFile])
		[args addObject:@"-L"];
	if ([bt symbolsAreCaseSensitive])
		[args addObject:@"-A"];
	if ([[bt includeDirectories] count])
		for (WCIncludeDirectory *idir in [bt includeDirectories])
			[args addObject:[NSString stringWithFormat:@"-I%@",[idir absolutePathForDisplay]]];
	if ([[bt defines] count])
		for (WCBuildDefine *def in [bt defines])
			[args addObject:[def processedDefine]];
	
	[args addObject:inputFile];
	[args addObject:outputFile];
	
	[_buildTask release];
	_buildTask = [[NSTask alloc] init];
	
	[_buildTask setLaunchPath:[[NSBundle mainBundle] pathForResource:@"spasm" ofType:@""]];
	[_buildTask setCurrentDirectoryPath:[[[self fileURL] URLByDeletingLastPathComponent] path]];
	[_buildTask setStandardOutput:[NSPipe pipe]];
	[_buildTask setStandardError:[_buildTask standardOutput]];
	[_buildTask setArguments:args];
	
#ifdef DEBUG
    NSLog(@"%@",[_buildTask arguments]);
#endif
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_readDataFromBuildTask:) name:NSFileHandleReadCompletionNotification object:[[_buildTask standardOutput] fileHandleForReading]];
	
	[[[_buildTask standardOutput] fileHandleForReading] readInBackgroundAndNotify];
	
	[self setBuildStatus:WCProjectBuildStatusBuilding];
	[self setIsBuilding:YES];
	
	@try {
		[_buildTask launch];
	}
	@catch (NSException *exception) {
		NSLog(@"exception was thrown when spasm failed");
	}
}
- (IBAction)buildAndRun:(id)sender; {
	[self setShouldRunAfterBuilding:YES];
	[self build:nil];
}
- (IBAction)buildAndDebug:(id)sender; {
	[self setShouldDebugAfterBuilding:YES];
	[self build:nil];
}

- (IBAction)editBuildTargets:(id)sender; {
	[WCBuildTargetsInfoSheetController presentBuildTargetInfoSheetControllerForProject:self];
}

- (IBAction)newGroup:(id)sender; {
	WCFile *file = [(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] selectedRepresentedObject];
	WCFile *group = [WCFile fileWithURL:[file directoryURL] name:NSLocalizedString(@"New Group", @"New Group")];
	
	if ([file isDirectory])
		[[file mutableChildNodes] insertObject:group atIndex:0];
	else
		[[[file parentNode] mutableChildNodes] insertObject:group atIndex:[[[file parentNode] childNodes] indexOfObject:file]];
	
	[(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] setSelectedRepresentedObject:group];
	[[[self projectFilesOutlineViewController] outlineView] editColumn:0 row:[[[self projectFilesOutlineViewController] outlineView] selectedRow] withEvent:nil select:YES];
}

- (IBAction)group:(id)sender; {
	NSArray *files = [(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] selectedRepresentedObjects];
	WCFile *firstFile = [files firstObject];
	WCFile *group = [WCFile fileWithURL:[firstFile directoryURL] name:NSLocalizedString(@"New Group", @"New Group")];
	
	[[[firstFile parentNode] mutableChildNodes] insertObject:group atIndex:[[[firstFile parentNode] childNodes] indexOfObject:firstFile]];
	
	[(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] moveNodes:[(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] treeNodesForRepresentedObjects:files] toIndexPath:[[(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] indexPathForRepresentedObject:group] indexPathByAddingIndex:0]];
	
	[(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] setSelectedRepresentedObject:group];
	[[[self projectFilesOutlineViewController] outlineView] collapseItem:[[[self projectFilesOutlineViewController] outlineView] itemAtRow:[[[self projectFilesOutlineViewController] outlineView] selectedRow]]];
	[[[self projectFilesOutlineViewController] outlineView] editColumn:0 row:[[[self projectFilesOutlineViewController] outlineView] selectedRow] withEvent:nil select:YES];
}

- (IBAction)ungroup:(id)sender; {
	NSArray *nodesToRemove = [(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] selectedNodes];
	//NSMutableArray *objectsToSelect = [nodesToRemove valueForKeyPath:@"@distinctUnionOfArrays.childNodes"];
	NSMutableArray *objectsToSelect = [NSMutableArray array];
	
	for (NSTreeNode *node in nodesToRemove)
		[objectsToSelect addObjectsFromArray:[node childNodes]];
	
	[(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] moveNodes:objectsToSelect toIndexPath:[[nodesToRemove firstObject] indexPath]];
	
	for (NSTreeNode *node in nodesToRemove)
		[[[node parentNode] mutableChildNodes] removeObject:node];
	
	[(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] setSelectedTreeNodes:objectsToSelect];
}

- (IBAction)delete:(id)sender {
	// we allow deleting breakpoints without confirmation, check for that first
	if ([self currentViewController] == [self breakpointsViewControllerDontCreate]) {
		if (![[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesAdvancedConfirmDeleteOfBreakpointKey]) {
			for (WCBreakpoint *breakpoint in [[self breakpointsViewController] selectedObjects])
				[[breakpoint file] removeBreakpoint:breakpoint];
			return;
		}
		
		NSArray *breakpoints = [[self breakpointsViewController] selectedObjects];
		NSAlert *alert = [NSAlert alertWithMessageText:([breakpoints count] == 1)?NSLocalizedString(@"Delete Breakpoint", @"Delete Breakpoint"):[NSString stringWithFormat:NSLocalizedString(@"Delete %lu Breakpoints", @"delete multiple breakpoints alert message"),[breakpoints count]] defaultButton:NS_LOCALIZED_STRING_DELETE alternateButton:NS_LOCALIZED_STRING_CANCEL otherButton:nil informativeTextWithFormat:NSLocalizedString(@"This operation cannot be undone.", @"This operation cannot be undone.")];
		
		[alert setShowsSuppressionButton:YES]; 
		
		[alert beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSAlert *mAlert, NSInteger result) {
			[[NSUserDefaults standardUserDefaults] setBool:([[mAlert suppressionButton] state] == NSOnState)?NO:YES forKey:kWCPreferencesAdvancedConfirmDeleteOfBreakpointKey];
			if (result != NSAlertDefaultReturn)
				return;
			
			for (WCBreakpoint *breakpoint in [[self breakpointsViewController] selectedObjects])
				[[breakpoint file] removeBreakpoint:breakpoint];
		}];
		
		return;
	}
	
	// deletes are only allowed in the files outline view, for now at least
	if ([self currentViewController] != [self projectFilesOutlineViewController]) {
		NSBeep();
		return;
	}
	
	NSArray *files = [[self currentViewController] selectedObjects];
	BOOL canDeleteWithoutAlert = YES;
	
	// we allow deletes of groups that are empty or only contain other empty groups
	for (WCFile *file in files) {
		if ([file isKindOfClass:[WCProjectFile class]]) {
			NSBeep();
			return;
		}
		else if (![file isDirectory]) {
			canDeleteWithoutAlert = NO;
			break;
		}
		else if ([[file descendantLeafNodes] count] != 0) {
			canDeleteWithoutAlert = NO;
			break;
		}
	}

	// delete the groups without confirming
	if (canDeleteWithoutAlert) {
		for (WCFile *file in files)
			[[[file parentNode] mutableChildNodes] removeObject:file];
		return;
	}
	
	// otherwise we have to confirm with the user before proceeding
	NSAlert *alert = [NSAlert alertWithMessageText:([files count] == 1)?[NSString stringWithFormat:NSLocalizedString(@"Delete \"%@\"", @"delete one file alert message text"),[[files firstObject] name]]:[NSString stringWithFormat:NSLocalizedString(@"Delete %u Files", @"delete multiple files alert message text"),[files count]] defaultButton:([files count] == 1)?NSLocalizedString(@"Remove Reference Only", @"delete one file alert default button title"):NSLocalizedString(@"Remove References Only", @"delete multiple files alert default button title") alternateButton:NS_LOCALIZED_STRING_CANCEL otherButton:NS_LOCALIZED_STRING_DELETE informativeTextWithFormat:([files count] == 1)?[NSString stringWithFormat:NSLocalizedString(@"Do you want to delete \"%@\" or only remove the reference to it? This operation cannot be undone. Unsaved changes will be lost.", @"delete one file alert informative text"),[[files firstObject] name]]:[NSString stringWithFormat:NSLocalizedString(@"Do you want to delete %u files or only remove the references to them? This operation cannot be undone. Unsaved changes will be lost.", @"delete multiple files alert informative text"),[files count]]];
	
	[alert setAlertStyle:NSCriticalAlertStyle];
	
	[alert beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSAlert *mAlert, NSInteger result) {
		if (result == NSAlertAlternateReturn)
			return;
		
		if (result == NSAlertDefaultReturn)
			[self _deleteObjects:[[self currentViewController] selectedObjects] deleteFiles:NO];
		else if (result == NSAlertOtherReturn)
			[self _deleteObjects:[[self currentViewController] selectedObjects] deleteFiles:YES];
	}];
}

- (IBAction)rename:(id)sender; {
	NSTreeNode *node = [[self projectFilesOutlineViewController] selectedNode];
	
	if ([[node representedObject] canEditName])
		[[[self projectFilesOutlineViewController] outlineView] editColumn:0 row:[[[self projectFilesOutlineViewController] outlineView] selectedRow] withEvent:nil select:YES];
}

- (IBAction)viewProject:(id)sender; {
	[self setCurrentViewController:[self projectFilesOutlineViewController]];
	[_navBarControl setSelectedIndex:[[_navBarControl selectors] indexOfObject:NSStringFromSelector(_cmd)]];
}
- (IBAction)viewBuildMessages:(id)sender; {
	if ([[NSApplication sharedApplication] keyWindow] != [[[self windowControllers] firstObject] window])
		[[[self windowControllers] firstObject] showWindow:nil];
	[self setCurrentViewController:[self buildMessagesViewController]];
	[_navBarControl setSelectedIndex:[[_navBarControl selectors] indexOfObject:NSStringFromSelector(_cmd)]];
}
- (IBAction)viewSymbols:(id)sender; {
	[self setCurrentViewController:[self symbolsViewController]];
	[_navBarControl setSelectedIndex:[[_navBarControl selectors] indexOfObject:NSStringFromSelector(_cmd)]];
}
- (IBAction)viewSearch:(id)sender; {
	[self setCurrentViewController:[self findInProjectViewController]];
	[_navBarControl setSelectedIndex:[[_navBarControl selectors] indexOfObject:NSStringFromSelector(_cmd)]];
}
- (IBAction)viewBreakpoints:(id)sender; {
	if ([[NSApplication sharedApplication] keyWindow] != [[[self windowControllers] firstObject] window])
		[[[self windowControllers] firstObject] showWindow:nil];
	[self setCurrentViewController:[self breakpointsViewController]];
	[_navBarControl setSelectedIndex:[[_navBarControl selectors] indexOfObject:NSStringFromSelector(_cmd)]];
}
- (IBAction)changeProjectView:(WCProjectNavView *)sender; {
	[self performSelector:NSSelectorFromString([[sender selectors] objectAtIndex:[sender selectedIndex]]) withObject:nil];
}

- (IBAction)nextTab:(id)sender; {
	id <WCTabViewContext> context = [self currentTabViewContext];
	NSTabView *tabView = [[context tabBarControl] tabView];
	
	// no other tabs to switch to
	if ([tabView numberOfTabViewItems] <= 1) {
		NSBeep();
		return;
	}
	
	// get the index of the currently selected tab
	NSArray *items = [[context tabBarControl] representedTabViewItems];
	NSUInteger index = [items indexOfObject:[tabView selectedTabViewItem]];
	
	// if we would go off the end of the tab view, wrap around to the first tab
	if (++index >= [tabView numberOfTabViewItems])
		index = 0;
	
	// select the new tab
	[tabView selectTabViewItem:[items objectAtIndex:index]];
}
- (IBAction)previousTab:(id)sender; {
	id <WCTabViewContext> context = [self currentTabViewContext];
	NSTabView *tabView = [[context tabBarControl] tabView];
	
	// no other tabs to switch to
	if ([tabView numberOfTabViewItems] <= 1) {
		NSBeep();
		return;
	}
	
	// get the index of the currently selected tab
	NSArray *items = [[context tabBarControl] representedTabViewItems];
	NSUInteger index = [items indexOfObject:[tabView selectedTabViewItem]];
	
	// if the first tab is selected wrap around to the last tab
	if (index == 0)
		index = [tabView numberOfTabViewItems] - 1;
	else
		--index;
	
	// select the new tab
	[tabView selectTabViewItem:[items objectAtIndex:index]];
}

- (IBAction)openInSeparateEditor:(id)sender; {
	NSArray *nodes = [[self projectFilesOutlineViewController] selectedNodes];
	
	if ([nodes count] == 1 && [[[nodes lastObject] representedObject] isTextFile])
		[self _openSeparateEditorForFile:[[nodes lastObject] representedObject]];
	else {
		for (NSTreeNode *node in nodes)
			if ([[node representedObject] isTextFile])
				[self _openSeparateEditorForFile:[node representedObject]];
	}
}

- (IBAction)showInFinder:(id)sender; {
	[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:[[[self projectFilesOutlineViewController] selectedObjects] valueForKeyPath:@"URL"]];
}
- (IBAction)openWithFinder:(id)sender; {
	for (NSTreeNode *node in [[self projectFilesOutlineViewController] selectedNodes]) {
		if ([[node representedObject] isTextFile])
			[self addFileViewControllerForFile:[node representedObject] inTabViewContext:[self currentTabViewContext]];
		else
			[[NSWorkspace sharedWorkspace] openFile:[[node representedObject] absolutePath]];
	}
}

- (IBAction)projectWindow:(id)sender; {
	[[[self windowControllers] firstObject] showWindow:nil];
}

- (IBAction)runAfterBuilding:(id)sender; {
	NSError *error;
	if (![[self calculator] loadRomOrSavestate:[[self romOrSavestateAlias] absolutePath] error:&error]) {
		if (error != NULL)
			[self presentError:error];
		return;
	}
	
	[WETransferSheetController transferFiles:[NSArray arrayWithObjects:[[_buildTask arguments] lastObject], nil] toCalculator:[self calculator] runAfterTransfer:YES];
	[[self debuggerWindowController] showWindow:nil];
}

- (IBAction)debugAfterBuilding:(id)sender; {
	NSError *error;
	if (![[self calculator] loadRomOrSavestate:[[self romOrSavestateAlias] absolutePath] error:&error]) {
		if (error != NULL)
			[self presentError:error];
		return;
	}
	
	[WETransferSheetController transferFiles:[NSArray arrayWithObjects:[[_buildTask arguments] lastObject], nil] toCalculator:[self calculator] finishedSelector:@selector(transferFinished)];
}

- (IBAction)step:(id)sender; {
	if (![self isDebugging]) {
		NSBeep();
		return;
	}
	
	[[self calculator] step];
}
- (IBAction)stepOver:(id)sender; {
	if (![self isDebugging]) {
		NSBeep();
		return;
	}
	
	[[self calculator] stepOver];
}
#pragma mark -
#pragma mark *** Private Methods ***
- (void)_addBuildMessageForString:(NSString *)string; {
#ifdef DEBUG
    NSLog(@"%@",string);
#endif
	
	NSArray *parts = [string componentsSeparatedByString:@":"];
	
	if ([parts count] == 4) {
		
		WCFile *mFile = nil;
		for (WCFile *file in [self textFiles]) {
			if ([[file absolutePath] isEqualToString:[parts firstObject]]) {
				mFile = file;
				break;
			}
		}
		
		WCBuildMessage *mMessage = nil;
		for (WCBuildMessage *message in [self buildMessages]) {
			if ([message file] == mFile) {
				mMessage = message;
				break;
			}
		}
		
		NSUInteger line = (NSUInteger)[(NSString *)[parts objectAtIndex:1] integerValue];
		NSString *message = [[parts objectAtIndex:3] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
		WCBuildMessageType type = WCBuildMessageTypeFile;
		
		if ([[parts objectAtIndex:2] rangeOfString:@"error" options:NSLiteralSearch].length)
			type = WCBuildMessageTypeError;
		else if ([[parts objectAtIndex:2] rangeOfString:@"warning" options:NSLiteralSearch].length)
			type = WCBuildMessageTypeWarning;
		
		if (!mFile) {
			/*
			if ([[parts objectAtIndex:0] rangeOfString:@"Built-in"].length) {
				mMessage = [WCBuildMessage buildMessageWithMessage:[NSString stringWithFormat:@"%@:%u: %@",[parts objectAtIndex:0],(NSUInteger)line,originalMessage] ofType:type inFile:[self projectFile] atLineNumber:0];
				[[[self rootBuildMessage] mutableChildNodes] addObject:mMessage];
			}
			 */
			return;
		}
		
		if (!mMessage) {
			mMessage = [WCBuildMessage buildMessageWithMessage:nil ofType:WCBuildMessageTypeFile inFile:mFile atLineNumber:0];
			[[self mutableBuildMessages] addObject:mMessage];
		}
		
#ifdef DEBUG
		NSAssert(mFile != nil && mMessage != nil, @"matching file for build message not found!");
#endif
		
		WCBuildMessage *nMessage = [WCBuildMessage buildMessageWithMessage:message ofType:type inFile:mFile atLineNumber:line];
		
		[[mMessage mutableChildNodes] addObject:nMessage];
		
		if (type == WCBuildMessageTypeError)
			[mFile addErrorMessage:nMessage];
		else if (type == WCBuildMessageTypeWarning) {
			if ([[NSUserDefaults standardUserDefaults] boolForKey:kWCPreferencesBuildingTreatWarningsAsErrorsKey])
				[mFile addErrorMessage:nMessage];
			else
				[mFile addWarningMessage:nMessage];
		}
	}
}

- (void)_deleteObjects:(NSArray *)objects deleteFiles:(BOOL)deleteFiles; {
	if ([[objects firstObject] isKindOfClass:[WCFile class]]) {
		if (deleteFiles) {
			// move the represented files to the trash can
			[[NSWorkspace sharedWorkspace] recycleURLs:[objects valueForKeyPath:@"descendantLeafNodes.@distinctUnionOfArrays.URL"] completionHandler:nil];
		}
		
		// close any open tabs representing the files
		for (WCFile *file in objects) {
			for (NSTabViewItem *item in [[_tabBarControl tabView] tabViewItems]) {
				if ([item identifier] == file) {
					[[_tabBarControl tabView] removeTabViewItem:item];
					break;
				}
			}
			
			//[self removeFileViewControllerForFile:file];
		}
		
		for (WCFile *file in objects) {
			WCFindInProjectResult *fResultToRemove = nil;
			for (WCFindInProjectResult *fResult in [[self findInProjectViewControllerDontCreate] findResults]) {
				if ([fResult file] == file) {
					fResultToRemove = fResult;
					break;
				}
			}
			if (fResultToRemove)
				[[[self findInProjectViewControllerDontCreate] mutableFindResults] removeObject:fResultToRemove];
		}
		
		// remove the objects from our model
		for (WCFile *file in objects)
			[[[file parentNode] mutableChildNodes] removeObject:file];
		
		// update our cached file paths
		[self setAbsoluteFilePaths:[NSSet setWithArray:[[[self projectFile] descendantNodes] valueForKeyPath:@"absolutePath"]]];
		
		[[NSNotificationCenter defaultCenter] postNotificationName:kWCProjectNumberOfFilesDidChangeNotification object:self];
	}
}

- (WCFileWindowController *)_openSeparateEditorForFile:(WCFile *)file; {
	WCFileWindowController *mController = nil;
	for (id controller in [self windowControllers]) {
		if ([controller isKindOfClass:[WCFileWindowController class]] &&
			[controller file] == file) {
			mController = controller;
			break;
		}
	}
	
	if (!mController) {
		mController = [WCFileWindowController fileWindowControllerWithFile:file];
		[self addWindowController:mController];
	}
	
	[mController showWindow:nil];
	return mController;
}

- (void)_createDockIconBadgesIfNecessary {
	if (_errorBadge == nil || _warningBadge == nil) {
		_errorBadge = [[CTBadge badgeWithColor:[[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEditorErrorLineHighlightColorKey] labelColor:[NSColor whiteColor]] retain];
		_warningBadge = [[CTBadge badgeWithColor:[NSColor colorWithCalibratedRed:0.941 green:0.741 blue:0.18 alpha:1.0] labelColor:[NSColor whiteColor]] retain];
	}
}
#pragma mark Project Settings
- (void)_updateProjectSettings; {
	[[self projectSettings] setObject:[[[self projectFilesOutlineViewController] outlineView] expandedItemUUIDs] forKey:kWCProjectSettingsProjectFilesOutlineViewExpandedItemUUIDsKey];
	[[self projectSettings] setObject:[[self windowForSheet] stringWithSavedFrame] forKey:kWCProjectSettingsProjectWindowFrameKey];
	[[self projectSettings] setObject:[NSNumber numberWithDouble:[[[_splitView subviews] firstObject] frame].size.width] forKey:kWCProjectSettingsLeftVerticalSplitViewDividerPositionKey];
	[[self projectSettings] setObject:[[[[self tabBarControl] tabView] tabViewItems] valueForKeyPath:@"identifier.UUID"] forKey:kWCProjectSettingsOpenFileUUIDsKey];
	if ([[[self tabBarControl] tabView] numberOfTabViewItems])
		[[self projectSettings] setObject:[[[[self tabBarControl] tabView] selectedTabViewItem] valueForKeyPath:@"identifier.UUID"] forKey:kWCProjectSettingsSelectedFileUUIDKey];
	if ([self romOrSavestateAlias] != nil)
		[[self projectSettings] setObject:[NSKeyedArchiver archivedDataWithRootObject:[self romOrSavestateAlias]] forKey:kWCProjectSettingsRomOrSavestateAliasKey];
	
	// file specific settings
	NSMutableDictionary *fileDict = [[self projectSettings] objectForKey:kWCProjectSettingsFileSettingsDictionaryKey];
	
	if (!fileDict) {
		fileDict = [NSMutableDictionary dictionary];
		[[self projectSettings] setObject:fileDict forKey:kWCProjectSettingsFileSettingsDictionaryKey];
	}
	
	for (WCFileWindowController *controller in [self fileWindowControllers]) {
		NSMutableDictionary *fDict = [fileDict objectForKey:[[controller file] UUID]];
		
		if (!fDict) {
			fDict = [NSMutableDictionary dictionary];
			[fileDict setObject:fDict forKey:[[controller file] UUID]];
		}
		
		// separate editor window frame
		[fDict setObject:[[controller window] stringWithSavedFrame] forKey:kWCProjectSettingsFileSettingsFileSeparateEditorWindowFrameKey];
	}
}
- (void)_applyProjectSettings; {
	// expand the right items in our files outline view
	if ([[self projectSettings] objectForKey:kWCProjectSettingsProjectFilesOutlineViewExpandedItemUUIDsKey])
		[[[self projectFilesOutlineViewController] outlineView] expandItemsWithUUIDs:[[self projectSettings] objectForKey:kWCProjectSettingsProjectFilesOutlineViewExpandedItemUUIDsKey]];
	else
		[[[self projectFilesOutlineViewController] outlineView] expandItem:[(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] treeNodeForRepresentedObject:[self projectFile]]];
	
	// restore the project window's frame
	if (![[self projectSettings] objectForKey:kWCProjectSettingsProjectWindowFrameKey])
		[[self windowForSheet] center];
	else
		[[self windowForSheet] setFrameFromString:[[self projectSettings] objectForKey:kWCProjectSettingsProjectWindowFrameKey]];
	
	// restore the position of the left vertical split view divider
	if ([[self projectSettings] objectForKey:kWCProjectSettingsLeftVerticalSplitViewDividerPositionKey])
		[_splitView setPosition:[[[self projectSettings] objectForKey:kWCProjectSettingsLeftVerticalSplitViewDividerPositionKey] doubleValue] ofDividerAtIndex:0];
	
	// restore open files
	NSArray *tFiles = [self textFiles];
	for (NSString *UUID in [[self projectSettings] objectForKey:kWCProjectSettingsOpenFileUUIDsKey]) {
		for (WCFile *file in tFiles) {
			if ([[file UUID] isEqualToString:UUID]) {
				[self addFileViewControllerForFile:file inTabViewContext:self];
				break;
			}
		}
	}
	
	// restore the selected file
	if ([[self projectSettings] objectForKey:kWCProjectSettingsSelectedFileUUIDKey]) {
		NSString *sUUID = [[self projectSettings] objectForKey:kWCProjectSettingsSelectedFileUUIDKey];
		
		for (WCFile *file in tFiles) {
			if ([[file UUID] isEqualToString:sUUID]) {
				for (NSTabViewItem *item in [[[self tabBarControl] tabView] tabViewItems]) {
					if ([item identifier] == file) {
						[[[self tabBarControl] tabView] selectTabViewItem:item];
						break;
					}
				}
				break;
			}
		}
	}
}
#pragma mark Accessors
@dynamic absoluteFilePaths;
- (NSSet *)absoluteFilePaths {
	if (!_cachedAbsoluteFilePaths)
		_cachedAbsoluteFilePaths = [[NSSet setWithArray:[[[self projectFile] descendantLeafNodes] valueForKeyPath:@"absolutePath"]] retain];
	return _cachedAbsoluteFilePaths;
}
- (void)setAbsoluteFilePaths:(NSSet *)absoluteFilePaths {
	if (_cachedAbsoluteFilePaths == absoluteFilePaths)
		return;
	
	[_cachedAbsoluteFilePaths release];
	_cachedAbsoluteFilePaths = [absoluteFilePaths retain];
}
@synthesize codeListing=_codeListing;
@synthesize shouldRunAfterBuilding=_shouldRunAfterBuilding;
@synthesize shouldDebugAfterBuilding=_shouldDebugAfterBuilding;
#pragma mark Notifications
- (void)_readDataFromBuildTask:(NSNotification *)note {	
	NSData *data = [[note userInfo] objectForKey:NSFileHandleNotificationDataItem];
	
	if ([data length] > 0) {
		NSString *message = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
		
		for (NSString *sMessage in [message componentsSeparatedByCharactersInSet:[NSCharacterSet newlineCharacterSet]])
			[self _addBuildMessageForString:sMessage];
		
		[[note object] readInBackgroundAndNotify];
	}
	else {
		[[NSNotificationCenter defaultCenter] removeObserver:self name:NSFileHandleReadCompletionNotification object:[[_buildTask standardOutput] fileHandleForReading]];
		
		[_buildTask terminate];
		
		NSData *eData = nil;
		while ((eData = [[[_buildTask standardOutput] fileHandleForReading] availableData]) && [eData length]) {
			NSString *message = [[[NSString alloc] initWithData:eData encoding:NSUTF8StringEncoding] autorelease];
			
			for (NSString *sMessage in [message componentsSeparatedByCharactersInSet:[NSCharacterSet newlineCharacterSet]])
				[self _addBuildMessageForString:sMessage];
		}
		
		NSUInteger errors = 0;
		NSUInteger warnings = 0;
		for (WCBuildMessage *m in [self buildMessages]) {
			for (WCBuildMessage *mm in [m childNodes]) {
				if ([mm messageType] == WCBuildMessageTypeError)
					errors++;
				else if ([mm messageType] == WCBuildMessageTypeWarning)
					warnings++;
			}
		}
		
		_totalErrors = errors;
		_totalWarnings = warnings;
		
		if ([[self buildMessages] count]) {
			[self _createDockIconBadgesIfNecessary];
			
			if (errors > 0 && warnings > 0) {				
				NSImage *eBadge = [_errorBadge badgeOverlayImageForValue:errors insetX:0.0 y:0.0];
				NSImage *wBadge = [_warningBadge badgeOverlayImageForValue:warnings insetX:0.0 y:CTLargeBadgeSize];
				NSImage *appImage = [[_appIcon copy] autorelease];
				
				[appImage lockFocus];
				[eBadge drawInRect:NSMakeRect(0.0, 0.0, [eBadge size].width, [eBadge size].height) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
				[wBadge drawInRect:NSMakeRect(0.0, 0.0, [wBadge size].width, [wBadge size].height) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
				[appImage unlockFocus];
				 
				[NSApp setApplicationIconImage:appImage];
				[self setBuildStatus:WCProjectBuildStatusErrorsAndWarnings];
			}
			else if (errors > 0) {
				NSImage *eBadge = [_errorBadge badgeOverlayImageForValue:errors insetX:0.0 y:0.0];
				NSImage *appImage = [[_appIcon copy] autorelease];
				
				[appImage lockFocus];
				[eBadge drawInRect:NSMakeRect(0.0, 0.0, [eBadge size].width, [eBadge size].height) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
				[appImage unlockFocus];
				
				[NSApp setApplicationIconImage:appImage];
				[self setBuildStatus:WCProjectBuildStatusFailureErrors];
			}
			else {				
				NSImage *wBadge = [_warningBadge badgeOverlayImageForValue:warnings insetX:0.0 y:0.0];
				NSImage *appImage = [[_appIcon copy] autorelease];
				
				[appImage lockFocus];
				[wBadge drawInRect:NSMakeRect(0.0, 0.0, [wBadge size].width, [wBadge size].height) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
				[appImage unlockFocus];
				
				[NSApp setApplicationIconImage:appImage];
				[self setBuildStatus:WCProjectBuildStatusSuccessWarnings];
			}
			
			[self viewBuildMessages:nil];
			[[[self buildMessagesViewController] outlineView] expandItem:nil expandChildren:YES];
		}
		else {
			[NSApp setApplicationIconImage:_appIcon];
			
			if ([[self activeBuildTarget] generateCodeListing]) {
				NSString *codeListingPath = [[[[_buildTask arguments] lastObject] stringByDeletingPathExtension] stringByAppendingPathExtension:@"lst"];
				[self setCodeListing:[NSString stringWithContentsOfFile:codeListingPath encoding:NSASCIIStringEncoding error:NULL]];
			}
			else
				[self setCodeListing:nil];
			
			[self setBuildStatus:WCProjectBuildStatusSuccess];
			
			// send our build file to the emulator
			if ([self shouldRunAfterBuilding]) {
				if ([self romOrSavestateAlias] == nil) {
					NSAlert *alert = [NSAlert alertWithMessageText:NSLocalizedString(@"No Rom or Savestate Selected For Running", @"rom or savestate for running message") defaultButton:NS_LOCALIZED_STRING_CHOOSE_ELLIPSIS alternateButton:NS_LOCALIZED_STRING_CANCEL otherButton:nil informativeTextWithFormat:NSLocalizedString(@"Would you like to choose one now?", @"rom or savestate for running informative text")];
					
					[alert beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSAlert *mAlert,NSInteger result) {
						if (result != NSAlertDefaultReturn)
							return;
						
						[[mAlert window] orderOut:nil];
						
						NSOpenPanel *panel = [NSOpenPanel openPanel];
						
						[panel setAllowedFileTypes:[NSArray arrayWithObjects:kWECalculatorRomUTI,kWECalculatorSavestateUTI, nil]];
						
						[panel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
							if (result != NSFileHandlingPanelOKButton)
								return;
							
							[panel orderOut:nil];
							[self setRomOrSavestateAlias:[WCAlias aliasWithURL:[[panel URLs] lastObject]]];
							[self runAfterBuilding:nil];
						}];
					}];
				}
				else
					[self runAfterBuilding:nil];
			}
			else if ([self shouldDebugAfterBuilding]) {
				if ([self romOrSavestateAlias] == nil) {
					NSAlert *alert = [NSAlert alertWithMessageText:NSLocalizedString(@"No Rom or Savestate Selected For Debugging", @"rom or savestate for debugging message") defaultButton:NS_LOCALIZED_STRING_CHOOSE_ELLIPSIS alternateButton:NS_LOCALIZED_STRING_CANCEL otherButton:nil informativeTextWithFormat:NSLocalizedString(@"Would you like to choose one now?", @"rom or savestate for debugging informative text")];
					
					[alert beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSAlert *mAlert,NSInteger result) {
						if (result != NSAlertDefaultReturn)
							return;
						
						[[mAlert window] orderOut:nil];
						
						NSOpenPanel *panel = [NSOpenPanel openPanel];
						
						[panel setAllowedFileTypes:[NSArray arrayWithObjects:kWECalculatorRomUTI,kWECalculatorSavestateUTI, nil]];
						
						[panel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
							if (result != NSFileHandlingPanelOKButton)
								return;
							
							[panel orderOut:nil];
							[self setRomOrSavestateAlias:[WCAlias aliasWithURL:[[panel URLs] lastObject]]];
							[self debugAfterBuilding:nil];
						}];
					}];
				}
				else
					[self debugAfterBuilding:nil];
			}
		}
		
		[self setShouldRunAfterBuilding:NO];
		[self setShouldDebugAfterBuilding:NO];
		[self setIsBuilding:NO];
	}
}

- (void)_fileHasUnsavedChanges:(NSNotification *)note {
	WCFile *file = [note object];
	
	if ([file project] == self)
		[[[self projectFilesOutlineViewController] outlineView] setNeedsDisplayInRect:[[[self projectFilesOutlineViewController] outlineView] rectOfRow:[[[self projectFilesOutlineViewController] outlineView] rowForItem:[(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] treeNodeForRepresentedObject:file]]]];
}

- (void)_fileNameDidChange:(NSNotification *)note {
	WCFile *file = [note object];
	
	if ([file project] != self)
		return;
	
	for (NSTabViewItem *item in [[_tabBarControl tabView] tabViewItems]) {
		if ([item identifier] == file) {
			[item setLabel:[file name]];
			break;
		}
	}
}

#pragma mark Callbacks
- (void)_activeBuildTargetHasNoInputFileAlertDidEnd:(NSAlert *)alert code:(NSInteger)code info:(void *)info {
	if (code != NSAlertDefaultReturn)
		return;
	
	[[alert window] close];
	
	[self editBuildTargets:nil];
}
@end
