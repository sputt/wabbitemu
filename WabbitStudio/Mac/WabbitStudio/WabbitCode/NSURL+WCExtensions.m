//
//  NSURL+WCExtensions.m
//  WabbitStudio
//
//  Created by William Towe on 3/29/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "NSURL+WCExtensions.h"


@implementation NSURL (NSURL_WCExtensions)
// assumes that both the receiver and 'fileURL' are file URL's
- (BOOL)isEqualToFileURL:(NSURL *)fileURL; {
	return [[self path] isEqualToString:[fileURL path]];
}
@end
