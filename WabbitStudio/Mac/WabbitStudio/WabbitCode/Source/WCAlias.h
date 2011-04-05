//
//  WCAlias.h
//  WabbitStudio
//
//  Created by William Towe on 3/29/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCObject.h"

@class NDAlias;

@interface WCAlias : WCObject <NSCoding,NSCopying> {
@private
    NDAlias *_alias;
	NSString *_cachedUTI;
}
@property (retain,nonatomic) NSURL *fileURL;
@property (readonly,nonatomic) NSString *absolutePath;
@property (readonly,nonatomic) NSString *absolutePathForDisplay;
@property (readonly,nonatomic) NSString *UTI;

+ (id)aliasWithURL:(NSURL *)url;
- (id)initWithURL:(NSURL *)url;
@end
