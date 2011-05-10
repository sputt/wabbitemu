//
//  WCIncludeDirectory.h
//  WabbitStudio
//
//  Created by William Towe on 3/29/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCObject.h"

@class WCAlias,WCFile;

@interface WCIncludeDirectory : WCObject <NSCoding,NSCopying,NSMutableCopying,WCPlistRepresentation> {
@private
    WCAlias *_alias;
}
@property (readonly,nonatomic) WCAlias *alias;
@property (copy,nonatomic) NSURL *URL;
@property (readonly,nonatomic) NSString *absolutePathForDisplay;

+ (id)includeDirectoryWithURL:(NSURL *)url;
- (id)initWithURL:(NSURL *)url;
@end
