//
//  WCPreferencesWindowController.m
//  WabbitStudio
//
//  Created by William Towe on 3/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCPreferencesWindowController.h"
#import "WCTextView.h"
#import "WCFile.h"
//#import "WCRulerView.h"
#import "NoodleLineNumberView.h"
#import "WCTextStorage.h"
#import "WCSymbolScanner.h"
#import "NSUserDefaults+WCExtensions.h"
#import "WCPreferencesController.h"
#import "WCDefines.h"

NSString* const kWCPreferencesWindowControllerGeneralIdentifier = @"General";
NSString* const kWCPreferencesWindowControllerAppearanceIdentifier = @"Colors";
NSString* const kWCPreferencesWindowControllerBuildingIdentifier = @"Building";
NSString* const kWCPreferencesWindowControllerDebuggingIdentifier = @"Debugging";
NSString* const kWCPreferencesWindowControllerEditorIdentifier = @"Editor";
NSString* const kWCPreferencesWindowControllerUpdatesIdentifier = @"Updates";
NSString* const kWCPreferencesWindowControllerAdvancedIdentifier = @"Advanced";

@interface WCPreferencesWindowController (Private)

@end

@implementation WCPreferencesWindowController

+ (NSString *)nibName {
	return @"WCPreferencesWindow";
}

- (void)windowWillLoad {
	[super windowWillLoad];
	
	_file = [[WCFile alloc] initWithURL:[[NSBundle mainBundle] URLForResource:@"sample_assembly" withExtension:@"z80"]];
	
}

- (void)windowDidLoad {
	[super windowDidLoad];
	
	[[_textView layoutManager] replaceTextStorage:[_file textStorage]];
	
	[(WCTextView *)_textView setFile:_file];
	
	NoodleLineNumberView *rulerView = [[[NoodleLineNumberView alloc] initWithScrollView:_scrollView] autorelease];
	
	[_scrollView setVerticalRulerView:rulerView];
	[_scrollView setHasVerticalRuler:YES];
	[_scrollView setHasHorizontalRuler:NO];
	[_scrollView setRulersVisible:YES];
	
	[_textView setSelectedRange:NSMakeRange(0, 0)];
}

- (void)setupToolbar {
	[self addView:_general label:NSLocalizedString(@"General", @"general prefs title") image:[NSImage imageNamed:NSImageNamePreferencesGeneral]];
	[self addView:_appearance label:NSLocalizedString(@"Colors", @"colors prefs title") image:[NSImage imageNamed:kWCPreferencesWindowControllerAppearanceIdentifier]];
	[self addView:_building label:NSLocalizedString(@"Building", @"building prefs title") image:[NSImage imageNamed:kWCPreferencesWindowControllerBuildingIdentifier]];
	[self addView:_debugging label:NSLocalizedString(@"Debugging", @"debugging prefs title") image:[NSImage imageNamed:kWCPreferencesWindowControllerDebuggingIdentifier]];
	[self addView:_editor label:NSLocalizedString(@"Editor", @"editor prefs title") image:[NSImage imageNamed:kWCPreferencesWindowControllerEditorIdentifier]];
	[self addView:_files label:NSLocalizedString(@"Files", @"files prefs title") image:[NSImage imageNamed:NSImageNameMultipleDocuments]];
	[self addView:_hardware label:NSLocalizedString(@"Hardware", @"Hardware")];
	[self addView:_screenshots label:NSLocalizedString(@"Screenshots", @"Screenshots")];
	[self addView:_advanced label:NSLocalizedString(@"Advanced", @"advanced prefs title") image:[NSImage imageNamed:kWCPreferencesWindowControllerAdvancedIdentifier]];
}

- (IBAction)chooseEditorFont:(id)sender; {
	NSFontManager *fm = [NSFontManager sharedFontManager];
	
	[fm setDelegate:self];
	
	NSFontPanel *fp = [fm fontPanel:YES];
	
	[fp setPanelFont:[[NSUserDefaults standardUserDefaults] fontForKey:kWCPreferencesEditorFontKey] isMultiple:NO];
	[fp orderFront:nil];
}

- (IBAction)chooseCustomBuildLocation:(id)sender; {
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	[panel setCanChooseFiles:NO];
	[panel setCanChooseDirectories:YES];
	[panel setPrompt:NS_LOCALIZED_STRING_CHOOSE];
	
	[panel beginSheetModalForWindow:[self window] completionHandler:^(NSInteger result) {
		if (result != NSFileHandlingPanelOKButton)
			return;
		
		[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:[[panel URLs] lastObject]] forKey:kWCPreferencesBuildingCustomLocationKey];
	}];
}

- (IBAction)selectTextEncoding:(NSPopUpButton *)sender; {
	[[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithUnsignedInteger:(NSUInteger)[sender selectedTag]] forKey:kWCPreferencesFilesTextEncodingKey];
}

- (void)changeFont:(id)sender {
	NSFont *newFont = [(NSFontManager *)sender convertFont:[[NSUserDefaults standardUserDefaults] fontForKey:kWCPreferencesEditorFontKey]];
	
	[[NSUserDefaults standardUserDefaults] setObject:[NSKeyedArchiver archivedDataWithRootObject:newFont] forKey:kWCPreferencesEditorFontKey];
}
@end
