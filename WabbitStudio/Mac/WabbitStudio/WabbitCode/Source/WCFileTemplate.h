//
//  WCFileTemplate.h
//  WabbitStudio
//
//  Created by William Towe on 4/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"

extern NSString *const kWCFileTemplateDescriptionKey;

extern NSString *const kWCProjectTemplateProjectNamePlaceholder;
extern NSString *const kWCProjectTemplateFileNamePlaceholder;
extern NSString *const kWCProjectTemplateFullUserNamePlaceholder;
extern NSString *const kWCProjectTemplateDatePlaceholder;
extern NSString *const kWCProjectTemplateIncludeFileNamesPlaceholder;

@interface WCFileTemplate : WCTreeNode {
@private
    NSURL *_templateURL;
	NSDictionary *_templateDictionary;
	NSString *_projectName;
	NSString *_fileName;
}
@property (readonly,nonatomic) NSURL *templateURL;
@property (readonly,nonatomic) NSDictionary *templateDictionary;
@property (readonly,nonatomic) NSString *templateDescription;
@property (readonly,nonatomic) BOOL isTemplateHeader;
@property (copy,nonatomic) NSString *projectName;
@property (readonly,nonatomic) NSString *templateFileName;
@property (copy,nonatomic) NSString *fileName;

+ (id)templateWithURL:(NSURL *)URL;
- (id)initWithURL:(NSURL *)URL;
@end
