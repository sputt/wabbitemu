//
//  WCAddDocumentToProjectSheet.h
//  WabbitStudio
//
//  Created by William Towe on 4/16/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSWindowController.h>


@class WCDocument;

@interface WCAddDocumentToProjectSheet : NSWindowController {
@private
	IBOutlet NSPopUpButton *_popUpButton;
	
    __weak WCDocument *_documentToAdd;
}

+ (void)presentSheetForDocument:(WCDocument *)documentToAdd;
@end
