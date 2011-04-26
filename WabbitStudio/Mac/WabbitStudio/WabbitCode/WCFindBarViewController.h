//
//  WCFindBarViewController.h
//  WabbitStudio
//
//  Created by William Towe on 3/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>
#import <AppKit/NSAnimation.h>
#import <AppKit/NSMenuItem.h>
#import "WCDefines.h"

@class WCTextView,WCMutableRangeArray;

@interface WCFindBarViewController : NSViewController <NSAnimationDelegate,NSUserInterfaceValidations,NSControlTextEditingDelegate> {
@private
	IBOutlet NSSearchField *_searchField;
	IBOutlet NSTextField *_replaceTextField;
	
    __weak WCTextView *_textView;
	NSViewAnimation *_showAnimation;
	NSViewAnimation *_closeAnimation;
	NSViewAnimation *_toggleReplaceControlsAnimation;
	BOOL _replaceControlsVisible;
	BOOL _useRegularExpression;
	BOOL _ignoreCase;
	BOOL _wrapAround;
	WCFindScope _findScope;
	WCFindSubScope _findSubScope;
	NSString *_matchesString;
	NSString *_findString;
	NSUInteger _numberOfMatches;
	NSString *_replaceString;
	WCMutableRangeArray *_findRanges;
}
@property (readonly,nonatomic) WCTextView *textView;
@property (readonly,nonatomic) NSSearchField *searchField;
@property (copy,nonatomic) NSString *findString;
@property (copy,nonatomic) NSString *replaceString;
@property (copy,nonatomic) NSString *matchesString;
@property (assign,nonatomic) BOOL replaceControlsVisible;
@property (assign,nonatomic) BOOL ignoreCase;
@property (assign,nonatomic) BOOL useRegularExpression;
@property (assign,nonatomic) BOOL wrapAround;
@property (assign,nonatomic) WCFindScope findScope;
@property (assign,nonatomic) WCFindSubScope findSubScope;

+ (void)presentFindBarForTextView:(WCTextView *)textView;

- (IBAction)closeFindBar:(id)sender;
- (IBAction)toggleReplaceControls:(id)sender;
- (IBAction)find:(id)sender;
- (IBAction)findNextOrPrevious:(id)sender;
- (IBAction)findNext:(id)sender;
- (IBAction)findPrevious:(id)sender;
- (IBAction)replace:(id)sender;
- (IBAction)replaceAndFind:(id)sender;
- (IBAction)replaceAll:(id)sender;
- (IBAction)changeFindScope:(NSMenuItem *)sender;
- (IBAction)changeFindSubScope:(NSMenuItem *)sender;
@end
