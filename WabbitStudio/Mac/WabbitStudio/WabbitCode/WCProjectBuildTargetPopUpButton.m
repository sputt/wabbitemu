//
//  WCProjectBuildTargetPopUpButton.m
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectBuildTargetPopUpButton.h"
#import "WCProject.h"
#import "WCBuildTarget.h"
#import "WCAlias.h"
#import "NSPopUpButton+WCExtensions.h"
#import "WCProjectBuildTargetPopUpButtonCell.h"

@interface WCProjectBuildTargetPopUpButton ()
- (void)_setupMenu;
@end

@implementation WCProjectBuildTargetPopUpButton

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	_project = nil;
	[super dealloc];
}

#define kNSPopUpButtonRoundTexturedHeight 25.0

+ (Class)cellClass {
	return [WCProjectBuildTargetPopUpButtonCell class];
}

- (id)initWithProject:(WCProject *)project; {
	if (!(self = [super initWithFrame:NSMakeRect(0.0, 0.0, 250.0, kNSPopUpButtonRoundTexturedHeight) pullsDown:NO]))
		return nil;
	
	_project = project;
	
	[self setPreferredEdge:NSMaxXEdge];
	[[self cell] setArrowPosition:NSPopUpArrowAtBottom];
	[[self cell] setBezelStyle:NSTexturedRoundedBezelStyle];
	[self setAutoresizesSubviews:YES];
	[self setAutoresizingMask:NSViewWidthSizable];
	[self setAutoenablesItems:NO];
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_redoMenu:) name:kWCProjectActiveBuildTargetDidChangeNotification object:_project];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_redoMenu:) name:kWCProjectNumberOfBuildTargetsDidChangeNotification object:_project];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_redoMenuBuildTarget:) name:kWCBuildTargetNameDidChangeNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_redoMenuBuildTarget:) name:kWCBuildTargetInputFileDidChangeNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_redoMenuBuildTarget:) name:kWCBuildTargetOutputNameDidChangeNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_redoMenuBuildTarget:) name:kWCBuildTargetOutputTypeDidChangeNotification object:nil];
	
	
	[self _setupMenu];
		
	return self;
}

@synthesize project=_project;

- (void)_setupMenu; {
	NSMenu *menu = [[[NSMenu alloc] initWithTitle:@"WCProjectBuildTargetPopUpButtonMenu"] autorelease];
	
	for (WCBuildTarget *target in [[self project] buildTargets]) {
		NSMenuItem *item = [menu addItemWithTitle:[NSString stringWithFormat:NSLocalizedString(@"%@ | %@", @"format for project build targets pop up menu"),[target name],([target inputFileAlias] == nil)?NSLocalizedString(@"No Input File", @"No Input File"):[NSString stringWithFormat:NSLocalizedString(@"%@ \u2192 %@", @"format for build target input output string"),[[[target inputFileAlias] absolutePathForDisplay] lastPathComponent],[[target outputName] stringByAppendingPathExtension:[target outputExtension]]]] action:@selector(_changeActiveBuildTarget:) keyEquivalent:@""];
		
		[item setTarget:self];
		[item setImage:[target icon]];
		[item setRepresentedObject:target];
	}
	
	[menu addItem:[NSMenuItem separatorItem]];
	
	[menu addItemWithTitle:NSLocalizedString(@"Edit Build Targets\u2026", @"edit build targets with ellipsis") action:@selector(editBuildTargets:) keyEquivalent:@""];
	
	WCBuildTarget *activeTarget = [[self project] activeBuildTarget];
	
	[self setMenu:menu];
	[self selectItemWithRepresentedObject:activeTarget];
	[self setToolTip:([activeTarget inputFileAlias] == nil)?[NSString stringWithFormat:NSLocalizedString(@"Name: %@\nInput File: None", @"project build targets pop up button tooltip for no input file"),[activeTarget name]]:[NSString stringWithFormat:NSLocalizedString(@"Name: %@\nInput File: %@\nOutput File: %@", @"build targets pop up menu tooltip when input file is set"),[activeTarget name],[[[activeTarget inputFileAlias] absolutePathForDisplay] lastPathComponent],[[activeTarget outputName] stringByAppendingPathExtension:[activeTarget outputExtension]]]];
}

- (void)_redoMenu:(NSNotification *)note {
	[self _setupMenu];
}

- (void)_redoMenuBuildTarget:(NSNotification *)note {
	if ([[note object] project] != [self project])
		return;
	
	[self _setupMenu];
}

- (void)_changeActiveBuildTarget:(NSMenuItem *)sender {
	[[self project] setActiveBuildTarget:[sender representedObject]];
}
@end
