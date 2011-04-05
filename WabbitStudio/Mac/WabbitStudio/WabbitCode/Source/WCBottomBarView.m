//
//  WCBottomBarView.m
//  WabbitStudio
//
//  Created by William Towe on 3/17/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCBottomBarView.h"


static NSGradient *_bottomBarGradient = nil;
static NSColor *_topLineColor = nil;

@implementation WCBottomBarView

+ (void)initialize {
	_bottomBarGradient = [[NSGradient alloc] initWithColorsAndLocations:[NSColor colorWithCalibratedWhite:253.0/255.0 alpha:1.0],0.0,[NSColor colorWithCalibratedWhite:242.0/255.0 alpha:1.0],0.45454,[NSColor colorWithCalibratedWhite:230.0/255.0 alpha:1.0],0.45454,[NSColor colorWithCalibratedWhite:230.0/255.0 alpha:1.0],1.0,nil];
	_topLineColor = [[NSColor colorWithCalibratedWhite:190.0/255.0 alpha:1.0] retain];
}

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)dealloc {
    [super dealloc];
}

- (void)drawRect:(NSRect)dirtyRect {
	NSRect bounds = [self bounds];
	
	[_bottomBarGradient drawInRect:bounds angle:-90.0];
	
	[_topLineColor setFill];
	NSRectFill(NSMakeRect(0.0, bounds.origin.y+bounds.size.height-1.0, bounds.size.width, 1.0));
	
}

@end
