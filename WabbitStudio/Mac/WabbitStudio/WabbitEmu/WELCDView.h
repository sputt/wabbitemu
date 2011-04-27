//
//  WELCDView.h
//  WabbitEmu Beta
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#include "calc.h"


@interface WELCDView : NSOpenGLView {
@private	
	GLubyte _buffer[128][192][4];
	GLubyte _lcd_buffer[128][192][4];
	GLuint _textures[2];
	
	LPCALC _calc;
}

@property (assign,nonatomic) LPCALC calc;
@end
