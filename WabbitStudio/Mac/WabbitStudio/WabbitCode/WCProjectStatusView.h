//
//  WCProjectStatusView.h
//  WabbitStudio
//
//  Created by William Towe on 4/26/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSView.h>


@class WCProject;

@interface WCProjectStatusView : NSView {
@private
    NSString *_statusString;
	NSString *_secondaryStatusString;
	__weak WCProject *_project;
}
@property (copy,nonatomic) NSString *statusString;
@property (copy,nonatomic) NSString *secondaryStatusString;
@property (readonly,nonatomic) WCProject *project;

- (id)initWithProject:(WCProject *)project;
@end
