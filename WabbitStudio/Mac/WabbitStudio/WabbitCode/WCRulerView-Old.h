//
//  WCRulerView.h
//  WabbitStudio
//
//  Created by William Towe on 3/23/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <AppKit/NSRulerView.h>


@class WCFile;

@interface WCRulerView : NSRulerView {
@private
    WCFile *_file;
	BOOL _hasAdjustedRulerThickness;
}
- (id)initWithScrollView:(NSScrollView *)scrollView forFile:(WCFile *)file;

- (void)adjustRequiredThickness;
@end
