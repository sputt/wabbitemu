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
#import "WCProject.h"
#import "NSBezierPath+MCAdditions.h"
#import "WCFile.h"


static NSShadow *kDropShadow = nil;
static NSShadow *kInnerShadow = nil;
static NSGradient *kBackgroundGradient = nil;
static NSColor *kBorderColor = nil;

@interface WCProjectStatusView ()
@property (readonly,nonatomic) NSDictionary *defaultAttributesForStatusString;
@property (readonly,nonatomic) NSDictionary *defaultAttributesForSecondaryStatusString;
@property (readonly,nonatomic) NSDictionary *attributesForFailureStatusString;
@property (readonly,nonatomic) NSDictionary *attributesForSuccessStatusString;
@end

@implementation WCProjectStatusView

+ (void)initialize {
	if ([WCProjectStatusView class] != self)
		return;
	
	// iTunes style
	//kBackgroundGradient = [[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedRed:0.929 green:0.945 blue:0.882 alpha:1.0],0.0,[NSColor colorWithCalibratedRed:0.902 green:0.922 blue:0.835 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.871 green:0.894 blue:0.78 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.949 green:0.961 blue:0.878 alpha:1.0],1.0, nil];
	// Xcode style
	kBackgroundGradient = [[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedRed:0.957 green:0.976 blue:1.0 alpha:1.0],0.0,[NSColor colorWithCalibratedRed:0.871 green:0.894 blue:0.918 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.831 green:0.851 blue:0.867 alpha:1.0],0.5,[NSColor colorWithCalibratedRed:0.82 green:0.847 blue:0.89 alpha:1.0],1.0, nil];
	kDropShadow = [[NSShadow alloc] initWithColor:[NSColor colorWithCalibratedWhite:.863 alpha:.75] offset:NSMakeSize(0, -1.0) blurRadius:1.0];
	kInnerShadow = [[NSShadow alloc] initWithColor:[NSColor colorWithCalibratedWhite:0.0 alpha:.52] offset:NSMakeSize(0.0, -1.0) blurRadius:4.0];
	kBorderColor = [[NSColor colorWithCalibratedWhite:0.569 alpha:1.0] retain];
}

- (void)dealloc {
	[self unbind:@"isDebugging"];
	[self unbind:@"statusString"];
	[self unbind:@"secondaryStatusString"];
	[self unbind:@"buildStatus"];
	[_statusString release];
	[_secondaryStatusString release];
	[super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect {
	NSRect bounds = [self bounds];
	bounds.size.height -= 1.0;
	bounds.origin.y += 1.0;
	NSRect top, bottom;
	NSDivideRect(bounds, &bottom, &top, floor(NSHeight(bounds)/2.0), NSMinYEdge);
	
	NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:bounds xRadius:3.5 yRadius:3.5];
	
	[NSGraphicsContext saveGraphicsState];
	[kDropShadow set];
	[path fill];
	[NSGraphicsContext restoreGraphicsState];
	
	[kBackgroundGradient drawInBezierPath:path angle:-90.0];
	
	[kBorderColor setStroke];
	[path strokeInside];
	
	[path fillWithInnerShadow:kInnerShadow];
	
	if ([self isDebugging]) {
		NSAttributedString *attributedString = [[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:NSLocalizedString(@"Debugging %@", @"project status view debugging status string"),[[self project] displayName]] attributes:[self defaultAttributesForStatusString]] autorelease];
		[attributedString drawInRect:WCCenteredRectWithSize(NSMakeSize(NSWidth(top), [attributedString size].height), top)];
		
		NSAttributedString *secondaryAttributedString = [[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:NSLocalizedString(@"Hit breakpoint in \"%@\" - line %lu", @"project status view debugging secondary status string"),[[[self project] currentBreakpointFile] name],[[self project] currentBreakpointLineNumber]] attributes:[self defaultAttributesForSecondaryStatusString]] autorelease];
		[secondaryAttributedString drawInRect:WCCenteredRectWithSize(NSMakeSize(NSWidth(bottom), [attributedString size].height), bottom)];
	}
	// draw our strings normally if no build status has been set
	else if ([self buildStatus] == WCProjectBuildStatusNone) {
		NSAttributedString *attributedString = [[[NSAttributedString alloc] initWithString:[self statusString] attributes:[self defaultAttributesForStatusString]] autorelease];
		
		[attributedString drawInRect:WCCenteredRectWithSize(NSMakeSize(NSWidth(top), [attributedString size].height), top)];

		if ([[self secondaryStatusString] length] > 0) {
			NSAttributedString *secondaryAttributedString = [[[NSAttributedString alloc] initWithString:[self secondaryStatusString] attributes:[self defaultAttributesForSecondaryStatusString]] autorelease];
			[secondaryAttributedString drawInRect:WCCenteredRectWithSize(NSMakeSize(NSWidth(bottom), [attributedString size].height), bottom)];
		}
	}
	else {
		NSMutableAttributedString *statusString = [[[NSMutableAttributedString alloc] initWithString:NSLocalizedString(@"Build ", @"Build") attributes:[self defaultAttributesForStatusString]] autorelease];
		switch ([self buildStatus]) {
			case WCProjectBuildStatusErrorsAndWarnings:
			case WCProjectBuildStatusFailureErrors:
				[statusString appendAttributedString:[[[NSAttributedString alloc] initWithString:NSLocalizedString(@"Failure", @"Failure") attributes:[self attributesForFailureStatusString]] autorelease]];
				break;
			case WCProjectBuildStatusSuccess:
			case WCProjectBuildStatusSuccessWarnings:
				[statusString appendAttributedString:[[[NSAttributedString alloc] initWithString:NSLocalizedString(@"Success", @"Success") attributes:[self attributesForSuccessStatusString]] autorelease]];
				break;
			case WCProjectBuildStatusBuilding:
				[statusString setAttributedString:[[[NSAttributedString alloc] initWithString:NSLocalizedString(@"Building\u2026", @"Building with ellipsis") attributes:[self defaultAttributesForStatusString]] autorelease]];
				break;
			default:
				break;
		}
		
		NSMutableAttributedString *secondaryStatusString = [[[NSMutableAttributedString alloc] initWithString:NSLocalizedString(@"No Issues", @"No Issues") attributes:[self defaultAttributesForSecondaryStatusString]] autorelease];
		switch ([self buildStatus]) {
			case WCProjectBuildStatusErrorsAndWarnings:
				[secondaryStatusString setAttributedString:[[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:NSLocalizedString(@"%lu error(s), %lu warning(s)", @"errors and warnings secondary status string"),[[self project] totalErrors],[[self project] totalWarnings]] attributes:[self defaultAttributesForSecondaryStatusString]] autorelease]];
				break;
			case WCProjectBuildStatusFailureErrors:
				[secondaryStatusString setAttributedString:[[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:NSLocalizedString(@"%lu error(s)", @"errors secondary status string"),[[self project] totalErrors]] attributes:[self defaultAttributesForSecondaryStatusString]] autorelease]];
				break;
			case WCProjectBuildStatusSuccessWarnings:
				[secondaryStatusString setAttributedString:[[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:NSLocalizedString(@"%lu warning(s)", @"warnings secondary status string"),[[self project] totalWarnings]] attributes:[self defaultAttributesForSecondaryStatusString]] autorelease]];
				break;
			case WCProjectBuildStatusBuilding:
			case WCProjectBuildStatusSuccess:
			default:
				break;
		}
		
		[statusString drawInRect:WCCenteredRectWithSize(NSMakeSize(NSWidth(top), [statusString size].height), top)];
		[secondaryStatusString drawInRect:WCCenteredRectWithSize(NSMakeSize(NSWidth(bottom), [secondaryStatusString size].height), bottom)];
	}
}

- (id)initWithProject:(WCProject *)project; {
	if (!(self = [super initWithFrame:NSMakeRect(0.0, 0.0, 350.0, 35.0)]))
		return nil;
	
	_project = project;
	
	[self bind:@"statusString" toObject:project withKeyPath:@"statusString" options:nil];
	[self bind:@"secondaryStatusString" toObject:project withKeyPath:@"secondaryStatusString" options:nil];
	[self bind:@"buildStatus" toObject:project withKeyPath:@"buildStatus" options:nil];
	[self bind:@"isDebugging" toObject:project withKeyPath:@"isDebugging" options:nil];
	
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
@dynamic buildStatus;
- (WCProjectBuildStatus)buildStatus {
	return _buildStatus;
}
- (void)setBuildStatus:(WCProjectBuildStatus)buildStatus {
	if (_buildStatus == buildStatus)
		return;
	
	_buildStatus = buildStatus;
	
	[self setNeedsDisplay:YES];
}
@dynamic isDebugging;
- (BOOL)isDebugging {
	return _isDebugging;
}
- (void)setIsDebugging:(BOOL)isDebugging {
	if (_isDebugging == isDebugging)
		return;
	
	_isDebugging = isDebugging;
	
	[self setNeedsDisplay:YES];
}
									
- (NSDictionary *)defaultAttributesForStatusString {
	NSShadow *shadow = [[[NSShadow alloc] initWithColor:[NSColor whiteColor] offset:NSMakeSize(1.0, -1.0) blurRadius:1.0] autorelease];
	NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
	[style setAlignment:NSCenterTextAlignment];
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]],NSFontAttributeName,style,NSParagraphStyleAttributeName,shadow,NSShadowAttributeName, nil];
	return attributes;
}

- (NSDictionary *)defaultAttributesForSecondaryStatusString {
	NSShadow *shadow = [[[NSShadow alloc] initWithColor:[NSColor whiteColor] offset:NSMakeSize(1.0, -1.0) blurRadius:1.0] autorelease];
	NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
	[style setAlignment:NSCenterTextAlignment];
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSMiniControlSize]],NSFontAttributeName,style,NSParagraphStyleAttributeName,shadow,NSShadowAttributeName, nil];
	return attributes;
}

- (NSDictionary *)attributesForFailureStatusString {
	NSShadow *shadow = [[[NSShadow alloc] initWithColor:[NSColor whiteColor] offset:NSMakeSize(1.0, -1.0) blurRadius:1.0] autorelease];
	NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
	[style setAlignment:NSCenterTextAlignment];
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont boldSystemFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]],NSFontAttributeName,style,NSParagraphStyleAttributeName,shadow,NSShadowAttributeName,[NSColor colorWithCalibratedRed:1.0 green:0.0 blue:0.0 alpha:1.0],NSForegroundColorAttributeName, nil];
	return attributes;
}

- (NSDictionary *)attributesForSuccessStatusString {
	NSShadow *shadow = [[[NSShadow alloc] initWithColor:[NSColor whiteColor] offset:NSMakeSize(1.0, -1.0) blurRadius:1.0] autorelease];
	NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
	[style setAlignment:NSCenterTextAlignment];
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont boldSystemFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]],NSFontAttributeName,style,NSParagraphStyleAttributeName,shadow,NSShadowAttributeName,([[self project] totalWarnings] > 0)?[NSColor textColor]:[NSColor colorWithCalibratedRed:0.0 green:0.5 blue:0.0 alpha:1.0],NSForegroundColorAttributeName, nil];
	return attributes;
}
@end
