//
//  WCFileTemplate.h
//  WabbitStudio
//
//  Created by William Towe on 4/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTemplate.h"

extern NSString *const kWCFileTemplateDescriptionKey;

@interface WCFileTemplate : WCTemplate {
@private
	NSString *_fileName;
}
@property (readonly,nonatomic) NSString *templateFileName;
@property (copy,nonatomic) NSString *fileName;

@end
