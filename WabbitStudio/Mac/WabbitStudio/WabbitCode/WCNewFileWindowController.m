//
//  WCNewFileWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 4/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCNewFileWindowController.h"
#import "NSArray+WCExtensions.h"
#import "WCGeneralPerformer.h"
#import "WCProject.h"
#import "WCGradientBackgroundTextFieldCell.h"
#import "WCFile.h"
#import "WCFileTemplate.h"
#import "WCNewFileAccessoryViewController.h"

#import <BWToolkitFramework/BWAnchoredButtonBar.h>

@interface WCNewFileWindowController (Private)
- (id)initWithProject:(WCProject *)project;
@end

@implementation WCNewFileWindowController

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	[_templates release];
	[super dealloc];
}

- (NSString *)windowNibName {
	return @"WCNewFileWindow";
}

- (void)windowWillLoad {
	[super windowWillLoad];
	
	_templates = [[NSMutableArray alloc] init];
	
	NSURL *templatesDirectoryURL = [[NSBundle mainBundle] URLForResource:@"File Templates" withExtension:@""];
	
#ifdef DEBUG
    NSAssert(templatesDirectoryURL != nil, @"project templates directory is nil!");
#endif
	
	// add our header group first
	[_templates addObject:[WCFileTemplate objectWithName:NSLocalizedString(@"Built-in Templates", @"built-in templates header title") icon:[NSImage imageNamed:@"NSApplicationIcon"]]];
	
	// loop through top level template groups; these go in the table view at the left side of the window
	for (NSURL *gURL in [[NSFileManager defaultManager] contentsOfDirectoryAtURL:templatesDirectoryURL includingPropertiesForKeys:nil options:(NSDirectoryEnumerationSkipsHiddenFiles|NSDirectoryEnumerationSkipsSubdirectoryDescendants) error:NULL]) {
		
		WCFileTemplate *group = [WCFileTemplate templateWithURL:gURL];
		
		// loop through the actual project templates within the given group
		for (NSURL *tURL in [[NSFileManager defaultManager] contentsOfDirectoryAtURL:gURL includingPropertiesForKeys:nil options:(NSDirectoryEnumerationSkipsHiddenFiles|NSDirectoryEnumerationSkipsSubdirectoryDescendants) error:NULL]) {
			
			WCFileTemplate *template = [WCFileTemplate templateWithURL:tURL];
			
			if (template)
				[[group mutableChildNodes] addObject:template];
		}
		
		if ([group countOfChildNodes])
			[_templates addObject:group];
	}
	
	NSMutableArray *uGroups = [NSMutableArray array];
	[uGroups addObject:[WCFileTemplate objectWithName:NSLocalizedString(@"User Templates", @"user templates header title") icon:[NSImage imageNamed:NSImageNameUser]]];
	
	// loop through top level template groups; these go in the table view at the left side of the window
	for (NSURL *gURL in [[NSFileManager defaultManager] contentsOfDirectoryAtURL:[[WCGeneralPerformer sharedPerformer] userFileTemplatesURL] includingPropertiesForKeys:nil options:(NSDirectoryEnumerationSkipsHiddenFiles|NSDirectoryEnumerationSkipsSubdirectoryDescendants) error:NULL]) {
		
		WCFileTemplate *group = [WCFileTemplate templateWithURL:gURL];
		
		// loop through the actual project templates within the given group
		for (NSURL *tURL in [[NSFileManager defaultManager] contentsOfDirectoryAtURL:gURL includingPropertiesForKeys:nil options:(NSDirectoryEnumerationSkipsHiddenFiles|NSDirectoryEnumerationSkipsSubdirectoryDescendants) error:NULL]) {
			
			WCFileTemplate *template = [WCFileTemplate templateWithURL:tURL];
			
			if (template)
				[[group mutableChildNodes] addObject:template];
		}
		
		if ([group countOfChildNodes])
			[uGroups addObject:group];
	}
	
	if ([uGroups count] > 1)
		[_templates addObjectsFromArray:uGroups];
}

- (void)windowDidLoad {
	[super windowDidLoad];
	
	[_buttonBar setIsAtBottom:YES];
	[_buttonBar setIsResizable:NO];
	
	[_splitView setDelegate:self];
	
	[_tableView selectRowIndexes:[NSIndexSet indexSetWithIndex:1] byExtendingSelection:NO];
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
	WCFileTemplate *pt = [[(NSArrayController *)[tableView dataSource] arrangedObjects] objectAtIndex:row];
	
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
	WCFileTemplate *pt = [[(NSArrayController *)[tableView dataSource] arrangedObjects] objectAtIndex:row];
	
	if ([pt isTemplateHeader])
		return kWCProjectTemplateHeaderRowHeight;
	return [tableView rowHeight];
}

- (BOOL)tableView:(NSTableView *)tableView shouldSelectRow:(NSInteger)row {
	WCFileTemplate *pt = [[(NSArrayController *)[tableView dataSource] arrangedObjects] objectAtIndex:row];
	
	if ([pt isTemplateHeader])
		return NO;
	return YES;
}

@synthesize templates=_templates;
@synthesize project=_project;
@dynamic selectedTemplate;
- (WCFileTemplate *)selectedTemplate {
	return [[_templatesArrayController selectedObjects] lastObject];
}

- (IBAction)create:(id)sender; {
	[WCNewFileAccessoryViewController presentSheetForNewFileWindowController:self];
}

+ (void)presentNewFileSheetForProject:(WCProject *)project; {
	WCNewFileWindowController *controller = [[[self class] alloc] initWithProject:project];
	
	[NSApp beginSheet:[controller window] modalForWindow:[project windowForSheet] modalDelegate:controller didEndSelector:@selector(_sheetDidEnd:code:info:) contextInfo:NULL];
}

- (id)initWithProject:(WCProject *)project; {
	if (!(self = [super initWithWindowNibName:[self windowNibName]]))
		return nil;
	
	_project = project;
	
	return self;
}

- (void)_sheetDidEnd:(NSWindow *)sheet code:(NSInteger)code info:(void *)info {
	[self autorelease];
	if (code != NSOKButton)
		return;
}
@end
