//
//  WCTooltipManager.h
//  WabbitStudio
//
//  Created by William Towe on 5/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSingleton.h"


extern const CGFloat kTooltipDelay;

@class WCTooltip,WCTooltipView;

@interface WCTooltipManager : WCSingleton {
@private
    IBOutlet NSPanel *_tooltipPanel;
	IBOutlet WCTooltipView *_tooltipView;
	
	BOOL _isShowingTooltip;
}
+ (WCTooltipManager *)sharedTooltipManager;

- (void)showTooltip:(WCTooltip *)tooltip;
- (void)hideTooltip;
@end
