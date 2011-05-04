//
//  RSDebuggerDetailsViewController.h
//  WabbitStudio
//
//  Created by William Towe on 5/3/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSViewController.h>


@class RSCalculator;

@interface RSDebuggerDetailsViewController : NSViewController {
@private
	IBOutlet NSView *_documentView;
	
    RSCalculator *_calculator;
}
@property (retain,nonatomic) RSCalculator *calculator;

- (id)initWithCalculator:(RSCalculator *)calculator;

@end
