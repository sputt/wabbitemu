#include "stdafx.h"

#include "displayoptionstest.h"
#include "lcd.h"
#include "device.h"
#include "map.h"
#include "linksprites.h"


static unsigned char ball[] = {
	0,0,1,1,1,1,0,0,
	0,1,0,0,0,0,1,0,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	0,1,0,0,0,0,1,0,
	0,0,1,1,1,1,0,0
};

static u_char display_test[LCD_HEIGHT][LCD_MEM_WIDTH];


void fastcopy(CPU_t *cpu) {
	int x, y;
	//was LCD_MEM_WIDTH, but most calcs don't do that.
	for (x = 0; x < 12; x++) {
		for (y = 0; y < LCD_HEIGHT; y++) {
			cpu->bus = display_test[y][x];
			cpu->output = TRUE;
			
			cpu->pio.lcd->x = y;
			cpu->pio.lcd->y = x;
			
			cpu->pio.lcd->data(cpu, &(cpu->pio.devices[0x11]));
			tc_add(cpu->timer_c, 67);
		}
	}
}


static void pixel(int x, int y, int i) {
	if (y<0) return;
	if (x<0) return;
	if (y>63) return;
	if (x>95) return;
	
	u_char data = display_test[y][x/8];

	data &= ~(0x80 >> (x % 8));
	data |= i << (7 - (x % 8));
	
	display_test[y][x/8] = data;
}
static int getpixel(int x, int y, int w, int h, unsigned char *img) {
	if (x<0) x+=w;
	if (x>=w) x-=w;
	if (y<0) y+=h;
	if (y>=h) y-=h;	
	return (img[(y*w/8)+(x/8)]>>(7-(x&0x07)))&0x01;
}
static void clearscreen() {
	memset(display_test, 0, DISPLAY_SIZE);
}

static void sprite(int x,int y,int w,int h,unsigned char * img) {
	int xc,yc;
	for(yc=0;yc<h;yc++) {
		for(xc=0;xc<w;xc++) {
			if (img[(yc*w)+xc]!=2) {
				pixel(xc+x,yc+y,img[(yc*w)+xc]);
			}
		}
	}
}
//BW shades = 2
static void gradient(int shades, double, double time) {
	int frame = (int) (time*70.0f);
	int x,y,c;
	for(y=0;y<64;y++) {
		for(x=0;x<96;x++) {
			c = (((frame+(y)+x)%(shades-1))<(x*shades/96))?1:0;
			pixel(x,y,c);
		}
	}
	frame++;
}	
static void bounce(double time) {
	double g	= 9.80f*28.0f;
	double dy	= 44.0f;
	double dx	= 104.0f;
	double vx	= 20.0f;
	double vy	= -sqrt(2.0f*dy*g);
	double tmaxy= sqrt((2.0f*dy)/g);
	double ty	= fmod((time),(tmaxy*2));
	double y	= 56.0f+(vy*ty)+(0.5f*g*ty*ty);
	double tmaxx= dx/vx;
	double tx	= fmod(time,tmaxx);
	double x	= (tx*vx)-7.0f;
	sprite((int) x, (int) y, 8, 8, ball);
}

static void map() {
	static int px = 96;
	static int py = 96;
	static int dir = 0;
	int i,x,y;
	if (px==96 && py==96) {
		do {
			i = rand()%4;
		} while( i == (dir^0x01) );
		dir = i&0x03;
	}
	
	switch(dir) {
		case 0x00:
			py--;
			break;
		case 0x01:
			py++;
			break;
		case 0x02:
			px--;
			break;
		case 0x03:
			px++;
			break;

	}
	if (px<0) px+=192;
	if (px>=192) px-=192;
	if (py<0) py+=192;
	if (py>=192) py-=192;	
	for(y=0;y<64;y++) {
		for(x=0;x<96;x++) {
			pixel(x,y,getpixel(x+px-48,y+py-32,192,192,MapImg));
		}
	}
	
	switch(dir) {
		case 0x00:
		case 0x01:
			i = (dir<<1)+((py/8)&0x01);
			break;
		case 0x02:
		case 0x03:
			i = (dir<<1)+((px/8)&0x01);
			break;
		default:
			return;

	}
	sprite(40,24,16,16,linksprite[i]);
	
}

u_char *displayoptionstest_draw_scroll(int, double, double) {
	map();
	return (u_char*) display_test;
}

u_char *displayoptionstest_draw_bounce(int, double, double time) {
	clearscreen();
	bounce(time);
	return (u_char*) display_test;
}

u_char *displayoptionstest_draw_gradient(int shades, double framerate, double time) {
	gradient(shades, framerate, time);
	return (u_char *) display_test;
}