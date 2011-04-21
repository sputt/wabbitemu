//
//  NSString+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 4/2/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSString+WCExtensions.h"
#import "WCProjectTemplate.h"
#import "WCFileTemplate.h"


@implementation NSString (NSString_WCExtensions)
// returns an autoreleased UUID String
+ (NSString *)UUIDString; {
	CFUUIDRef UUID = CFUUIDCreate(kCFAllocatorDefault);
	// turn out CFUUIDRef into a CFStringRef 
	CFStringRef UUIDString = CFUUIDCreateString(kCFAllocatorDefault, UUID);
	
	CFRelease(UUID);
	
	// toll free bridging is cool!
	return [(NSString *)UUIDString autorelease];
}

- (NSString *)stringByReplacingPlaceholdersWithTemplate:(WCProjectTemplate *)projectTemplate; {
	NSMutableString *string = [[self mutableCopy] autorelease];
	
	[string replaceOccurrencesOfString:kWCTemplateFileNamePlaceholder withString:[[projectTemplate projectName] stringByAppendingPathExtension:@"asm"] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCTemplateProjectNamePlaceholder withString:[projectTemplate projectName] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCTemplateFullUserNamePlaceholder withString:NSFullUserName() options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCTemplateDatePlaceholder withString:[[NSDate date] descriptionWithLocale:[NSLocale currentLocale]] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	
	NSMutableString *includeFilesString = [NSMutableString string];
	for (NSString *includeFile in [projectTemplate includeFileNames])
		[includeFilesString appendFormat:@"#import \"%@\"\n",[[includeFile stringByDeletingPathExtension] stringByAppendingPathExtension:@"inc"]];
	
	[string replaceOccurrencesOfString:kWCTemplateIncludeFileNamesPlaceholder withString:includeFilesString options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	
	return [[string copy] autorelease];
}

- (NSString *)stringByReplacingPlaceholdersWithFileTemplate:(WCFileTemplate *)fileTemplate; {
	NSMutableString *string = [[self mutableCopy] autorelease];
	
	[string replaceOccurrencesOfString:kWCTemplateFileNamePlaceholder withString:[fileTemplate fileName] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCTemplateProjectNamePlaceholder withString:[fileTemplate projectName] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCTemplateFullUserNamePlaceholder withString:NSFullUserName() options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCTemplateDatePlaceholder withString:[[NSDate date] descriptionWithLocale:[NSLocale currentLocale]] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	
	return [[string copy] autorelease];
}

- (NSString *)stringByRemovingInvalidHexDigits; {
	if (!self || [self length] == 0)
		return nil;
	
	static NSCharacterSet *allowedCharacters = nil;
	if (!allowedCharacters)
		allowedCharacters = [[NSCharacterSet characterSetWithCharactersInString:@"0123456789abcdefABCDEF"] retain];
	NSUInteger trimLength = 0, length = [self length];
	unichar buffer[length];
	
	for (NSUInteger index = 0; index < length; index++) {
		if ([allowedCharacters characterIsMember:[self characterAtIndex:index]])
			buffer[trimLength++] = [self characterAtIndex:index];
	}
	
	if (trimLength == 0)
		return nil;
	return [[[NSString alloc] initWithCharacters:buffer length:trimLength] autorelease];
}
@end
