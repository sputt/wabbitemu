//
//  WCDefines.h
//  WabbitStudio
//
//  Created by William Towe on 4/1/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>

#define NS_LOCALIZED_STRING_DELETE NSLocalizedString(@"Delete",@"Delete")
#define NS_LOCALIZED_STRING_DELETE_ALL NSLocalizedString(@"Delete All",@"Delete All")
#define NS_LOCALIZED_STRING_CANCEL NSLocalizedString(@"Cancel",@"Cancel")
#define NS_LOCALIZED_STRING_CREATE NSLocalizedString(@"Create",@"Create")
#define NS_LOCALIZED_STRING_ADD NSLocalizedString(@"Add",@"Add")
#define NS_LOCALIZED_STRING_DONE NSLocalizedString(@"Done",@"Done")
#define NS_LOCALIZED_STRING_NONE NSLocalizedString(@"None",@"None")
#define NS_LOCALIZED_STRING_CHOOSE NSLocalizedString(@"Choose",@"Choose")
#define NS_LOCALIZED_STRING_SAVE NSLocalizedString(@"Save",@"Save")
#define NS_LOCALIZED_STRING_FALSE NSLocalizedString(@"FALSE",@"FALSE")
#define NS_LOCALIZED_STRING_TRUE NSLocalizedString(@"TRUE",@"TRUE")

// returns a rect with the size of rect1 centered inside rect2
static inline NSRect WCCenteredRect(NSRect rect1, NSRect rect2) {
	return NSMakeRect((rect2.origin.x+floor(rect2.size.width/2.0))-floor(rect1.size.width/2.0), (rect2.origin.y+floor(rect2.size.height/2.0))-floor(rect1.size.height/2.0), rect1.size.width, rect1.size.height);
}

static inline NSRect WCCenteredRectWithSize(NSSize size, NSRect rect) {
	return WCCenteredRect(NSMakeRect(NSMinX(rect), NSMinY(rect), size.width, size.height), rect);
}

enum {
	WCFindScopeFile = 0,
	WCFindScopeSelection = 1,
	WCFindScopeOpenFiles = 2,
	WCFindScopeAllFiles = 3
	
};
typedef NSUInteger WCFindScope;

enum {
	WCFindSubScopeContains = 0,
	WCFindSubScopeStartsWith = 1,
	WCFindSubScopeEndsWith = 2,
	WCFindSubScopeWholeWord = 3
	
};
typedef NSUInteger WCFindSubScope;

extern const NSRange WCEmptyRange;

@interface WCDefines : NSObject {
@private
    
}

@end
