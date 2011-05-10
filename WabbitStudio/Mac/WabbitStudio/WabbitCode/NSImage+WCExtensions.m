//
//  NSImage+WCExtensions.m
//  WabbitCode
//
//  Created by William Towe on 10/17/08.
//  Copyright 2008 Revolution Software. All rights reserved.
//

#import "NSImage+WCExtensions.h"
#import <QuartzCore/CIFilter.h>
#import <QuartzCore/CIVector.h>


@implementation NSImage (WCExtensions)
// returns a new image that is darker than the original; similar to the unsaved image icons in Xcode
// this was adapted from the Smultron (now Fraise) source code
- (NSImage *)unsavedIconFromImage; {
	NSImage *returnImage = [[[NSImage alloc] initWithSize:[self size]] autorelease];
	NSArray *array = [NSBitmapImageRep imageRepsWithData:[self TIFFRepresentation]];
	for (id item in array) {
		CIImage *coreImage = [[[CIImage alloc] initWithBitmapImageRep:item] autorelease];
		
		CIFilter *filter1 = [CIFilter filterWithName:@"CIColorControls"]; 
		[filter1 setDefaults]; 
		[filter1 setValue:coreImage forKey:@"inputImage"];  
		[filter1 setValue:[NSNumber numberWithFloat:-0.4] forKey:@"inputBrightness"];
		
		CIImage *result = [filter1 valueForKey:@"outputImage"];
		
		[returnImage addRepresentation:[NSCIImageRep imageRepWithCIImage:result]];
	}
	return returnImage;
}
// badges the receiver with the badgeImage in the given location
- (NSImage *)badgedImageWithImage:(NSImage *)badgeImage badgePosition:(WCImageBadgePosition)badgePosition; {
	NSImage *retval = [[[NSImage alloc] initWithSize:[self size]] autorelease];
	NSRect rect = NSMakeRect(0.0, 0.0, [self size].width, [self size].height);
	NSRect badgeRect = NSZeroRect;
	
	switch (badgePosition) {
		case WCImageBadgePositionUpperLeft:
			badgeRect = NSMakeRect(0.0, [self size].height - [badgeImage size].height, [badgeImage size].width, [badgeImage size].height);
			break;
		case WCImageBadgePositionUpperRight:
			badgeRect = NSMakeRect([self size].width - [badgeImage size].width, [self size].height - [badgeImage size].height, [badgeImage size].width, [badgeImage size].height);
			break;
		case WCImageBadgePositionLowerLeft:
			badgeRect = NSMakeRect(0.0, 0.0, [badgeImage size].width, [badgeImage size].height);
			break;
		case WCImageBadgePositionLowerRight:
			badgeRect = NSMakeRect([self size].width - [badgeImage size].width, 0.0, [badgeImage size].width, [badgeImage size].height);
			break;
		default:
			break;
	}
	
	[retval lockFocus];
	[self drawInRect:rect fromRect:NSZeroRect operation:NSCompositeCopy fraction:1.0 respectFlipped:YES hints:nil];
	[badgeImage drawInRect:badgeRect fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0 respectFlipped:YES hints:nil];
	[retval unlockFocus];
	
	return retval;
}
@end
