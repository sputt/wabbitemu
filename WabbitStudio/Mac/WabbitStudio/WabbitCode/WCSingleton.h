//
//  WCSingleton.h
//  WabbitStudio
//
//  Created by William Towe on 3/20/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>


// manages all the singletons in the project, you have to override init for this to work properly
@interface WCSingleton : NSObject {
@private
    
}
+ (id)sharedController;
+ (id)sharedPerformer;
@end
