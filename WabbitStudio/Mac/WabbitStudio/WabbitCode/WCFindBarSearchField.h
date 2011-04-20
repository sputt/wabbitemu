//
//  WCFindBarSearchField.h
//  WabbitStudio
//
//  Created by William Towe on 4/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSSearchField.h>


@class WCFindBarViewController;

@interface WCFindBarSearchField : NSSearchField {
@private
	IBOutlet WCFindBarViewController *_findBarViewController;
}

@end
