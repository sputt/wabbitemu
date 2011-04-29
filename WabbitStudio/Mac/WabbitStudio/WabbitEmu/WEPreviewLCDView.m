//
//  WEPreviewLCDView.m
//  WabbitStudio
//
//  Created by William Towe on 4/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WEPreviewLCDView.h"


@implementation WEPreviewLCDView

- (void)commonInit {
	[super commonInit];
	[self unregisterDraggedTypes];
}

- (BOOL)acceptsFirstResponder {
	return NO;
}
@end
