//
//  WCTemplate.h
//  WabbitStudio
//
//  Created by William Towe on 4/5/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"

extern NSString *const kWCTemplateProjectNamePlaceholder;
extern NSString *const kWCTemplateFileNamePlaceholder;
extern NSString *const kWCTemplateFullUserNamePlaceholder;
extern NSString *const kWCTemplateDatePlaceholder;
extern NSString *const kWCTemplateIncludeFileNamesPlaceholder;

@interface WCTemplate : WCTreeNode {
@private
	NSURL *_templateURL;
	NSDictionary *_templateDictionary;
	NSString *_projectName;
}
@property (readonly,nonatomic) NSURL *templateURL;
@property (readonly,nonatomic) NSDictionary *templateDictionary;
@property (readonly,nonatomic) NSString *templateDescription;
@property (readonly,nonatomic) BOOL isTemplateHeader;
@property (copy,nonatomic) NSString *projectName;

+ (id)templateWithURL:(NSURL *)URL;
- (id)initWithURL:(NSURL *)URL;
@end
