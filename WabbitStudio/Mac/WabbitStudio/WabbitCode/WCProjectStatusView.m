//
//  WCProjectStatusView.m
//  WabbitStudio
//
//  Created by William Towe on 4/26/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCProjectStatusView.h"
#import "NSBezierPath+StrokeExtensions.h"
#import "NSShadow+MCAdditions.h"
#import "NS(Attributed)String+Geometrics.h"
#import "WCDefines.h"
#import "WCProject.h"


@implementation WCProjectStatusView

- (void)dealloc {
	[self unbind:@"statusString"];
	[self unbind:@"secondaryStatusString"];
	[_statusString release];
	[_secondaryStatusString release];
	[super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect {
	NSRect frame = [self bounds];
	NSRect bounds = NSInsetRect(frame, 1.0, 1.0);
	NSRect top, bottom;
	NSDivideRect(bounds, &bottom, &top, floor(NSHeight(bounds)/2.0), NSMinYEdge);
	NSGradient *gradient = [[[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedRed:0.929 green:0.945 blue:0.882 alpha:1.0],0.0,[NSColor colorWithCalibratedRed:0.902 green:0.922 blue:0.835 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.871 green:0.894 blue:0.78 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.949 green:0.961 blue:0.878 alpha:1.0],1.0, nil] autorelease];
	
	NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:frame xRadius:3.5 yRadius:3.5];
	
	[gradient drawInBezierPath:path angle:-90.0];
	
	[[NSColor darkGrayColor] setStroke];
	[path strokeInside];
	
	NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
	[style setAlignment:NSCenterTextAlignment];
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]],NSFontAttributeName,style,NSParagraphStyleAttributeName, nil];
	NSAttributedString *attributedString = [[[NSAttributedString alloc] initWithString:[self statusString] attributes:attributes] autorelease];
	
	[attributedString drawInRect:WCCenteredRectWithSize(NSMakeSize(NSWidth(top), [attributedString size].height), top)];
	
	if ([[self secondaryStatusString] length] > 0) {
		attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSMiniControlSize]],NSFontAttributeName,style,NSParagraphStyleAttributeName, nil];
		attributedString = [[[NSAttributedString alloc] initWithString:[self secondaryStatusString] attributes:attributes] autorelease];
		[attributedString drawInRect:WCCenteredRectWithSize(NSMakeSize(NSWidth(bottom), [attributedString size].height), bottom)];
	}
}

- (id)initWithProject:(WCProject *)project; {
	if (!(self = [super initWithFrame:NSMakeRect(0.0, 0.0, 250.0, 35.0)]))
		return nil;
	
	_project = project;
	
	[self bind:@"statusString" toObject:project withKeyPath:@"statusString" options:nil];
	[self bind:@"secondaryStatusString" toObject:project withKeyPath:@"secondaryStatusString" options:nil];
	
	return self;
}

@synthesize project=_project;
@dynamic statusString;
- (NSString *)statusString {
	if ([_statusString length] == 0)
		return NSLocalizedString(@"Nothing to Report", @"Nothing to Report");
	return _statusString;
}
- (void)setStatusString:(NSString *)statusString {
	if ([_statusString isEqualToString:statusString])
		return;
	
	[_statusString release];
	_statusString = [statusString copy];
	
	[self setNeedsDisplay:YES];
}
@dynamic secondaryStatusString;
- (NSString *)secondaryStatusString {
	return _secondaryStatusString;
}
- (void)setSecondaryStatusString:(NSString *)secondaryStatusString {
	if ([_secondaryStatusString isEqualToString:secondaryStatusString])
		return;
	
	[_secondaryStatusString release];
	_secondaryStatusString = [secondaryStatusString copy];
	
	[self setNeedsDisplay:YES];
}
@end
