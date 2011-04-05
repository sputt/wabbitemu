//
//  WCProjectNavigationViewController.h
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>

@class WCProject,BWAnchoredButtonBar;

@interface WCProjectNavigationViewController : NSViewController {
@private
	IBOutlet NSImageView *_splitterHandleImageView;
	IBOutlet BWAnchoredButtonBar *_buttonBar;
	
	WCProject *_project; // non retained
}
@property (readonly,nonatomic) WCProject *project;
@property (readonly,nonatomic) NSImageView *splitterHandleImageView;
@property (readonly,nonatomic) NSString *viewNibName;
@property (assign,nonatomic) id selectedObject;
@property (assign,nonatomic) NSArray *selectedObjects;

+ (id)projectNavigationViewControllerWithProject:(WCProject *)project;
- (id)initWithProject:(WCProject *)project;
@end
