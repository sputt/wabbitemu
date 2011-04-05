//
//  WCSymbol.m
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCSymbol.h"
#import "WCPreferencesController.h"
#import "NSUserDefaults+WCExtensions.h"

static NSMutableDictionary *_symbolIconCache = nil;

@implementation WCSymbol

+ (void)initialize {
	if ([WCSymbol class] != self)
		return;
	
	_symbolIconCache = [[NSMutableDictionary alloc] init];
}

- (void)dealloc {
	[_symbolValue release];
	_file = nil;
    [super dealloc];
}

- (NSString *)description {
	return [NSString stringWithFormat:@"name: %@ range: %@",[self name],NSStringFromRange([self symbolRange])];
}

@synthesize symbolType=_symbolType;
@synthesize symbolValue=_symbolValue;
@synthesize file=_file;
@synthesize symbolRange=_symbolRange;

- (NSImage *)icon {
	NSString *key = [NSString stringWithFormat:@"%umedium", _symbolType];
	NSImage *icon = [_symbolIconCache objectForKey:key];
	
	if (!icon) {
		icon = [[[NSImage alloc] initWithSize:NSMakeSize(14.0, 14.0)] autorelease];
		
		static NSDictionary *attributes = nil;
		if (!attributes) {
			NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
			[style setAlignment:NSCenterTextAlignment];
			/*
			NSShadow *shadow = [[[NSShadow alloc] init] autorelease];
			[shadow setShadowOffset:NSMakeSize(1.0, -1.0)];
			[shadow setShadowBlurRadius:1.0];
			 */
			attributes = [[NSDictionary alloc] initWithObjectsAndKeys:[NSFont labelFontOfSize:[NSFont systemFontSizeForControlSize:NSMiniControlSize]], NSFontAttributeName, [NSColor whiteColor], NSForegroundColorAttributeName, style, NSParagraphStyleAttributeName,/*shadow,NSShadowAttributeName,*/ nil];
		}
		
		NSString *letter = nil;
		NSColor *color = nil;
		
		switch (_symbolType) {
			case WCSymbolLabelType:
				letter = @"L";
				color = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesLabelsColorKey];
				break;
			case WCSymbolEquateType:
				letter = @"E";
				color = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEquatesColorKey];
				break;
			case WCSymbolMacroType:
				letter = @"M";
				color = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesMacrosColorKey];
				break;
			case WCSymbolDefineType:
				letter = @"D";
				color = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesDefinesColorKey];
				break;
			default:
				break;
		}
		
		NSAttributedString *string = [[[NSAttributedString alloc] initWithString:letter attributes:attributes] autorelease];
		NSRect bounds = NSMakeRect(0.0, 0.0, [icon size].width, [icon size].height);
		NSRect title = [string boundingRectWithSize:bounds.size options:NSStringDrawingOneShot];
		NSRect draw = bounds;
		draw.size.height = title.size.height;
		draw.origin.y = bounds.origin.y + (floorf(bounds.size.height/2) - floorf(title.size.height/2));
		
		CGFloat hue, saturation, brightness, alpha;
		[color getHue:&hue saturation:&saturation brightness:&brightness alpha:&alpha];
		
		NSColor *strokeColor = [NSColor colorWithDeviceHue:hue saturation:MIN(1.0,(saturation>.04)?saturation+0.12:0.0) brightness:MAX(0.0, brightness-0.045) alpha:alpha];
		NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:bounds xRadius:3.0 yRadius:3.0];
		
		[icon lockFocus];
		[color setFill];
		[path fill];
		[strokeColor setStroke];
		[path stroke];
		[string drawInRect:draw];
		[icon unlockFocus];
		
		[_symbolIconCache setObject:icon forKey:key];
	}
	return icon;
}

@dynamic iconForContextualMenu;
- (NSImage *)iconForContextualMenu {
	NSString *key = [NSString stringWithFormat:@"%usmall", _symbolType];
	NSImage *icon = [_symbolIconCache objectForKey:key];
	
	if (!icon) {
		icon = [[[NSImage alloc] initWithSize:NSMakeSize(12.0, 12.0)] autorelease];
		
		static NSDictionary *attributes = nil;
		if (!attributes) {
			NSMutableParagraphStyle *style = [[[NSParagraphStyle defaultParagraphStyle] mutableCopy] autorelease];
			[style setAlignment:NSCenterTextAlignment];
			attributes = [[NSDictionary alloc] initWithObjectsAndKeys:[NSFont menuFontOfSize:[NSFont systemFontSizeForControlSize:NSMiniControlSize]], NSFontAttributeName, [NSColor whiteColor], NSForegroundColorAttributeName, style, NSParagraphStyleAttributeName, nil];
		}
		
		NSString *letter = nil;
		NSColor *color = nil;
		
		switch (_symbolType) {
			case WCSymbolLabelType:
				letter = @"L";
				color = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesLabelsColorKey];
				break;
			case WCSymbolEquateType:
				letter = @"E";
				color = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesEquatesColorKey];
				break;
			case WCSymbolMacroType:
				letter = @"M";
				color = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesMacrosColorKey];
				break;
			case WCSymbolDefineType:
				letter = @"D";
				color = [[NSUserDefaults standardUserDefaults] colorForKey:kWCPreferencesDefinesColorKey];
				break;
			default:
				break;
		}
		
		NSAttributedString *string = [[[NSAttributedString alloc] initWithString:letter attributes:attributes] autorelease];
		NSRect bounds = NSMakeRect(0.0, 0.0, [icon size].width, [icon size].height);
		NSRect title = [string boundingRectWithSize:bounds.size options:NSStringDrawingOneShot];
		NSRect draw = bounds;
		draw.size.height = title.size.height;
		draw.origin.y = bounds.origin.y + (floorf(bounds.size.height/2) - floorf(title.size.height/2));
		
		[icon lockFocus];
		[color setFill];
		[[NSBezierPath bezierPathWithRoundedRect:bounds xRadius:3.0 yRadius:3.0] fill];
		[string drawInRect:draw];
		[icon unlockFocus];
		
		[_symbolIconCache setObject:icon forKey:key];
	}
	return icon;
}

+ (id)symbolWithName:(NSString *)name ofType:(WCSymbolType)type inFile:(WCFile *)file withRange:(NSRange)range; {
	return [[[[self class] alloc] initWithName:name type:type file:file range:range] autorelease];
}
- (id)initWithName:(NSString *)name type:(WCSymbolType)type file:(WCFile *)file range:(NSRange)range; {
	if (!(self = [super initWithName:name]))
		return nil;
	
	_symbolType = type;
	_file = file;
	_symbolRange = range;
	
	return self;
}

- (NSComparisonResult)compareUsingSymbolRange:(WCSymbol *)symbol; {
	NSRange mRange = [self symbolRange];
	NSRange range = [symbol symbolRange];
	
	if (mRange.location < range.location)
		return NSOrderedAscending;
	return NSOrderedDescending;
}
@end
