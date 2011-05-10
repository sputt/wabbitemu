//
//  WCDocumentController.h
//  WabbitStudio
//
//  Created by William Towe on 4/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSDocumentController.h>

@class WCProject;

@interface WCDocumentController : NSDocumentController {
@private
    
}
@property (readonly,nonatomic) WCProject *currentProject;
@property (readonly,nonatomic) NSArray *projects;
@property (readonly,nonatomic) NSArray *recentProjectURLs;
@end
