//
//  WCPlistRepresentationProtocol.h
//  WabbitStudio
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>


@protocol WCPlistRepresentation <NSObject>
@required
- (NSDictionary *)plistRepresentation;
@end
