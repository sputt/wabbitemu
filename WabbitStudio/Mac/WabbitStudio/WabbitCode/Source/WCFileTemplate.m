//
//  WCFileTemplate.m
//  WabbitStudio
//
//  Created by William Towe on 4/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCFileTemplate.h"

NSString *const kWCFileTemplateDescriptionKey = @"description";

NSString *const kWCProjectTemplateProjectNamePlaceholder = @"<<PROJECT_NAME>>";
NSString *const kWCProjectTemplateFileNamePlaceholder = @"<<FILE_NAME>>";
NSString *const kWCProjectTemplateFullUserNamePlaceholder = @"<<FULL_USER_NAME>>";
NSString *const kWCProjectTemplateDatePlaceholder = @"<<DATE>>";
NSString *const kWCProjectTemplateIncludeFileNamesPlaceholder = @"<<INCLUDE_FILE_NAMES>>";

@implementation WCFileTemplate

- (void)dealloc {
	[_fileName release];
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

- (NSImage *)icon {
	if ([self isTemplateHeader])
		return [super icon];
	return [[NSWorkspace sharedWorkspace] iconForFile:[[[self templateURL] URLByAppendingPathComponent:[self templateFileName]] path]];
}

@synthesize templateURL=_templateURL;
@dynamic templateDictionary;
- (NSDictionary *)templateDictionary {
	if (!_templateDictionary) {
		if ([[[self templateURL] URLByAppendingPathComponent:@"info.plist"] checkResourceIsReachableAndReturnError:NULL])
			_templateDictionary = [[NSDictionary alloc] initWithContentsOfURL:[[self templateURL] URLByAppendingPathComponent:@"info.plist"]];
	}
	return _templateDictionary;
}
@dynamic templateDescription;
- (NSString *)templateDescription {
	return [[self templateDictionary] objectForKey:kWCFileTemplateDescriptionKey];
}
@dynamic isTemplateHeader;
- (BOOL)isTemplateHeader {
	return ([self templateURL] == nil); 
}
@synthesize projectName=_projectName;
@dynamic templateFileName;
- (NSString *)templateFileName {
	return [[self templateDictionary] objectForKey:@"templateFileName"];
}
@synthesize fileName=_fileName;

+ (id)templateWithURL:(NSURL *)URL; {
	return [[[[self class] alloc] initWithURL:URL] autorelease];
}
- (id)initWithURL:(NSURL *)URL; {
	if (!(self = [super initWithName:nil]))
		return nil;
	
	_templateURL = [URL retain];
	
	return self;
}

@end
