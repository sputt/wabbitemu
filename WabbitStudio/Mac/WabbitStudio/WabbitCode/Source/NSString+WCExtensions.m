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
	
	[string replaceOccurrencesOfString:kWCProjectTemplateFileNamePlaceholder withString:[[projectTemplate projectName] stringByAppendingPathExtension:@"asm"] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCProjectTemplateProjectNamePlaceholder withString:[projectTemplate projectName] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCProjectTemplateFullUserNamePlaceholder withString:NSFullUserName() options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCProjectTemplateDatePlaceholder withString:[[NSDate date] descriptionWithLocale:[NSLocale currentLocale]] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	
	NSMutableString *includeFilesString = [NSMutableString string];
	for (NSString *includeFile in [projectTemplate includeFileNames])
		[includeFilesString appendFormat:@"#import \"%@\"\n",[[includeFile stringByDeletingPathExtension] stringByAppendingPathExtension:@"inc"]];
	
	[string replaceOccurrencesOfString:kWCProjectTemplateIncludeFileNamesPlaceholder withString:includeFilesString options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	
	return [[string copy] autorelease];
}

- (NSString *)stringByReplacingPlaceholdersWithFileTemplate:(WCFileTemplate *)fileTemplate; {
	NSMutableString *string = [[self mutableCopy] autorelease];
	
	[string replaceOccurrencesOfString:kWCProjectTemplateFileNamePlaceholder withString:[fileTemplate fileName] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCProjectTemplateProjectNamePlaceholder withString:[fileTemplate projectName] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCProjectTemplateFullUserNamePlaceholder withString:NSFullUserName() options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	[string replaceOccurrencesOfString:kWCProjectTemplateDatePlaceholder withString:[[NSDate date] descriptionWithLocale:[NSLocale currentLocale]] options:NSLiteralSearch range:NSMakeRange(0, [string length])];
	
	return [[string copy] autorelease];
}
@end
