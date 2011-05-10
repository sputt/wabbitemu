//
//  WCApplicationDelegate.h
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>
#import <AppKit/NSApplication.h>


@interface WCApplicationDelegate : NSObject <NSApplicationDelegate> {
@private
    
}
- (IBAction)newProject:(id)sender;
- (IBAction)preferences:(id)sender;
- (IBAction)newBlankFile:(id)sender;
@end
