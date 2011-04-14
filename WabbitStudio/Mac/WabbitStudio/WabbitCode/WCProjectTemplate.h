//
//  WCProjectTemplate.h
//  WabbitStudio
//
//  Created by William Towe on 3/31/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WCTemplate.h"
#import "WCBuildTarget.h"


@interface WCProjectTemplate : WCTemplate {
@private
    
}
@property (readonly,nonatomic) WCBuildTargetOutputType templateOutputType;
@property (readonly,nonatomic) NSArray *includeFileNames;

@end
