//
//  WCProjectNavView.h
//  WabbitStudio
//
//  Created by William Towe on 4/9/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSControl.h>

@class WCProject;

@interface WCProjectNavView : NSControl {
@private
	IBOutlet WCProject *_project;
	
    NSArray *_images;
	NSUInteger _selectedIndex;
	NSArray *_selectors;
	NSArray *_tooltips;
}
@property (readonly,nonatomic) NSArray *images;
@property (readonly,nonatomic) NSArray *selectors;
@property (assign,nonatomic) NSUInteger selectedIndex;
@end
