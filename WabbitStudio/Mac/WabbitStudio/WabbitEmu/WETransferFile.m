//
//  WETransferFile.m
//  WabbitStudio
//
//  Created by William Towe on 4/27/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WETransferFile.h"



@implementation WETransferFile

- (void)dealloc {
#ifdef DEBUG
	NSLog(@"%@ called in %@",NSStringFromSelector(_cmd),[self className]);
#endif
	FreeTiFile(_tifile);
	[_path release];
    [super dealloc];
}

+ (WETransferFile *)transferFileWithPath:(NSString *)path; {
	return [[[[self class] alloc] initWithPath:path] autorelease];
}
- (id)initWithPath:(NSString *)path; {
	if (!(self = [super init]))
		return nil;
	
	_path = [path copy];
	
	_tifile = newimportvar([path fileSystemRepresentation]);
	_type = _tifile->type;
	
	switch (_type) {
		case WETransferFileTypeFlash:
			for (uint16_t page = 0; page < 256; page++)
				_size += _tifile->flash->pagesize[page];
			break;
		case WETransferFileTypeGroup:
		case WETransferFileTypeVar:
			_size = _tifile->var->length;
		default:
			break;
	}
	
	return self;
}

@synthesize path=_path;
@synthesize type=_type;
@synthesize tifile=_tifile;
@synthesize size=_size;
@synthesize currentProgress=_currentProgress;

@end
