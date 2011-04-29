//
//  WELCDView.h
//  WabbitEmu Beta
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import "WELCDProtocol.h"


@class WECalculator;

@interface WELCDView : NSOpenGLView <WELCDProtocol> {
@private	
	GLubyte _buffer[kLCDHeight][kLCDWidth][4];
	GLubyte _lcd_buffer[kLCDHeight][kLCDWidth][4];
	GLubyte _wbuffer[kLCDHeight][kLCDWidescreenWidth][4];
	GLubyte _wlcd_buffer[kLCDHeight][kLCDWidescreenWidth][4];
	GLuint _textures[2];
	
	LPCALC _calc;
	BOOL _isWidescreen;
	
	NSArray *_currentFilePaths;
}

- (void)commonInit;
@end
