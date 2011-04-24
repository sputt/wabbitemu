//
//  WCInfoTextFieldCell.m
//  WabbitStudio
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCInfoTextFieldCell.h"


@implementation WCInfoTextFieldCell

- (NSBackgroundStyle)backgroundStyle {
	if ([self isHighlighted])
		return NSBackgroundStyleDark;
	return NSBackgroundStyleRaised;
}

@end
