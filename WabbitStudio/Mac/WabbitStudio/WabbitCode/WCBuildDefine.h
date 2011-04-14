//
//  WCBuildDefine.h
//  WabbitStudio
//
//  Created by William Towe on 3/28/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCObject.h"


@interface WCBuildDefine : WCObject <NSCopying,NSCoding> {
@private
    NSString *_defineValue; // <name> = <value>, this stores the <value> part
}
@property (copy,nonatomic) NSString *defineValue;
@property (readonly,nonatomic) NSString *processedDefine;

+ (id)defineWithName:(NSString *)name;

+ (id)defineWithName:(NSString *)name value:(NSString *)value;
- (id)initWithName:(NSString *)name value:(NSString *)value;
@end
