//
//  WCTabViewContextProtocol.h
//  WabbitStudio
//
//  Created by William Towe on 4/15/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>


@class PSMTabBarControl,WCTextView;

@protocol WCTabViewContext <NSObject>
@required
- (NSWindow *)tabWindow;
- (PSMTabBarControl *)tabBarControl;
- (WCTextView *)selectedTextView;
@end
