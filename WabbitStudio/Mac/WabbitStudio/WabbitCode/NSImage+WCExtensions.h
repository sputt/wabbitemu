//
//  NSImage+WCExtensions.h
//  WabbitCode
//
//  Created by William Towe on 10/17/08.
//  Copyright 2008 Revolution Software. All rights reserved.
//

#import <AppKit/NSImage.h>


typedef enum _WCImageBadgePosition {
	WCImageBadgePositionUpperLeft,
	WCImageBadgePositionUpperRight,
	WCImageBadgePositionLowerLeft,
	WCImageBadgePositionLowerRight
	
} WCImageBadgePosition;

@interface NSImage (WCExtensions)
- (NSImage *)unsavedIconFromImage;
- (NSImage *)badgedImageWithImage:(NSImage *)badgeImage badgePosition:(WCImageBadgePosition)badgePosition;
@end
