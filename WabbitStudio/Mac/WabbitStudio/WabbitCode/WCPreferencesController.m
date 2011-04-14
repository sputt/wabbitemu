//
//  WCPreferencesController.m
//  WabbitStudio
//
//  Created by William Towe on 3/22/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCPreferencesController.h"


NSString *const kWCPreferencesAddFilesToProjectCopyFilesKey = @"advancedAddFilesToProjectCopyFiles";
NSString *const kWCPreferencesAddFilesToProjectGroupOrFolderReferencesKey = @"advancedAddFilesToProjectGroupOrFolderReferences";
NSString *const kWCPreferencesOnStartupKey = @"generalOnStartup";
         
NSString *const kWCPreferencesUseSyntaxHighlightingKey = @"kWCPreferencesUseSyntaxHighlightingKey";
         
NSString *const kWCPreferencesDirectivesKey = @"kWCPreferencesDirectivesKey";
NSString *const kWCPreferencesDirectivesColorKey = @"kWCPreferencesDirectivesColorKey";
NSString *const kWCPreferencesOpCodesKey = @"kWCPreferencesOpCodesKey";
NSString *const kWCPreferencesOpCodesColorKey = @"kWCPreferencesOpCodesColorKey";
NSString *const kWCPreferencesConditionalsKey = @"kWCPreferencesConditionalsKey";
NSString *const kWCPreferencesConditionalsColorKey = @"kWCPreferencesConditionalsColorKey";
NSString *const kWCPreferencesRegistersKey = @"kWCPreferencesRegistersKey";
NSString *const kWCPreferencesRegistersColorKey = @"kWCPreferencesRegistersColorKey";
NSString *const kWCPreferencesPreOpsKey = @"kWCPreferencesPreOpsKey";
NSString *const kWCPreferencesPreOpsColorKey = @"kWCPreferencesPreOpsColorKey";
NSString *const kWCPreferencesStringsKey = @"kWCPreferencesStringsKey";
NSString *const kWCPreferencesStringsColorKey = @"kWCPreferencesStringsColorKey";
NSString *const kWCPreferencesNumbersKey = @"kWCPreferencesNumbersKey";
NSString *const kWCPreferencesNumbersColorKey = @"kWCPreferencesNumbersColorKey";
NSString *const kWCPreferencesHexadecimalsKey = @"kWCPreferencesHexadecimalsKey";
NSString *const kWCPreferencesHexadecimalsColorKey = @"kWCPreferencesHexadecimalsColorKey";
NSString *const kWCPreferencesBinariesKey = @"kWCPreferencesBinariesKey";
NSString *const kWCPreferencesBinariesColorKey = @"kWCPreferencesBinariesColorKey";
NSString *const kWCPreferencesCommentsKey = @"kWCPreferencesCommentsKey";
NSString *const kWCPreferencesCommentsColorKey = @"kWCPreferencesCommentsColorKey";
NSString *const kWCPreferencesMultilineCommentsColorKey = @"kWCPreferencesMultilineCommentsColorKey";
NSString *const kWCPreferencesLabelsKey = @"kWCPreferencesLabelsKey";
NSString *const kWCPreferencesLabelsColorKey = @"kWCPreferencesLabelsColorKey";
NSString *const kWCPreferencesEquatesKey = @"kWCPreferencesEquatesKey";
NSString *const kWCPreferencesEquatesColorKey = @"kWCPreferencesEquatesColorKey";
NSString *const kWCPreferencesDefinesKey = @"kWCPreferencesDefinesKey";
NSString *const kWCPreferencesDefinesColorKey = @"kWCPreferencesDefinesColorKey";
NSString *const kWCPreferencesMacrosKey = @"kWCPreferencesMacrosKey";
NSString *const kWCPreferencesMacrosColorKey = @"kWCPreferencesMacrosColorKey";
         
NSString *const kWCPreferencesCurrentLineHighlightKey = @"kWCPreferencesCurrentLineHighlightKey";
NSString *const kWCPreferencesCurrentLineHighlightColorKey = @"kWCPreferencesCurrentLineHighlightColorKey";
         
NSString *const kWCPreferencesEditorTextColorKey = @"editorTextColor";
NSString *const kWCPreferencesEditorBackgroundColorKey = @"editorBackgroundColor";
         
NSString *const kWCPreferencesEditorFontKey = @"editorFont";
         
NSString *const kWCPreferencesEditorShowEquateValueTooltipsKey = @"editorShowEquateValueTooltips";
         
NSString *const kWCPreferencesEditorAutomaticallyIndentNewLinesKey = @"editorAutomaticallyIndentNewLines";
         
NSString *const kWCPreferencesEditorDisplayErrorBadgesKey = @"editorDisplayErrorBadges";
NSString *const kWCPreferencesEditorErrorLineHighlightKey = @"editorErrorLineHighlight";
NSString *const kWCPreferencesEditorErrorLineHighlightColorKey = @"editorErrorLineHighlightColor";
NSString *const kWCPreferencesEditorDisplayWarningBadgesKey = @"editorDisplayWarningBadges";
NSString *const kWCPreferencesEditorWarningLineHighlightKey = @"editorWarningLineHighlight";
NSString *const kWCPreferencesEditorWarningLineHighlightColorKey = @"editorWarningLineHighlightColor";
NSString *const kWCPreferencesEditorWrapLinesKey = @"editorWrapLines";
         
NSString *const kWCPreferencesNewBuildTargetsCopyActiveBuildTargetKey = @"advancedNewBuildTargetsCopyActiveBuildTarget";
         
NSString *const kWCPreferencesBuildingForUnsavedFilesKey = @"buildingForUnsavedFiles";
NSString *const kWCPreferencesBuildingLocationKey = @"buildingLocation";
NSString *const kWCPreferencesBuildingCustomLocationKey = @"buildingCustomLocation";
NSString *const kWCPreferencesBuildingTreatWarningsAsErrorsKey = @"buildingTreatWarningsAsErrors";
         
NSString *const kWCPreferencesAdvancedProjectTemplatesPreferUserIncludeFilesKey = @"advancedProjectTemplatesPreferUserIncludeFiles";

@implementation WCPreferencesController
#pragma mark *** Subclass Overrides ***
+ (void)initialize {
	if ([WCPreferencesController class] != self)
		return;
	
	NSMutableDictionary *preferences = [NSMutableDictionary dictionary];
	
	// project add files to project copy files
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesAddFilesToProjectCopyFilesKey];
	// project add files to project groups or folder references
	[preferences setObject:[NSNumber numberWithUnsignedInteger:WCPreferencesAddFilesToProjectGroups] forKey:kWCPreferencesAddFilesToProjectGroupOrFolderReferencesKey];
	// on startup action
	[preferences setObject:[NSNumber numberWithUnsignedInteger:WCPreferencesOnStartupOpenMostRecentProject] forKey:kWCPreferencesOnStartupKey];
	
	// use syntax highlighting
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesUseSyntaxHighlightingKey];
	
	// directives enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesDirectivesKey];
	// directives color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor orangeColor]] forKey:kWCPreferencesDirectivesColorKey];
	// op codes enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesOpCodesKey];
	// op codes color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor blueColor]] forKey:kWCPreferencesOpCodesColorKey];
	// conditionals enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesConditionalsKey];
	// conditionals color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor cyanColor]] forKey:kWCPreferencesConditionalsColorKey];
	// registers enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesRegistersKey];
	// registers color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor redColor]] forKey:kWCPreferencesRegistersColorKey];
	// pre ops enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesPreOpsKey];
	// pre ops color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor brownColor]] forKey:kWCPreferencesPreOpsColorKey];
	// strings enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesStringsKey];
	// strings color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor purpleColor]] forKey:kWCPreferencesStringsColorKey];
	// numbers enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesNumbersKey];
	// numbers color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor blueColor]] forKey:kWCPreferencesNumbersColorKey];
	// hexadecimals enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesHexadecimalsKey];
	// hexadecimals color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor magentaColor]] forKey:kWCPreferencesHexadecimalsColorKey];
	// binaries enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesBinariesKey];
	// binaries color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor colorWithCalibratedRed:0.0 green:0.5 blue:1.0 alpha:1.0]] forKey:kWCPreferencesBinariesColorKey];
	// comments enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesCommentsKey];
	// comments color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor colorWithCalibratedRed:0.0 green:0.5 blue:0.0 alpha:1.0]] forKey:kWCPreferencesCommentsColorKey];
	// multiline comments color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor colorWithCalibratedRed:0.0 green:0.5 blue:0.0 alpha:1.0]] forKey:kWCPreferencesMultilineCommentsColorKey];
	// labels enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesLabelsKey];
	// labels color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor colorWithCalibratedRed:0.5 green:0.5 blue:0.0 alpha:1.0]] forKey:kWCPreferencesLabelsColorKey];
	// equates enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesEquatesKey];
	// equates color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor colorWithCalibratedRed:0.0 green:0.5 blue:0.5 alpha:1.0]] forKey:kWCPreferencesEquatesColorKey];
	// defines enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesDefinesKey];
	// defines color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor brownColor]] forKey:kWCPreferencesDefinesColorKey];
	// macros enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesMacrosKey];
	// macros color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor colorWithCalibratedRed:1.0 green:0.5 blue:0.5 alpha:1.0]] forKey:kWCPreferencesMacrosColorKey];
	
	// current line highlight enabled
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesCurrentLineHighlightKey];
	// current line highlight color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:0.8 alpha:1.0]] forKey:kWCPreferencesCurrentLineHighlightColorKey];
	
	// text view text color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor textColor]] forKey:kWCPreferencesEditorTextColorKey];
	// text view background color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor textBackgroundColor]]  forKey:kWCPreferencesEditorBackgroundColorKey];
	
	// text view font
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSFont userFixedPitchFontOfSize:11.0]] forKey:kWCPreferencesEditorFontKey];
	
	// text view show equate value tooltips
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesEditorShowEquateValueTooltipsKey];
	// editor automatically indent new lines
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesEditorAutomaticallyIndentNewLinesKey];
	
	// editor display error badges
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesEditorDisplayErrorBadgesKey];
	// editor error line highlight
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesEditorErrorLineHighlightKey];
	// editor error line highlight color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor redColor]] forKey:kWCPreferencesEditorErrorLineHighlightColorKey];
	
	// editor display warning badges
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesEditorDisplayWarningBadgesKey];
	// editor warning line highlight
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesEditorWarningLineHighlightKey];
	// editor warning line highlight color
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSColor orangeColor]] forKey:kWCPreferencesEditorWarningLineHighlightColorKey];
	
	// editor wrap lines
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesEditorWrapLinesKey];
	
	// new build targets copy active build target
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesNewBuildTargetsCopyActiveBuildTargetKey];
	
	// building for unsaved files
	[preferences setObject:[NSNumber numberWithUnsignedInteger:WCPreferencesBuildingForUnsavedFilesAlwaysSave] forKey:kWCPreferencesBuildingForUnsavedFilesKey];
	// building location
	[preferences setObject:[NSNumber numberWithUnsignedInteger:WCPreferencesBuildingLocationProjectFolder] forKey:kWCPreferencesBuildingLocationKey];
	[preferences setObject:[NSKeyedArchiver archivedDataWithRootObject:[NSURL fileURLWithPath:NSHomeDirectory() isDirectory:YES]] forKey:kWCPreferencesBuildingCustomLocationKey];
	
	// advanced project templates prefer user include files
	[preferences setObject:[NSNumber numberWithBool:YES] forKey:kWCPreferencesAdvancedProjectTemplatesPreferUserIncludeFilesKey];
	
	[[NSUserDefaults standardUserDefaults] registerDefaults:preferences];
}

@end
