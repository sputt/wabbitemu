//
//  WCNewProjectWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 3/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCNewProjectWindowController.h"
#import <QuartzCore/QuartzCore.h>
#import "WCLinkedView.h"
#import "NSArray+WCExtensions.h"
#import "WCGeneralPerformer.h"
#import "WCProject.h"
#import "WCProjectTemplate.h"
#import "WCGradientTextFieldCell.h"
#import "WCFile.h"
#import "WCBuildTarget.h"
#import "WCDefines.h"

#import <BWToolkitFramework/BWAnchoredButtonBar.h>

@interface WCNewProjectWindowController ()
@property (readonly,nonatomic) NSMutableArray *mutableTemplates;

- (NSUInteger)countOfTemplates;
- (id)objectInTemplatesAtIndex:(NSUInteger)index;
- (void)insertObject:(id)object inTemplatesAtIndex:(NSUInteger)index;
- (void)removeObjectFromTemplatesAtIndex:(NSUInteger)index;
@end

@implementation WCNewProjectWindowController

+ (NSSet *)keyPathsForValuesAffectingValueForKey:(NSString *)key {
	if ([key isEqualToString:@"canCreateProject"])
		return [[super keyPathsForValuesAffectingValueForKey:key] setByAddingObjectsFromSet:[NSSet setWithObjects:@"collectionViewSelectionIndexes", nil]];
	return [super keyPathsForValuesAffectingValueForKey:key];
}

- (NSString *)windowNibName {
	return @"WCNewProjectWindow";
}

- (void)windowWillLoad {
	[super windowWillLoad];
	
	[self reloadTemplates:nil];
}

- (void)windowDidLoad {
	[super windowDidLoad];
	
	[_buttonBar setIsAtBottom:YES];
	[_buttonBar setIsResizable:NO];
	
	[_splitView setDelegate:self];
	
	[_tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:1] byExtendingSelection:NO];
}

- (NSString *)windowTitle {
	return NSLocalizedString(@"New Project", @"new project window title");
}
#pragma mark NSSplitViewDelegate
- (BOOL)splitView:(NSSplitView *)splitView shouldAdjustSizeOfSubview:(NSView *)view {
	if ([splitView isVertical] && [[splitView subviews] firstObject] == view)
		return NO;
	return YES;
}

#define kWCNewProjectVerticalSplitViewLeftMin 150.0
#define kWCNewProjectVerticalSplitViewRightMin 300.0
#define kWCNewProjectHorizontalSplitViewTopMin 250.0
#define kWCNewProjectHorizontalSplitViewBottomMin 150.0

- (CGFloat)splitView:(NSSplitView *)splitView constrainMaxCoordinate:(CGFloat)proposedMaximumPosition ofSubviewAt:(NSInteger)dividerIndex {
	if ([splitView isVertical])
		return proposedMaximumPosition - kWCNewProjectVerticalSplitViewRightMin;
	return proposedMaximumPosition - kWCNewProjectHorizontalSplitViewTopMin;
}

- (CGFloat)splitView:(NSSplitView *)splitView constrainMinCoordinate:(CGFloat)proposedMinimumPosition ofSubviewAt:(NSInteger)dividerIndex {
	if ([splitView isVertical])
		return proposedMinimumPosition + kWCNewProjectVerticalSplitViewLeftMin;
	return proposedMinimumPosition + kWCNewProjectHorizontalSplitViewBottomMin;
}

- (NSRect)splitView:(NSSplitView *)splitView additionalEffectiveRectOfDividerAtIndex:(NSInteger)dividerIndex {
	return [splitView convertRect:[_splitterHandleImageView bounds] fromView:_splitterHandleImageView];
}
#pragma mark NSTableViewDelegate
- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	WCProjectTemplate *pt = [[(NSArrayController *)[tableView dataSource] arrangedObjects] objectAtIndex:row];
	
	if ([pt isTemplateHeader]) {
		[cell setIcon:[pt icon]];
		[cell setIconSize:NSMakeSize(24.0, 24.0)];
		[cell setDrawGradient:YES];
	}
	else {
		[cell setIcon:nil];
		[cell setDrawGradient:NO];
	}
}

#define kWCProjectTemplateHeaderRowHeight 32.0
- (CGFloat)tableView:(NSTableView *)tableView heightOfRow:(NSInteger)row {
	WCProjectTemplate *pt = [[(NSArrayController *)[tableView dataSource] arrangedObjects] objectAtIndex:row];
	
	if ([pt isTemplateHeader])
		return kWCProjectTemplateHeaderRowHeight;
	return [tableView rowHeight];
}

- (BOOL)tableView:(NSTableView *)tableView shouldSelectRow:(NSInteger)row {
	WCProjectTemplate *pt = [[(NSArrayController *)[tableView dataSource] arrangedObjects] objectAtIndex:row];
	
	if ([pt isTemplateHeader])
		return NO;
	return YES;
}

@synthesize templates=_templates;
@dynamic mutableTemplates;
- (NSMutableArray *)mutableTemplates {
	if (!_templates)
		_templates = [[NSMutableArray alloc] init];
	return [self mutableArrayValueForKey:@"templates"];
}
@dynamic canCreateProject;
- (BOOL)canCreateProject {

	return YES;
}

- (IBAction)createProject:(id)sender; {
	if (![self canCreateProject])
		return;
	
	NSSavePanel *panel = [NSSavePanel savePanel];
	
	[panel setRequiredFileType:kWCProjectUTI];
	//[panel setPrompt:NS_LOCALIZED_STRING_SAVE];
	
	[panel beginSheetModalForWindow:[self window] completionHandler:^(NSInteger result) {
		[panel close];
		if (result != NSFileHandlingPanelOKButton)
			return;
		
		NSError *error = nil;
		WCProjectTemplate *projectTemplate = [[_templatesArrayController selectedObjects] lastObject];
		WCProject *project = [[WCGeneralPerformer sharedPerformer] createProjectAtURL:[panel URL] withTemplate:projectTemplate error:&error];
		
		if (!project && error)
			[self presentError:error];
		
		WCBuildTarget *target = [project activeBuildTarget];
		
		[target setOutputType:[projectTemplate templateOutputType]];
		
		for (WCFile *file in [project textFiles]) {
			if ([[[file name] stringByDeletingPathExtension] isEqualToString:[projectTemplate projectName]]) {
				[target setInputFileAlias:[file alias]];
			}
		}
		
		[[self window] close];
		[NSApp stopModalWithCode:NSOKButton];
	}];
}

- (IBAction)cancel:(id)sender; {
	[[NSApp modalWindow] close];
	[NSApp stopModalWithCode:NSCancelButton];
}

- (IBAction)newProjectFromFolder:(id)sender; {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	[panel setCanChooseFiles:NO];
	[panel setCanChooseDirectories:YES];
	[panel setAllowsMultipleSelection:NO];
	[panel setPrompt:NS_LOCALIZED_STRING_CHOOSE];
	
	[panel beginSheetModalForWindow:[self window] completionHandler:^(NSInteger result){
		[panel close];
		if (result != NSFileHandlingPanelOKButton)
			return;
		
		NSError *outError = nil;
		WCProject *project = [[WCGeneralPerformer sharedPerformer] createProjectFromFolder:[[panel URLs] lastObject] error:&outError];
		
		if (!project && outError)
			[self presentError:outError];
		
		[[self window] close];
		[NSApp stopModalWithCode:NSOKButton];
	}];
}

- (IBAction)reloadTemplates:(id)sender; {	
	[[self mutableTemplates] removeAllObjects];
	
	NSURL *templatesDirectoryURL = [[NSBundle mainBundle] URLForResource:@"Project Templates" withExtension:@""];
	
#ifdef DEBUG
    NSAssert(templatesDirectoryURL != nil, @"project templates directory is nil!");
#endif
	
	// add our header group first
	[[self mutableTemplates] addObject:[WCProjectTemplate objectWithName:NSLocalizedString(@"Built-in Templates", @"built-in templates header title") icon:[NSImage imageNamed:@"NSApplicationIcon"]]];
	
	// loop through top level template groups; these go in the table view at the left side of the window
	for (NSURL *gURL in [[NSFileManager defaultManager] contentsOfDirectoryAtURL:templatesDirectoryURL includingPropertiesForKeys:nil options:(NSDirectoryEnumerationSkipsHiddenFiles|NSDirectoryEnumerationSkipsSubdirectoryDescendants) error:NULL]) {
		
		WCProjectTemplate *group = [WCProjectTemplate templateWithURL:gURL];
		
		// loop through the actual project templates within the given group
		for (NSURL *tURL in [[NSFileManager defaultManager] contentsOfDirectoryAtURL:gURL includingPropertiesForKeys:nil options:(NSDirectoryEnumerationSkipsHiddenFiles|NSDirectoryEnumerationSkipsSubdirectoryDescendants) error:NULL]) {
			
			WCProjectTemplate *template = [WCProjectTemplate templateWithURL:tURL];
			
			if (template)
				[[group mutableChildNodes] addObject:template];
		}
		
		if ([group countOfChildNodes])
			[[self mutableTemplates] addObject:group];
	}
	
	NSMutableArray *uGroups = [NSMutableArray array];
	[uGroups addObject:[WCProjectTemplate objectWithName:NSLocalizedString(@"User Templates", @"user templates header title") icon:[NSImage imageNamed:NSImageNameUser]]];
	
	// loop through top level template groups; these go in the table view at the left side of the window
	for (NSURL *gURL in [[NSFileManager defaultManager] contentsOfDirectoryAtURL:[[WCGeneralPerformer sharedPerformer] userProjectTemplatesURL] includingPropertiesForKeys:nil options:(NSDirectoryEnumerationSkipsHiddenFiles|NSDirectoryEnumerationSkipsSubdirectoryDescendants) error:NULL]) {
		
		WCProjectTemplate *group = [WCProjectTemplate templateWithURL:gURL];
		
		// loop through the actual project templates within the given group
		for (NSURL *tURL in [[NSFileManager defaultManager] contentsOfDirectoryAtURL:gURL includingPropertiesForKeys:nil options:(NSDirectoryEnumerationSkipsHiddenFiles|NSDirectoryEnumerationSkipsSubdirectoryDescendants) error:NULL]) {
			
			WCProjectTemplate *template = [WCProjectTemplate templateWithURL:tURL];
			
			if (template)
				[[group mutableChildNodes] addObject:template];
		}
		
		if ([group countOfChildNodes])
			[uGroups addObject:group];
	}
	
	if ([uGroups count] > 1)
		[[self mutableTemplates] addObjectsFromArray:uGroups];
	
	[_tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:1] byExtendingSelection:NO];
}

- (NSUInteger)countOfTemplates; {
	return [_templates count];
}
- (id)objectInTemplatesAtIndex:(NSUInteger)index; {
	return [_templates objectAtIndex:index];
}
- (void)insertObject:(id)object inTemplatesAtIndex:(NSUInteger)index; {
	if (!_templates)
		_templates = [[NSMutableArray alloc] init];
	[_templates insertObject:object atIndex:index];
}
- (void)removeObjectFromTemplatesAtIndex:(NSUInteger)index; {
	[_templates removeObjectAtIndex:index];
}
@end
