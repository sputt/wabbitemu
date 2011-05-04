//
//  RSDebuggerDetailsViewController.m
//  WabbitStudio
//
//  Created by William Towe on 5/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "RSDebuggerDetailsViewController.h"
#import "NSViewController+RSExtensions.h"
#import "WCDefines.h"


@interface RSDebuggerDetailsViewController ()

@end

@implementation RSDebuggerDetailsViewController

- (void)dealloc {
	[_calculator release];
    [super dealloc];
}

- (NSString *)viewNibName {
	return @"RSDebuggerDetailsView";
}

- (void)loadView {
	[super loadView];
	
	[(NSScrollView *)[self view] setDocumentView:_documentView];
	
	NSPoint newScrollOrigin;
	
    // assume that the scrollview is an existing variable
    if ([[(NSScrollView *)[self view] documentView] isFlipped]) {
        newScrollOrigin=NSMakePoint(0.0,0.0);
    } else {
        newScrollOrigin=NSMakePoint(0.0,NSMaxY([[(NSScrollView *)[self view] documentView] frame])
									-NSHeight([[(NSScrollView *)[self view] contentView] bounds]));
    }
	
    [[(NSScrollView *)[self view] documentView] scrollPoint:newScrollOrigin];
}

- (id)initWithCalculator:(RSCalculator *)calculator; {
	if (!(self = [super initWithNibName:[self viewNibName] bundle:nil]))
		return nil;
	
	_calculator = [calculator retain];
	
	return self;
}

@synthesize calculator=_calculator;

@end
