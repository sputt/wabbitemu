#import <Cocoa/Cocoa.h>
#import <QuickLook/QuickLook.h>
#include "calc.h"

/* -----------------------------------------------------------------------------
   Generate a preview for file

   This function's job is to create preview for designated file
   ----------------------------------------------------------------------------- */

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    LPCTSTR cPath = [[(NSURL *)url path] fileSystemRepresentation];
	LPCALC calc = calc_slot_new();
	
	if (calc == NULL)
		goto CLEANUP;
	
	if (!rom_load(calc, cPath))
		goto CLEANUP;
	
	// we cannot get a suitable preview image if the calc is not turned on
	if ([(NSString *)contentTypeUTI isEqualToString:@"org.revsoft.wabbitemu.rom"])
		calc_turn_on(calc);
	
	NSUInteger width = (calc->model == TI_85 || calc->model == TI_86)?256:192, height = 128;
	NSBitmapImageRep *bitmap = [[[NSBitmapImageRep alloc] initWithBitmapDataPlanes:NULL pixelsWide:width pixelsHigh:height bitsPerSample:8 samplesPerPixel:3 hasAlpha:NO isPlanar:NO colorSpaceName:NSCalibratedRGBColorSpace bytesPerRow:0 bitsPerPixel:0] autorelease];
	
	u_int8_t *lcdptr = LCD_image(calc->cpu.pio.lcd);
	u_int16_t row, col;
	for (row=0; row<height; row++) {
		for (col=0; col<width; col++) {
			u_int8_t val = 255-lcdptr[(row/2)*128+(col/2)];
			NSUInteger pixel[3] = {
				(0x9E*val)/255,
				(0xAB*val)/255,
				(0x88*val)/255
			};
			
			[bitmap setPixel:pixel atX:col y:row];
		}
	}
	
	NSData *data = [bitmap representationUsingType:NSPNGFileType properties:nil];
	NSDictionary *properties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithUnsignedInteger:height], (NSString *)kQLPreviewPropertyHeightKey, [NSNumber numberWithUnsignedInteger:width], (NSString *)kQLPreviewPropertyWidthKey, nil];
	
	QLPreviewRequestSetDataRepresentation(preview, (CFDataRef)data, kUTTypeImage, (CFDictionaryRef)properties);
	
CLEANUP:
	calc_slot_free(calc);
	[pool drain];
	
    return noErr;
}

void CancelPreviewGeneration(void* thisInterface, QLPreviewRequestRef preview)
{
    // implement only if supported
}
