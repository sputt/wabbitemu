//
//  WCProjectTemplate.h
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTreeNode.h"
#import "WCBuildTarget.h"

extern NSString *const kWCProjectTemplateOutputTypeKey;
extern NSString *const kWCProjectTemplateDescriptionKey;
extern NSString *const kWCProjectTemplateIncludeFileNamesKey;

@interface WCProjectTemplate : WCTreeNode {
@private
    NSURL *_templateURL;
	NSDictionary *_templateDictionary;
	NSString *_projectName;
}
@property (readonly,nonatomic) NSURL *templateURL;
@property (readonly,nonatomic) NSDictionary *templateDictionary;
@property (readonly,nonatomic) WCBuildTargetOutputType templateOutputType;
@property (readonly,nonatomic) NSString *templateDescription;
@property (readonly,nonatomic) NSArray *includeFileNames;
@property (readonly,nonatomic) BOOL isTemplateHeader;
@property (copy,nonatomic) NSString *projectName;

+ (id)templateWithURL:(NSURL *)URL;
- (id)initWithURL:(NSURL *)URL;

@end
