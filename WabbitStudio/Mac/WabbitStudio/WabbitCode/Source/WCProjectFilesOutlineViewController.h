//
//  WCProjectFilesOutlineViewController.h
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectNavigationViewController.h"

@interface WCProjectFilesOutlineViewController : WCProjectNavigationViewController <NSOutlineViewDelegate> {
@private
    IBOutlet NSOutlineView *_outlineView;

}
@property (readonly,nonatomic) NSOutlineView *outlineView;

@end
