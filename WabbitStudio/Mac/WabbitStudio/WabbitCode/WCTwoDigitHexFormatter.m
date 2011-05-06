//
//  WCTwoDigitHexFormatter.m
//  WabbitStudio
//
//  Created by William Towe on 5/4/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTwoDigitHexFormatter.h"


@implementation WCTwoDigitHexFormatter

- (NSString *)stringForObjectValue:(id)object {	
	if ([object isKindOfClass:[NSNumber class]])
		return [NSString stringWithFormat:@"%02x",[object unsignedShortValue]];
	return [NSString stringWithFormat:@"%02x",[object integerValue]];
}

- (NSAttributedString *)attributedStringForObjectValue:(id)obj withDefaultAttributes:(NSDictionary *)attrs {
	NSString *string = [self stringForObjectValue:obj];
	NSMutableAttributedString *attributedString = [[[NSMutableAttributedString alloc] initWithString:string attributes:attrs] autorelease];
	
	if ([self shouldDrawWithProgramCounterAttributes]) {
		[attributedString addAttribute:NSForegroundColorAttributeName value:[NSColor redColor] range:NSMakeRange(0, [attributedString length])];
		
		if ([self cellIsHighlighted])
			[attributedString applyFontTraits:NSBoldFontMask range:NSMakeRange(0, [attributedString length])];
	}
	return attributedString;
}

@synthesize shouldDrawWithProgramCounterAttributes=_shouldDrawWithProgramCounterAttributes;
@synthesize cellIsHighlighted=_cellIsHighlighted;
@end
