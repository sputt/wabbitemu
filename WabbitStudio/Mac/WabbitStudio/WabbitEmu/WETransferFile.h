//
//  WETransferFile.h
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Foundation/NSObject.h>
#include "var.h"


enum {
	WETransferFileTypeRom = ROM_TYPE,
	WETransferFileTypeFlash = FLASH_TYPE,
	WETransferFileTypeVar = VAR_TYPE,
	WETransferFileTypeSavestate = SAV_TYPE,
	WETransferFileTypeBackup = BACKUP_TYPE,
	WETransferFileTypeLabel = LABEL_TYPE,
	WETransferFileTypeBreakpoint = BREAKPOINT_TYPE,
	WETransferFileTypeGroup = GROUP_TYPE
};
typedef NSUInteger WETransferFileType;

@interface WETransferFile : NSObject {
@private
	NSString *_path;
    WETransferFileType _type;
	TIFILE_t *_tifile;
	size_t _size;
	CGFloat _currentProgress;
}
@property (readonly,nonatomic) NSString *path;
@property (readonly,nonatomic) WETransferFileType type;
@property (readonly,nonatomic) TIFILE_t *tifile;
@property (readonly,nonatomic) size_t size;
@property (assign,nonatomic) CGFloat currentProgress;

+ (WETransferFile *)transferFileWithPath:(NSString *)path;
- (id)initWithPath:(NSString *)path;
@end
