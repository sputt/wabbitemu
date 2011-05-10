//
//  WCLinkedView.h
//  WabbitStudio
//
//  Created by William Towe on 3/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@class WCLinkedView;

@interface WCLinkedView : NSView {
@private
    IBOutlet WCLinkedView *_nextView;
	IBOutlet WCLinkedView *_previousView;
}
@property (readonly) WCLinkedView *nextView;
@property (readonly) WCLinkedView *previousView;
@end
