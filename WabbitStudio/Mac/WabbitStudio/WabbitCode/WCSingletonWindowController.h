//
//  WCSingletonWindowController.h
//  WabbitStudio
//
//  Created by William Towe on 3/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>

// manages the singleton window controllers in the project, override windowNibName for it to work properly
@interface WCSingletonWindowController : NSWindowController {
@private
    
}
@property (readonly,nonatomic) NSString *windowTitle;

+ (id)sharedWindowController;
- (void)presentWindowController;
@end
