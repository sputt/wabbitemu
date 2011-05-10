//
//  WCDocument.h
//  WabbitStudio
//
//  Created by William Towe on 4/7/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSDocument.h>

@class WCFile,WCFileViewController,WCProject;

@interface WCDocument : NSDocument <NSUserInterfaceValidations> {
@private
	__weak WCProject *_project;
    WCFileViewController *_fileViewController;
	WCFile *_file;
}
@property (readonly,nonatomic) WCFile *file;
@property (assign,nonatomic) WCProject *project;
@property (readonly,nonatomic) WCFileViewController *fileViewController;

- (IBAction)addDocumentToProject:(id)sender;
@end
