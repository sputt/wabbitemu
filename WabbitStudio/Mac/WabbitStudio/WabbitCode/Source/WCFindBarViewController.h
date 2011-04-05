//
//  WCFindBarViewController.h
//  WabbitStudio
//
//  Created by William Towe on 3/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>

@class WCTextView;

@interface WCFindBarViewController : NSViewController <NSAnimationDelegate> {
@private
	IBOutlet NSSearchField *_searchField;
	
    __weak WCTextView *_textView; // back pointer to our text view, non retained
	NSViewAnimation *_showAnimation;
	NSViewAnimation *_closeAnimation;
	NSViewAnimation *_toggleReplaceControlsAnimation;
	BOOL _replaceControlsVisible;
}
+ (void)presentFindBarForTextView:(WCTextView *)textView;

- (IBAction)closeFindBar:(id)sender;
- (IBAction)toggleReplaceControls:(id)sender;
- (IBAction)find:(id)sender;
@end
