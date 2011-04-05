//
//  WCLinkedView.m
//  WabbitStudio
//
//  Created by William Towe on 3/18/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCLinkedView.h"


@implementation WCLinkedView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)dealloc {
	_previousView = nil;
	_nextView = nil;
    [super dealloc];
}

@synthesize nextView=_nextView;
@synthesize previousView=_previousView;
@end
