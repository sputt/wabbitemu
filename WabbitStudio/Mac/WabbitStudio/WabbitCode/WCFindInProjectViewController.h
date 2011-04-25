//
//  WCFindInProjectViewController.h
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectNavigationViewController.h"
#import "WCDefines.h"
#import <AppKit/NSOutlineView.h>
#import <AppKit/NSSearchField.h>


@interface WCFindInProjectViewController : WCProjectNavigationViewController <NSAnimationDelegate,NSUserInterfaceValidations> {
@private
    IBOutlet NSOutlineView *_outlineView;
	IBOutlet NSSearchField *_searchField;
	IBOutlet NSTextField *_replaceField;
	
	NSMutableArray *_findResults;
	BOOL _isFinding;
	NSString *_findString;
	BOOL _useRegularExpression;
	BOOL _ignoreCase;
	WCFindScope _findScope;
	WCFindSubScope _findSubScope;
	BOOL _replaceControlsVisible;
	NSViewAnimation *_toggleReplaceControlsAnimation;
	NSString *_matchesString;
	NSString *_replaceString;
	NSUInteger _numberOfFindMatches;
	BOOL _isReplacing;
	BOOL _hasConfirmationForReplace;
	NSArray *_replacementRanges;
}
@property (readonly,nonatomic) NSOutlineView *outlineView;
@property (assign,nonatomic) BOOL isFinding;
@property (readonly,nonatomic) NSArray *findResults;
@property (readonly,nonatomic) NSMutableArray *mutableFindResults;
@property (copy,nonatomic) NSString *findString;
@property (copy,nonatomic) NSString *matchesString;
@property (assign,nonatomic) BOOL ignoreCase;
@property (assign,nonatomic) BOOL useRegularExpression;
@property (assign,nonatomic) WCFindScope findScope;
@property (assign,nonatomic) WCFindSubScope findSubScope;
@property (assign,nonatomic) BOOL replaceControlsVisible;
@property (copy,nonatomic) NSString *replaceString;
@property (assign,nonatomic) NSUInteger numberOfFindMatches;
@property (assign,nonatomic) BOOL isReplacing;
@property (retain) NSArray *replacementRanges;

- (IBAction)findInProject:(id)sender;
- (IBAction)replaceInProject:(id)sender;
- (IBAction)toggleReplaceControls:(id)sender;
- (IBAction)changeFindScope:(NSMenuItem *)sender;
- (IBAction)changeFindSubScope:(NSMenuItem *)sender;

- (IBAction)findInProjectOutlineViewSingleClick:(id)sender;
- (IBAction)findInProjectOutlineViewDoubleClick:(id)sender;
@end
