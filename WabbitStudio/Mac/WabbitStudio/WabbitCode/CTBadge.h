//
//  CTBadge.h
//  CTWidgets
//
//  Created by Chad Weider on 2/14/07.
//  Written by Chad Weider.
//  
//  Released into Public Domain 4/10/08.
//  
//  Version: 2.0

#import <Cocoa/Cocoa.h>

extern const CGFloat CTLargeBadgeSize;
extern const CGFloat CTSmallBadgeSize;
extern const CGFloat CTLargeLabelSize;
extern const CGFloat CTSmallLabelSize;

@interface CTBadge : NSObject
  {
  NSColor *badgeColor;
  NSColor *labelColor;
  }

+ (CTBadge *)systemBadge;																//Classic white on red badge
+ (CTBadge *)badgeWithColor:(NSColor *)badgeColor labelColor:(NSColor *)labelColor;		//Badge of any color scheme

- (NSImage *)smallBadgeForValue:(NSUInteger)value;				   //Image to use during drag operations
- (NSImage *)smallBadgeForString:(NSString *)string;
- (NSImage *)largeBadgeForValue:(NSUInteger)value;				   //For dock icons, etc
- (NSImage *)largeBadgeForString:(NSString *)string;
- (NSImage *)badgeOfSize:(CGFloat)size forValue:(NSUInteger)value;	   //A badge of arbitrary size,
- (NSImage *)badgeOfSize:(CGFloat)size forString:(NSString *)string; //	<size> is the size in pixels of the badge
																   //	not counting the shadow effect
																   //	(image returned will be larger than <size>)

- (NSImage *)badgeOverlayImageForValue:(NSUInteger)value insetX:(CGFloat)dx y:(CGFloat)dy;		//Returns a transparent 128x128 image
- (NSImage *)badgeOverlayImageForString:(NSString *)string insetX:(CGFloat)dx y:(CGFloat)dy;	//  with Large badge inset dx/dy from the upper right
- (void)badgeApplicationDockIconWithValue:(NSUInteger)value insetX:(CGFloat)dx y:(CGFloat)dy;		//Badges the Application's icon with <value>
- (void)badgeApplicationDockIconWithString:(NSString *)string insetX:(CGFloat)dx y:(CGFloat)dy; //	and puts it on the dock

- (void)setBadgeColor:(NSColor *)theColor;					//Sets the color used on badge
- (void)setLabelColor:(NSColor *)theColor;					//Sets the color of the label

- (NSColor *)badgeColor;									//Color currently being used on the badge
- (NSColor *)labelColor;									//Color currently being used on the label

@end
