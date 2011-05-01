//
//  WELCDView.m
//  WabbitEmu Beta
//
//  Created by William Towe on 4/24/11.
//  Copyright 2011 Revolution Software. All rights reserved.
//

#import "WELCDView.h"
#import "WETransferSheetController.h"


@interface WELCDView ()
@property (copy,nonatomic) NSArray *currentFilePaths;
@property (readonly,nonatomic) NSBitmapImageRep *LCDBitmap;

- (void)_privateInit;
@end

@implementation WELCDView

- (id)initWithCoder:(NSCoder *)coder {
	if (!(self = [super initWithCoder:coder]))
		return nil;
	
	[self commonInit];
	
	return self;
}

- (void)dealloc {
	[_currentFilePaths release];
	_calc = NULL;
	glDeleteTextures(2, _textures);
    [super dealloc];
}

- (BOOL)isOpaque {
	return YES;
}

- (BOOL)acceptsFirstResponder {
	return YES;
}

- (void)mouseDragged:(NSEvent *)event {
	NSString *directoryPath = NSTemporaryDirectory();
	NSString *fileName = NSLocalizedString(@"screenshot", @"screenshot");
	//NSString *fileName = [[NSUserDefaults standardUserDefaults] stringForKey:kWEPrefsScreenShotsFileNameKey];
	NSBitmapImageFileType fileType = NSPNGFileType;
	//NSBitmapImageFileType fileType = [[NSUserDefaults standardUserDefaults] unsignedIntegerForKey:kWEPrefsScreenShotsFormatKey];
	NSString *fileExtension = @"png";
	//NSString *fileExtension = [[WEBasicPerformer sharedInstance] fileExtensionForBitmapImageFileType:fileType];
	NSString *filePath = [directoryPath stringByAppendingPathComponent:[fileName stringByAppendingPathExtension:fileExtension]];
	
	// grab our bitmap rep
	NSBitmapImageRep *bitmap = [self LCDBitmap];
	// get it's data
	NSData *bitmapData = [bitmap representationUsingType:fileType properties:nil];
	// get the dragging pboard
	NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
	// get the point for this event, and the drag point which is centered on the image
	NSPoint dragPoint, location = [self convertPointFromBase:[event locationInWindow]];
	dragPoint.x = location.x - [bitmap size].width/2;
	dragPoint.y = location.y - [bitmap size].height/2;
	// the image that will be dragged
	NSImage *dragImage = [[[NSImage alloc] initWithSize:[bitmap size]] autorelease];
	// temp image used to draw into the drag image
	NSImage *tempImage = [[[NSImage alloc] initWithSize:[bitmap size]] autorelease];
	[tempImage addRepresentation:bitmap];
	[dragImage lockFocus];
	// draw with partial transparency
	[tempImage compositeToPoint:NSZeroPoint operation:NSCompositeCopy fraction:0.85];
	[dragImage unlockFocus];
	
	// write our temporary file to disk
	if (![bitmapData writeToFile:filePath options:NSAtomicWrite error:NULL])
		return;
	
	// declare the pboard types, adding our additional type
	[pboard declareTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil] owner:nil];
	// pass the path to the temporary file we wrote to the pboard
	[pboard setPropertyList:[NSArray arrayWithObjects:filePath,nil] forType:NSFilenamesPboardType];
	
	// initiate the drag
	[self dragImage:dragImage at:dragPoint offset:NSZeroSize event:event pasteboard:pboard source:self slideBack:YES];
}

- (void)keyDown:(NSEvent *)event {
	keypad_key_press(&_calc->cpu, (unsigned int)[event keyCode]);
}

- (void)keyUp:(NSEvent *)event {
	keypad_key_release(&_calc->cpu, (unsigned int)[event keyCode]);
}

- (IBAction)copy:(id)sender {
	if (_calc == NULL || !_calc->active || !_calc->running) {
		NSBeep();
		return;
	}
	
	TCHAR *ans = GetRealAns(&[self calc]->cpu);
	if (ans != NULL) {
		NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSGeneralPboard];
		
		[pboard declareTypes:[NSArray arrayWithObjects:NSStringPboardType, nil] owner:nil];
		[pboard setString:[NSString stringWithCString:ans encoding:NSASCIIStringEncoding] forType:NSStringPboardType];
		
		free(ans);
	}
}

- (void)drawRect:(NSRect)dirtyRect
{	
	if (_calc == NULL || !_calc->active || !_calc->running)
		return;
	
	u_int8_t *lcd = LCD_image([self calc]->cpu.pio.lcd);
	u_int16_t row, col;
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	if ([self usesLCDWirePattern])
		glEnable(GL_BLEND);
	 
	if ([self isWidescreen]) {
		for (row=0; row<kLCDHeight; row++) {
			for (col=0; col<kLCDWidescreenWidth; col++) {
				u_int8_t val = 255-lcd[row*128+col];
				
				_wbuffer[row][col][2] = (0x9E*val)/255;
				_wbuffer[row][col][1] = (0xAB*val)/255;
				_wbuffer[row][col][0] = (0x88*val)/255;
				
			}
		}
	}
	else {
		for (row=0; row<kLCDHeight; row++) {
			for (col=0; col<kLCDWidth; col++) {
				u_char val = 255-lcd[(row)*128+(col)];
				
				_buffer[row][col][2] = (0x9E*val)/255;
				_buffer[row][col][1] = (0xAB*val)/255;
				_buffer[row][col][0] = (0x88*val)/255;
				
			}
		}
	}
	
	CGFloat width = ([self isWidescreen])?kLCDWidescreenWidth:kLCDWidth, height = kLCDHeight;
	
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, _textures[0]);
	glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE);
	glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStoref(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
	
	if ([self isWidescreen])
		glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _wbuffer);
	else
		glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _buffer);
		
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, 1.0f);
	glTexCoord2f(0.0f, height );
	glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(width, height );
	glVertex2f(1.0f, -1.0f);
	glTexCoord2f(width, 0.0f );
	glVertex2f(1.0f, 1.0f);
	glEnd();
	
	if ([self usesLCDWirePattern]) {		
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _textures[1]);
		glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE);
		glTexParameterf(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glPixelStoref(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
		
		if ([self isWidescreen])
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _wlcd_buffer);
		else
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, _lcd_buffer);
		
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-1.0f, 1.0f);
		glTexCoord2f(0.0f, height );
		glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(width, height );
		glVertex2f(1.0f, -1.0f);
		glTexCoord2f(width, 0.0f );
		glVertex2f(1.0f, 1.0f);
		glEnd();
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glDisable(GL_BLEND);
	}
	
	glFinish();
}

- (void)prepareOpenGL {
	[super prepareOpenGL];
	
	glClearColor(128.0/255.0, 142.0/255.0, 107.0/255.0, 1.0);
	
	glEnable(GL_TEXTURE_RECTANGLE_EXT);
	
	glDisable(GL_DITHER);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_FOG);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glGenTextures(2, _textures);
}

- (void)reshape {
	// comment this out to see something interesting when you have more than one calc open and resize
	[[self openGLContext] makeCurrentContext];
	
	NSRect bounds = [self bounds];
	glViewport( 0, 0, bounds.size.width, bounds.size.height );
}

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
	if ([sender draggingSource] == self)
		return NSDragOperationNone;
	
	NSArray *filePaths = [[sender draggingPasteboard] propertyListForType:NSFilenamesPboardType];
	
	[self setCurrentFilePaths:[WETransferSheetController validateFilePaths:filePaths]];
	return ([[self currentFilePaths] count] > 0)?NSDragOperationCopy:NSDragOperationNone;
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender {
	[WETransferSheetController transferFiles:[self currentFilePaths] toCalculator:(WECalculator *)[[[self window] windowController] document]];
	return YES;
}

@synthesize calc=_calc;
@synthesize usesLCDWirePattern=_usesLCDWirePattern;
@synthesize isWidescreen=_isWidescreen;
@synthesize calculator=_calculator;

@synthesize currentFilePaths=_currentFilePaths;

- (void)commonInit; {	
	[self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
	_usesLCDWirePattern = YES;
	[self _privateInit];
}

@dynamic LCDBitmap;
- (NSBitmapImageRep *)LCDBitmap {
	NSUInteger width = ([self isWidescreen])?256:192, height = 128;
	NSBitmapImageRep *bitmap = [[[NSBitmapImageRep alloc] initWithBitmapDataPlanes:NULL pixelsWide:width pixelsHigh:height bitsPerSample:8 samplesPerPixel:3 hasAlpha:NO isPlanar:NO colorSpaceName:NSCalibratedRGBColorSpace bytesPerRow:0 bitsPerPixel:0] autorelease];
	
	u_int16_t row, col;
	for (row=0; row<height; row++) {
		for (col=0; col<width; col++) {
			NSUInteger pixel[3];
			if ([self isWidescreen]) {
				pixel[2] = _wbuffer[row/2][col/2][2];
				pixel[1] = _wbuffer[row/2][col/2][1];
				pixel[0] = _wbuffer[row/2][col/2][0];
			}
			else {
				pixel[2] = _buffer[row/2][col/2][2];
				pixel[1] = _buffer[row/2][col/2][1];
				pixel[0] = _buffer[row/2][col/2][0];
			}
			
			[bitmap setPixel:pixel atX:col y:row];
		}
	}
	return bitmap;
}

- (void)_privateInit {
	u_int16_t row, col;
	for (row=0; row<kLCDHeight; row++) {
		for (col=0; col<kLCDWidth; col++) {
			// alpha channel is always the same, set it once and forget it, while the lcd doesn't have an alpha channel, opengl requires it for textures
			_buffer[row][col][3] = 255;
		}
	}
	
	for (row=0; row<kLCDHeight; row++) {
		for (col=0; col<kLCDWidth; col++) {
			if (col%2 == 0 && row%2 == 0) {
				_lcd_buffer[row][col][2] = 158;
				_lcd_buffer[row][col][1] = 171;
				_lcd_buffer[row][col][0] = 136;
			}
			else if (col%2 == 1 && row%2 == 1) {
				_lcd_buffer[row][col][2] = 126;
				_lcd_buffer[row][col][1] = 137;
				_lcd_buffer[row][col][0] = 109;
			}
			else {
				_lcd_buffer[row][col][2] = 142;
				_lcd_buffer[row][col][1] = 154;
				_lcd_buffer[row][col][0] = 122;
			}
			// alpha channel is always the same, set it once and forget it, while the lcd doesn't have an alpha channel, opengl requires it for textures
			_lcd_buffer[row][col][3] = 108;
		}
	}
	
	for (row=0; row<kLCDHeight; row++) {
		for (col=0; col<kLCDWidescreenWidth; col++) {
			// alpha channel is always the same, set it once and forget it, while the lcd doesn't have an alpha channel, opengl requires it for textures
			_wbuffer[row][col][3] = 255;
		}
	}
	
	for (row=0; row<kLCDHeight; row++) {
		for (col=0; col<kLCDWidescreenWidth; col++) {
			if (col%2 == 0 && row%2 == 0) {
				_wlcd_buffer[row][col][2] = 158;
				_wlcd_buffer[row][col][1] = 171;
				_wlcd_buffer[row][col][0] = 136;
			}
			else if (col%2 == 1 && row%2 == 1) {
				_wlcd_buffer[row][col][2] = 126;
				_wlcd_buffer[row][col][1] = 137;
				_wlcd_buffer[row][col][0] = 109;
			}
			else {
				_wlcd_buffer[row][col][2] = 142;
				_wlcd_buffer[row][col][1] = 154;
				_wlcd_buffer[row][col][0] = 122;
			}
			// alpha channel is always the same, set it once and forget it, while the lcd doesn't have an alpha channel, opengl requires it for textures
			_wlcd_buffer[row][col][3] = 108;
		}
	}
}
@end
