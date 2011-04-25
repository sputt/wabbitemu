//
//  WCObject.h
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>
#import <Foundation/NSString.h>
#import <AppKit/NSImage.h>
#import <Foundation/NSDictionary.h>
#import "WCPlistRepresentationProtocol.h"


@interface WCObject : NSObject <NSCoding,NSCopying,NSMutableCopying,WCPlistRepresentation> {
@private
	NSString *_name;
	NSImage *_icon;
}
@property (readonly,nonatomic) NSImage *icon;
@property (copy,nonatomic) NSString *name;

+ (id)objectWithName:(NSString *)name;
- (id)initWithName:(NSString *)name;

+ (id)objectWithName:(NSString *)name icon:(NSImage *)icon;
- (id)initWithName:(NSString *)name icon:(NSImage *)icon;
@end
