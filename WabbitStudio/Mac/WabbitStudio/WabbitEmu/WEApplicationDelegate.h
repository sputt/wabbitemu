//
//  WEApplicationDelegate.h
//  WabbitEmu Beta
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/Foundation.h>


@class WELCDView;

@interface WEApplicationDelegate : NSObject <NSApplicationDelegate> {
@private
    NSMutableArray *_LCDViews;
	NSTimer *_timer;
}

- (void)addLCDView:(WELCDView *)LCDView;
- (void)removeLCDView:(WELCDView *)LCDView;
@end
