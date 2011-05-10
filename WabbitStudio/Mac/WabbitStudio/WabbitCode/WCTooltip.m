//
//  WCTooltip.m
//  WabbitStudio
//
//  Created by William Towe on 5/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTooltip.h"


@implementation WCTooltip

- (void)dealloc {
	[_string release];
	[_attributedString release];
    [super dealloc];
}

+ (WCTooltip *)tooltipWithString:(NSString *)string atLocation:(NSPoint)location; {
	return [[[[self class] alloc] initWithString:string location:location] autorelease];
}
- (id)initWithString:(NSString *)string location:(NSPoint)location; {
	if (!(self = [super init]))
		return nil;
	
	_string = [[string stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]] copy];
	_location = location;
	
	return self;
}

+ (WCTooltip *)tooltipWithAttributedString:(NSAttributedString *)attributedString atLocation:(NSPoint)location; {
	return [[[[self class] alloc] initWithAttributedString:attributedString location:location] autorelease];
}
- (id)initWithAttributedString:(NSAttributedString *)attributedString location:(NSPoint)location; {
	if (!(self = [super init]))
		return nil;
	
	_attributedString = [attributedString copy];
	_location = location;
	
	return self;
}

+ (NSFont *)defaultTooltipFont; {
	return [NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]];
}

+ (NSDictionary *)defaultTooltipAttributes; {
	NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
	[style setAlignment:NSCenterTextAlignment];
	return [NSDictionary dictionaryWithObjectsAndKeys:[self defaultTooltipFont],NSFontAttributeName,style,NSParagraphStyleAttributeName, nil];
}

@synthesize string=_string;
@synthesize attributedString=_attributedString;
@synthesize location=_location;
@dynamic attributesForTooltip;
- (NSDictionary *)attributesForTooltip {
	NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
	[style setAlignment:NSCenterTextAlignment];
	return [NSDictionary dictionaryWithObjectsAndKeys:[NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]],NSFontAttributeName,style,NSParagraphStyleAttributeName, nil];
}

@end
