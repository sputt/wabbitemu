//
//  WEApplicationDelegate.h
//  WabbitEmu Beta
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "WELCDProtocol.h"


@interface WEApplicationDelegate : NSObject <NSApplicationDelegate> {
@private
    NSMutableArray *_LCDViews;
	NSTimer *_timer;
	NSTimer *_FPSTimer;
}

- (void)addLCDView:(id <WELCDProtocol>)LCDView;
- (void)removeLCDView:(id <WELCDProtocol>)LCDView;

- (IBAction)preferences:(id)sender;
@end
