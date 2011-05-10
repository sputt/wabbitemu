//
//  WCTemplate.m
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTemplate.h"

NSString *const kWCTemplateProjectNamePlaceholder = @"<<PROJECT_NAME>>";
NSString *const kWCTemplateFileNamePlaceholder = @"<<FILE_NAME>>";
NSString *const kWCTemplateFullUserNamePlaceholder = @"<<FULL_USER_NAME>>";
NSString *const kWCTemplateDatePlaceholder = @"<<DATE>>";
NSString *const kWCTemplateIncludeFileNamesPlaceholder = @"<<INCLUDE_FILE_NAMES>>";

@implementation WCTemplate

- (void)dealloc {
	[_projectName release];
	[_templateURL release];
	[_templateDictionary release];
	[super dealloc];
}

- (NSString *)name {
	if ([self isTemplateHeader])
		return [super name];
	return [[self templateURL] lastPathComponent];
}

+ (id)templateWithURL:(NSURL *)URL; {
	return [[[[self class] alloc] initWithURL:URL] autorelease];
}
- (id)initWithURL:(NSURL *)URL; {
	if (!(self = [super initWithName:nil]))
		return nil;
	
	_templateURL = [URL retain];
	
	return self;
}

@synthesize templateURL=_templateURL;
@dynamic templateDictionary;
- (NSDictionary *)templateDictionary {
	if (!_templateDictionary) {
		if ([[[self templateURL] URLByAppendingPathComponent:@"TemplateInfo.plist"] checkResourceIsReachableAndReturnError:NULL])
			_templateDictionary = [[NSDictionary alloc] initWithContentsOfURL:[[self templateURL] URLByAppendingPathComponent:@"TemplateInfo.plist"]];
	}
	return _templateDictionary;
}
@dynamic templateDescription;
- (NSString *)templateDescription {
	return [[self templateDictionary] objectForKey:@"description"];
}
@dynamic isTemplateHeader;
- (BOOL)isTemplateHeader {
	return ([self templateURL] == nil); 
}
@synthesize projectName=_projectName;
@end
