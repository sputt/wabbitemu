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
#import "WCIconTextFieldCell.h"
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
#import "WCBuildTargetInfoSheetController.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"
#import "WCDefines.h"
#import "WCAlias.h"
#import "WCBuildDefine.h"
#import "WCIncludeDirectory.h"
#import "NSIndexPath+WCExtensions.h"
#import "WCProjectFilesOutlineViewController.h"
#import "WCBuildMessagesViewController.h"
#import "WCSymbolsViewController.h"
#import "WCSymbol.h"
#import "WCNewFileWindowController.h"

#import <PSMTabBarControl/PSMTabBarControl.h>
#import <BWToolkitFramework/BWToolkitFramework.h>

NSString* const kWCProjectFileKey = @"file";
NSString* const kWCProjectVersionKey = @"version";
NSString* const kWCProjectBuildTargetsKey = @"buildTargets";

const CGFloat kWCProjectVersion = 1.5;

NSString* const kWCProjectDataFileName = @"project.wcodedata";
NSString* const kWCProjectWrapperExtension = @"wcodeproj";

NSString* const kWCProjectUTI = @"org.revsoft.wabbitcode.project";

NSString* const kWCProjectErrorDomain = @"org.revsoft.wabbitcode.project.error";
const NSInteger kWCProjectDataFileMovedErrorCode = 1001;
const NSInteger kWCProjectDataFileIncorrectFormatErrorCode = 1002;
const NSInteger kWCProjectDataFileOldVersionErrorCode = 1003;

NSString* const kWCProjectActiveBuildTargetDidChangeNotification = @"kWCProjectActiveBuildTargetDidChangeNotification";
NSString *const kWCProjectNumberOfFilesDidChangeNotification = @"kWCProjectNumberOfFilesDidChangeNotification";

NSString *const kWCProjectSettingsProjectFilesOutlineViewExpandedItemUUIDsKey = @"projectFilesOutlineViewExpandedItemUUIDs";
NSString *const kWCProjectSettingsProjectWindowFrameKey = @"projectWindowFrame";
NSString *const kWCProjectSettingsLeftVerticalSplitViewDividerPositionKey = @"leftVerticalSplitViewDividerPosition";

@interface WCProject (Private)
- (void)_addBuildMessageForString:(NSString *)string;

- (void)_updateProjectSettings;
- (void)_applyProjectSettings;

- (void)_deleteObjects:(NSArray *)objects deleteFiles:(BOOL)deleteFiles;
@end

@implementation WCProject
#pragma mark -
#pragma mark *** Subclass Overrides ***
- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	_currentViewController = nil;
	[_buildMessages release];
	[_projectFilesOutlineViewController release];
	[_buildMessagesViewController release];
	[_buildTargets release];
	[_buildTask release];
	[_projectSettings release];
	[_projectFile release];
	[_filesToFileViewControllers release];
	[_openFiles release];
	[super dealloc];
}

- (NSString *)windowNibName {
	return @"WCProject";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)controller {
	[super windowControllerDidLoadNib:controller];
	// Add any code here that needs to be executed once the windowController has loaded the document's window.
	
	[_tabBarControl setCanCloseOnlyTab:YES];
	[_tabBarControl setStyleNamed:@"Unified"];
	[_tabBarControl setHideForSingleTab:NO];
	[_tabBarControl setUseOverflowMenu:YES];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileHasUnsavedChanges:) name:kWCFileHasUnsavedChangesNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_symolScannerDidFinishScanning:) name:kWCSymbolScannerFinishedScanningNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileNameDidChange:) name:kWCFileNameDidChangeNotification object:nil];

	[_rightButtonBar setIsAtBottom:YES];
	[_rightButtonBar setIsResizable:NO];
	
	[self setCurrentViewController:[self projectFilesOutlineViewController]];
	
	[self _applyProjectSettings];
}

- (BOOL)hasUndoManager {
	return NO;
}
#pragma mark Reading and Writing
- (BOOL)readFromFileWrapper:(NSFileWrapper *)fileWrapper ofType:(NSString *)typeName error:(NSError **)outError {
	// we should only ever be asked to open project file types
	NSAssert([typeName isEqualToString:kWCProjectUTI], @"passed document type other than project to WCProject!");
	
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
	[self _updateProjectSettings];
	return [[WCGeneralPerformer sharedPerformer] fileWrapperForProject:self error:outError];	
}
#pragma mark IBActions
- (void)saveDocument:(id)sender {
	[super saveDocument:sender];
	
	WCFile *file = [[self selectedFileViewController] file];
	
	if (!file)
		return;
	
	NSError *error = nil;
	
	if (![file saveFile:&error] && error)
		[self presentError:error];
}
#pragma mark -
#pragma mark *** Protocol Overrides ***
#pragma mark NSSplitViewDelegate
- (BOOL)splitView:(NSSplitView *)splitView shouldAdjustSizeOfSubview:(NSView *)view {
	if ([[splitView subviews] firstObject] == view)
		return NO;
	return YES;
}

#define kWCProjectSplitViewLeftMin 150.0
#define kWCProjectSplitViewRightMin 300.0

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
	return [splitView convertRectToBase:[[(id)[self currentViewController] splitterHandleImageView] frame]];
}

#pragma mark NSUserInterfaceValidations
- (BOOL)validateUserInterfaceItem:(id<NSValidatedUserInterfaceItem>)item {
	if ([item action] == @selector(addFilesToProject:) &&
		[(id <NSObject>)item isKindOfClass:[NSMenuItem class]]) {
		
		[(NSMenuItem *)item setTitle:[NSString stringWithFormat:NSLocalizedString(@"Add Files to \"%@\"\u2026", @"project add files to project menu item title"),[self displayName]]];
		
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
		for (WCFile *file in [(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] selectedRepresentedObjects])
			if ([file isKindOfClass:[WCProjectFile class]])
				return NO;
		return YES;
	}
	return [super validateUserInterfaceItem:item];
}

#pragma mark PSMTabBarControlDelegate
- (BOOL)tabView:(NSTabView *)tabView shouldCloseTabViewItem:(NSTabViewItem *)tabViewItem; {
	return ([[self openFiles] countForObject:[tabViewItem identifier]] > 1 || ![[tabViewItem identifier] hasUnsavedChanges]);
}
- (void)tabView:(NSTabView *)tabView didCloseTabViewItem:(NSTabViewItem *)tabViewItem; {
	[self removeFileViewControllerForFile:[tabViewItem identifier]];
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
- (void)jumpToBuildMessage:(WCBuildMessage *)message; {
	WCFileViewController *controller = [self addFileViewControllerForFile:[message file]];
	WCTextView *textView = [controller textView];
	NSRange range = NSMakeRange([[message file] lineStartForBuildMessage:message], 0);
	
	[textView setSelectedRange:range];
	[textView scrollRangeToVisible:range];
}
- (void)jumpToSymbol:(WCSymbol *)symbol; {
	WCFileViewController *controller = [self addFileViewControllerForFile:[symbol file]];
	WCTextView *textView = [controller textView];
	NSRange range = [symbol symbolRange];
	
	[textView setSelectedRange:range];
	[textView scrollRangeToVisible:range];
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
}
- (void)removeObjectFromChildNodesAtIndex:(NSUInteger)index; {	
	[_buildTargets removeObjectAtIndex:index];
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
- (WCFileViewController *)addFileViewControllerForFile:(WCFile *)file; {
	WCFileViewController *controller = [self selectFileViewControllerForFile:file];
	
	if (controller)
		return controller;
	
	controller = [WCFileViewController fileViewControllerWithFile:file];
	
#ifdef DEBUG
	NSAssert(controller != nil, @"file view controller was nil!");
	NSAssert(file != nil, @"cannot add a tab without a file identifier!");
#endif
	
	NSTabViewItem *item = [[[NSTabViewItem alloc] initWithIdentifier:file] autorelease];
	[item setLabel:[file name]];
	[item setView:[controller view]];
	
	[[_tabBarControl tabView] addTabViewItem:item];
	[[_tabBarControl tabView] selectTabViewItem:item];
	
	NSMutableSet *controllers = [[self filesToFileViewControllers] objectForKey:file];
	
	if (!controllers) {
		controllers = [[[NSMutableSet alloc] initWithCapacity:1] autorelease];
		[[self filesToFileViewControllers] setObject:controllers forKey:file];
	}
	
	[controllers addObject:controller];
	[[self openFiles] addObject:file];
	
	return controller;
}

- (WCFileViewController *)selectFileViewControllerForFile:(WCFile *)file; {
	for (NSTabViewItem *item in [[_tabBarControl tabView] tabViewItems]) {
		if ([item identifier] == file) {
			[[_tabBarControl tabView] selectTabViewItem:item];
			return [[[self filesToFileViewControllers] objectForKey:file] anyObject];
		}
	}
	return nil;
}

- (void)removeFileViewControllerForFile:(WCFile *)file; {
#ifdef DEBUG
	NSAssert(file != nil, @"cannot remove a tab without a file identifier!");
#endif
	NSMutableSet *controllers = [[self filesToFileViewControllers] objectForKey:file];
	WCFileViewController *controllerToRemove = [controllers anyObject];
	
	/*
	for (WCFileViewController *controller in controllers) {
		if ([[controller view] superview] == [_tabBarControl tabView]) {
			controllerToRemove = controller;
			break;
		}
	}
	 */
	
#ifdef DEBUG
    NSAssert(controllerToRemove != nil, @"could not find a controller to remove!");
#endif
	
	if ([[self openFiles] countForObject:file] == 1)
		[[file undoManager] removeAllActions];
	
	[[file textStorage] removeLayoutManager:[[controllerToRemove textView] layoutManager]];
	[controllers removeObject:controllerToRemove];
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

@dynamic labelStrings;
- (NSSet *)labelStrings {
	NSMutableSet *retval = [NSMutableSet set];
	for (WCFile *file in [self textFiles])
		[retval unionSet:[[file symbolScanner] labelStrings]];
	return [[retval copy] autorelease];
}
@dynamic equateStrings;
- (NSSet *)equateStrings {
	NSMutableSet *retval = [NSMutableSet set];
	for (WCFile *file in [self textFiles])
		[retval unionSet:[[file symbolScanner] equateStrings]];
	return [[retval copy] autorelease];
}
@dynamic defineStrings;
- (NSSet *)defineStrings {
	NSMutableSet *retval = [NSMutableSet set];
	for (WCFile *file in [self textFiles])
		[retval unionSet:[[file symbolScanner] defineStrings]];
	return [[retval copy] autorelease];
}
@dynamic macroStrings;
- (NSSet *)macroStrings {
	NSMutableSet *retval = [NSMutableSet set];
	for (WCFile *file in [self textFiles])
		[retval unionSet:[[file symbolScanner] macroStrings]];
	return [[retval copy] autorelease];
}
@dynamic textFiles;
- (NSArray *)textFiles {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCFile *file in [[self projectFile] descendantLeafNodes]) {
		NSString *UTI = [file UTI];
		
		if ([UTI isEqualToString:kWCFileAssemblyUTI] ||
			[UTI isEqualToString:kWCFileIncludeUTI] ||
			[UTI isEqualToString:kWCFilePanicCodaImportedUTI])
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
	for (WCSymbol *symbol in [self symbols]) {
		if ([symbol symbolType] == WCSymbolLabelType)
			[retval addObject:symbol];
	}
	return [[retval copy] autorelease];
}
@dynamic equateSymbols;
- (NSArray *)equateSymbols {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCSymbol *symbol in [self symbols]) {
		if ([symbol symbolType] == WCSymbolEquateType)
			[retval addObject:symbol];
	}
	return [[retval copy] autorelease];
}
@dynamic defineSymbols;
- (NSArray *)defineSymbols {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCSymbol *symbol in [self symbols]) {
		if ([symbol symbolType] == WCSymbolDefineType)
			[retval addObject:symbol];
	}
	return [[retval copy] autorelease];
}
@dynamic macroSymbols;
- (NSArray *)macroSymbols {
	NSMutableArray *retval = [NSMutableArray array];
	for (WCSymbol *symbol in [self symbols]) {
		if ([symbol symbolType] == WCSymbolMacroType)
			[retval addObject:symbol];
	}
	return [[retval copy] autorelease];
}
@dynamic selectedFileViewController;
- (WCFileViewController *)selectedFileViewController {
	return [[[self filesToFileViewControllers] objectForKey:[[[_tabBarControl tabView] selectedTabViewItem] identifier]] anyObject];
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
@synthesize projectSettings=_projectSettings;
@synthesize isBuilding=_isBuilding;
@dynamic currentViewController;
- (WCProjectNavigationViewController *)currentViewController {
	return _currentViewController;
}
- (void)setCurrentViewController:(WCProjectNavigationViewController *)controller {
	if (_currentViewController == controller)
		return;
	
	if (!_currentViewController) {
		[[controller view] setFrameSize:[[[_splitView subviews] firstObject] frame].size];
		[_splitView replaceSubview:[[_splitView subviews] firstObject] with:[controller view]];
	}
	else {
		[[controller view] setFrameSize:[[_currentViewController view] frame].size];
		[_splitView replaceSubview:[_currentViewController view] with:[controller view]];
	}
	
	_currentViewController = controller;
	
	// to counteract the BWAnchoredButtonBar, which sets itself as the delegate
	[_splitView setDelegate:self];
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
#pragma mark IBActions
- (IBAction)addFilesToProject:(id)sender; {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	[panel setAllowsMultipleSelection:YES];
	[panel setCanChooseDirectories:YES];
	[panel setPrompt:NSLocalizedString(@"Add", @"add files to project sheet add button title")];
	[panel setAccessoryView:[[[[WCAddFilesToProjectViewController alloc] init] autorelease] view]];
	
	[panel beginSheetModalForWindow:[self windowForSheet] completionHandler:^(NSInteger result) {
		if (result == NSFileHandlingPanelCancelButton)
			return;
		
		WCFile *file = [(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] selectedRepresentedObject];
		NSUInteger index = 0;
		
		if (![file isDirectory]) {
			index = [[[file parentNode] childNodes] indexOfObject:file];
			file = [file parentNode];
		}
			
		
		[[WCGeneralPerformer sharedPerformer] addFileURLs:[panel URLs] toFile:file atIndex:index];
		
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
					return;
				}
			}
			
			outputDirectory = [projectDirectory path];
		}
			break;
		case WCPreferencesBuildingLocationCustom:
			break;
		default:
			break;
	}
	
#ifdef DEBUG
    NSAssert(outputDirectory != nil, @"output directory cannot be nil!");
#endif
	
	NSString *inputFile = [[[bt inputFileAlias] fileURL] path];
	NSString *outputName = [bt outputName];
	NSString *outputExtension = [bt outputExtension];
	
#ifdef DEBUG
    NSAssert(outputName != nil && outputExtension != nil, @"outputName and outputExtension cannot be nil!");
#endif
	
	NSString *outputFile = [outputDirectory stringByAppendingPathComponent:[outputName stringByAppendingPathExtension:outputExtension]];
	
	[self removeAllBuildMessages];
	
	NSMutableArray *args = [NSMutableArray array];
	
	if ([bt generateCodeListing])
		[args addObject:@"-T"];
	if ([bt generateLabelFile])
		[args addObject:@"-L"];
	if ([bt labelsAreCaseSensitive])
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
	
	[self setIsBuilding:YES];
	
	@try {
		[_buildTask launch];
	}
	@catch (NSException *exception) {
		NSLog(@"exception was thrown when spasm failed");
	}
}

- (IBAction)editBuildTargets:(id)sender; {
	[WCBuildTargetInfoSheetController presentBuildTargetInfoSheetControllerForProject:self];
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
	// deletes are only allowed in the files outline view, for now at least
	if ([self currentViewController] != [self projectFilesOutlineViewController]) {
		NSBeep();
		return;
	}
	
	NSArray *files = [[self currentViewController] selectedObjects];
	BOOL canDeleteWithoutAlert = YES;
	
	// we allow deletes of groups that are empty or only contain other empty groups
	for (WCFile *file in files) {
		if (![file isDirectory]) {
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
	
	// otherwise we have to confirm with the user first before proceeding
	NSAlert *alert = [NSAlert alertWithMessageText:([files count] == 1)?[NSString stringWithFormat:NSLocalizedString(@"Delete \"%@\"", @"delete one file alert message text"),[[files firstObject] name]]:[NSString stringWithFormat:NSLocalizedString(@"Delete %u Files", @"delete multiple files alert message text"),[files count]] defaultButton:([files count] == 1)?NSLocalizedString(@"Remove Reference Only", @"delete one file alert default button title"):NSLocalizedString(@"Remove References Only", @"delete multiple files alert default button title") alternateButton:NS_LOCALIZED_STRING_CANCEL otherButton:NS_LOCALIZED_STRING_DELETE informativeTextWithFormat:([files count] == 1)?[NSString stringWithFormat:NSLocalizedString(@"Do you want to delete \"%@\" or only remove the reference to it? This operation cannot be undone. Unsaved changes will be lost.", @"delete one file alert informative text"),[[files firstObject] name]]:[NSString stringWithFormat:NSLocalizedString(@"Do you want to delete %u files or only remove the references to them? This operation cannot be undone. Unsaved changes will be lost.", @"delete multiple files alert informative text"),[files count]]];
	
	[alert beginSheetModalForWindow:[self windowForSheet] modalDelegate:self didEndSelector:@selector(_deleteAlertDidEnd:code:info:) contextInfo:NULL];
}

- (IBAction)viewProject:(id)sender; {
	[self setCurrentViewController:[self projectFilesOutlineViewController]];
}
- (IBAction)viewBuildMessages:(id)sender; {
	[self setCurrentViewController:[self buildMessagesViewController]];
}
- (IBAction)viewSymbols:(id)sender; {
	[self setCurrentViewController:[self symbolsViewController]];
}
#pragma mark -
#pragma mark *** Private Methods ***
- (void)_addBuildMessageForString:(NSString *)string; {
	NSArray *parts = [string componentsSeparatedByString:@":"];
	
	if ([parts count] == 4) {
		
		WCFile *mFile = nil;
		for (WCFile *file in [self textFiles]) {
			if ([[file filePath] isEqualToString:[parts firstObject]]) {
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
		WCBuildMessageType type = 0;
		
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
			[[NSWorkspace sharedWorkspace] recycleURLs:[objects valueForKeyPath:@"URL"] completionHandler:nil];
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
		
		[self willChangeValueForKey:@"symbols"];
		// remove the objects from our model
		for (WCFile *file in objects)
			[[[file parentNode] mutableChildNodes] removeObject:file];
		[self didChangeValueForKey:@"symbols"];
		
		[[NSNotificationCenter defaultCenter] postNotificationName:kWCProjectNumberOfFilesDidChangeNotification object:self];
	}
}
#pragma mark Project Settings
- (void)_updateProjectSettings; {
	[[self projectSettings] setObject:[[[self projectFilesOutlineViewController] outlineView] expandedItemUUIDs] forKey:kWCProjectSettingsProjectFilesOutlineViewExpandedItemUUIDsKey];
	[[self projectSettings] setObject:[[self windowForSheet] stringWithSavedFrame] forKey:kWCProjectSettingsProjectWindowFrameKey];
	[[self projectSettings] setObject:[NSNumber numberWithDouble:[[[_splitView subviews] firstObject] frame].size.width] forKey:kWCProjectSettingsLeftVerticalSplitViewDividerPositionKey];
}
- (void)_applyProjectSettings; {
	// expand the right items in our files outline view
	[[[self projectFilesOutlineViewController] outlineView] expandItemsWithUUIDs:[[self projectSettings] objectForKey:kWCProjectSettingsProjectFilesOutlineViewExpandedItemUUIDsKey]];
	
	// restore the project window's frame
	if (![[self projectSettings] objectForKey:kWCProjectSettingsProjectWindowFrameKey])
		[[self windowForSheet] center];
	else
		[[self windowForSheet] setFrameFromString:[[self projectSettings] objectForKey:kWCProjectSettingsProjectWindowFrameKey]];
	
	// restore the position of the left vertical split view divider
	if ([[self projectSettings] objectForKey:kWCProjectSettingsLeftVerticalSplitViewDividerPositionKey])
		[_splitView setPosition:[[[self projectSettings] objectForKey:kWCProjectSettingsLeftVerticalSplitViewDividerPositionKey] doubleValue] ofDividerAtIndex:0];
}
#pragma mark IBActions
- (IBAction)_outlineViewDoubleClick:(id)sender; {
	NSTreeNode *node = [(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] selectedNode];
	WCFile *file = [node representedObject];
	
	if ([[self textFiles] containsObject:file])
		[self addFileViewControllerForFile:file];
	else if ([file isDirectory]) {
		if ([[[self projectFilesOutlineViewController] outlineView] isItemExpanded:node])
			[[[self projectFilesOutlineViewController] outlineView] collapseItem:node];
		else
			[[[self projectFilesOutlineViewController] outlineView] expandItem:node];
	}
}
- (IBAction)_buildMessagesOutlineViewDoubleAction:(id)sender {
	NSTreeNode *node = [(NSTreeController *)[[[self buildMessagesViewController] outlineView] dataSource] selectedNode];
	WCBuildMessage *message = [node representedObject];
	
	if ([message isLeafNode])
		[self jumpToBuildMessage:message];
	else {
		if ([[[self buildMessagesViewController] outlineView] isItemExpanded:node])
			[[[self buildMessagesViewController] outlineView] collapseItem:node];
		else
			[[[self buildMessagesViewController] outlineView] expandItem:node];
	}
}
- (IBAction)_symbolsOutlineViewDoubleAction:(id)sender {
	WCSymbol *symbol = [[(NSArrayController *)[[[self symbolsViewController] tableView] dataSource] selectedObjects] lastObject];
	
	if ([symbol isLeafNode])
		[self jumpToSymbol:symbol];
}
#pragma mark Notifications
- (void)_readDataFromBuildTask:(NSNotification *)note {	
	NSData *data = [[note userInfo] objectForKey:NSFileHandleNotificationDataItem];
	
	if ([data length]) {
		NSString *message = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
		
		for (NSString *sMessage in [message componentsSeparatedByCharactersInSet:[NSCharacterSet newlineCharacterSet]])
			[self _addBuildMessageForString:sMessage];
		
		[[note object] readInBackgroundAndNotify];
	}
	else {
		[[NSNotificationCenter defaultCenter] removeObserver:self name:NSFileHandleReadCompletionNotification object:[[_buildTask standardOutput] fileHandleForReading]];
		
		[_buildTask terminate];
		
		NSData *data = nil;
		while ((data = [[[_buildTask standardOutput] fileHandleForReading] availableData]) && [data length]) {
			NSString *message = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
			
			for (NSString *sMessage in [message componentsSeparatedByCharactersInSet:[NSCharacterSet newlineCharacterSet]])
				[self _addBuildMessageForString:sMessage];
		}
		
		[self setIsBuilding:NO];
	}
}

- (void)_fileHasUnsavedChanges:(NSNotification *)note {
	WCFile *file = [note object];
	
	if ([file project] == self)
		[[[self projectFilesOutlineViewController] outlineView] setNeedsDisplayInRect:[[[self projectFilesOutlineViewController] outlineView] rectOfRow:[[[self projectFilesOutlineViewController] outlineView] rowForItem:[(NSTreeController *)[[[self projectFilesOutlineViewController] outlineView] dataSource] treeNodeForRepresentedObject:file]]]];
}

- (void)_symolScannerDidFinishScanning:(NSNotification *)note {
	if ([[self openFiles] containsObject:[[note object] file]]) {
		[self willChangeValueForKey:@"symbols"];
		[self didChangeValueForKey:@"symbols"];
	}
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

- (void)_deleteAlertDidEnd:(NSAlert *)alert code:(NSInteger)code info:(void *)info {
	if (code == NSAlertAlternateReturn)
		return;
	
	if (code == NSAlertDefaultReturn)
		[self _deleteObjects:[[self currentViewController] selectedObjects] deleteFiles:NO];
	else if (code == NSAlertOtherReturn)
		[self _deleteObjects:[[self currentViewController] selectedObjects] deleteFiles:YES];
}
@end
