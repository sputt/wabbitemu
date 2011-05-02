//
//  WCApplicationController.h
//  WabbitStudio
//
//  Created by William Towe on 4/30/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSingleton.h"


@class WELCDView;

@interface WCApplicationController : WCSingleton {
@private
    NSMutableArray *_LCDViews;
	NSTimer *_timer;
}
+ (WCApplicationController *)sharedApplicationController;

- (void)addLCDView:(WELCDView *)LCDView;
- (void)removeLCDView:(WELCDView *)LCDView;
@end
