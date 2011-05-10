//
//  WCTooltipView.h
//  WabbitStudio
//
//  Created by William Towe on 5/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSView.h>


extern const CGFloat kTooltipLeftMargin;
extern const CGFloat kTooltipRightMargin;
extern const CGFloat kTooltipTopMargin;
extern const CGFloat kTooltipBottomMargin;

@class WCTooltip;

@interface WCTooltipView : NSView {
@private
    WCTooltip *_tooltip;
}
@property (retain,nonatomic) WCTooltip *tooltip;
@end
