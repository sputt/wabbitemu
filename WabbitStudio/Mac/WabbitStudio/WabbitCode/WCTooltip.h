//
//  WCTooltip.h
//  WabbitStudio
//
//  Created by William Towe on 5/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>


@interface WCTooltip : NSObject {
@private
    NSString *_string;
	NSAttributedString *_attributedString;
	NSPoint _location;
}
@property (copy,nonatomic) NSString *string;
@property (copy,nonatomic) NSAttributedString *attributedString;
@property (assign,nonatomic) NSPoint location;
@property (readonly,nonatomic) NSDictionary *attributesForTooltip;

+ (NSDictionary *)defaultTooltipAttributes;
+ (NSFont *)defaultTooltipFont;

+ (WCTooltip *)tooltipWithString:(NSString *)string atLocation:(NSPoint)location;
- (id)initWithString:(NSString *)string location:(NSPoint)location;

+ (WCTooltip *)tooltipWithAttributedString:(NSAttributedString *)attributedString atLocation:(NSPoint)location;
- (id)initWithAttributedString:(NSAttributedString *)attributedString location:(NSPoint)location;
@end
