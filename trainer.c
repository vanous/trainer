// Petr Vanek
// LGPL
#include <ncurses.h>			/* ncurses.h includes stdio.h */  
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#ifndef WIN32
#include <glob.h> //find /dev/files
#include <termios.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#else
#include <winsock2.h>
//#include <windows.h>
#include "serialport.c" //includes windows.h
#include <io.h>
#endif
#include <time.h>
#include <getopt.h>
#include <errno.h>
#include <artnet/artnet.h>


#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


#define INFO 0x14
#define RDM_UID 0x24
#define PACKET_START 0xA5
#define SEND_DMX 6
#define STOP_DMX_RDM 8
#define DMX_IN 4
#define SNIFFER 0x34
#ifndef WIN32
	struct termios oldtio,newtio;
#endif

unsigned char buffer[1024];
int size;
int fd=-1;

#ifdef WIN32
	HANDLE h; //serial port
#endif

#define ARTNET
//#define DEBUG 

int calib[8][4]; // calibration values
int a,b;		 // helpers to init calib
int menu_r=0;	 // cursor position row
int menu_c=0;	// cursor position column
WINDOW  *w=NULL;

enum fixtures
{
NONE,
DLX, 
DLS, 
DLF
};

enum fixtures fixture = NONE;

int serial_port;
unsigned char serial_alias[42]="...searching...";
char serial_name[40];

int uid=0; //is DEVICE detected?

const char* fixtures_print[] = {
"press c to configure fixture",
"DLX",
"DLS",
"DLF"
};


float fade_time;

int program_step = 0;
int current_step = 0;
int current_program = 0;
int program_length;
unsigned long tstart;
unsigned long tend;
unsigned long tallend;
unsigned long t;
int sum=0;
int tsum=0;

#ifndef DEBUG
	int verbose = 0;
#else
	int verbose = 1;
#endif

int MAXCHANNELS=512;

static artnet_node node;

unsigned char dmx[512]; //dmx array


//           program,step,values
static int program[5][32][70]={
//first step is defining #of steps in program and delay time in ms for each step


//test program, pan/tilt movement

{}
,
{//DLX M1 The OFF 8000  4200     3200     2700   ON   8000     5600    4200   OFF   5600
{30,40,40,40,10,100,10,10,100,10,10,60,10,10,60,10,40,10,60,10,10,60,10,10,60,10,60,10,60,10,40}, //number of steps, delay for each step
{0,0,0,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8000K WhitePoint ON (shutter off, 3sec)
{0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//RGBW mixing ON (shutter off, 3sec)
{0,0,0,0,0,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//Theater mode OFF (3sec)
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,560,561,562,563,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8000K calibration
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,560,561,562,563,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,560,561,562,563,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,520,521,522,523,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//4200K calibration
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,520,521,522,523,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,520,521,522,523,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 
{0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,510,511,512,513,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//3200K calibration
{0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,510,511,512,513,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations
{0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,510,511,512,513,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 
{0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,500,501,502,503,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//2700K calibration
{0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,500,501,502,503,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations
{0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,500,501,502,503,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 
{0,0,0,0,0,241,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//Theater mode ON (3sec)
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,570,571,572,573,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8000K theater calibration
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,570,571,572,573,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,570,571,572,573,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,550,551,552,553,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//5600K theater calibration
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,550,551,552,553,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,550,551,552,553,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,530,531,532,533,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//4200K theater calibration
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,530,531,532,533,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,530,531,532,533,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 
{0,0,0,0,0,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//Theater mode OFF (3sec)
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,540,541,542,543,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//5600K calibration
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,540,541,542,543,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,540,541,542,543,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 
{128,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}//end
}
,
{//DLS
{30,40,40,40,10,40,10,10,40,10,10,40,10,10,40,10,40,10,40,10,10,40,10,10,40,10,40,10,40,10,40}, //number of steps, delay for each step
{0,0,0,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8000K WhitePoint ON (shutter off, 3sec)
{0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//RGBW mixing ON (shutter off, 3sec)
{0,0,0,0,0,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//Theater mode OFF (3sec)
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,560,561,562,563,0},//8000K calibration
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,560,561,562,563,0},//SAVE calibrations
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,560,561,562,563,0},//back to 0 
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,520,521,522,523,0},//4200K calibration
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,520,521,522,523,0},//SAVE calibrations
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,520,521,522,523,0},//back to 0 
{0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,511,512,513,0},//3200K calibration
{0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,511,512,513,0},//SAVE calibrations
{0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,511,512,513,0},//back to 0 
{0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,500,501,502,503,0},//2700K calibration
{0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,500,501,502,503,0},//SAVE calibrations
{0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,500,501,502,503,0},//back to 0 
{0,0,0,0,0,241,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//Theater mode ON (3sec)
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,570,571,572,573,0},//8000K theater calibration
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,570,571,572,573,0},//SAVE calibrations
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,570,571,572,573,0},//back to 0 
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,550,551,552,553,0},//5600K theater calibration
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,550,551,552,553,0},//SAVE calibrations
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,550,551,552,553,0},//back to 0 ,
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,530,531,532,533,0},//4200K theater calibration,
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,530,531,532,533,0},//SAVE calibrations
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,530,531,532,533,0},//back to 0 
{0,0,0,0,0,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//Theater mode OFF (3sec)
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,540,541,542,543,0},//5600K calibration
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,540,541,542,543,0},//SAVE calibrations
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,540,541,542,543,0},//back to 0 
{128,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}//end
},


{//DLF
{30,40,40,40,10,40,10,10,40,10,10,40,10,10,40,10,40,10,40,10,10,40,10,10,40,10,40,10,40,10,40}, //number of steps, delay for each step
{0,0,0,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8000K WhitePoint ON (shutter off, 3sec)
{0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//RGBW mixing ON (shutter off, 3sec)
{0,0,0,0,0,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//Theater mode OFF (3sec)
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,560,561,562,563,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8000K calibration
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,560,561,562,563,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations,
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,560,561,562,563,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 ,
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,520,521,522,523,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//4200K calibration,
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,520,521,522,523,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations,
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,520,521,522,523,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 ,
{0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,510,511,512,513,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//3200K calibration,
{0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,510,511,512,513,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations,
{0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,510,511,512,513,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 ,
{0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,500,501,502,503,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//2700K calibration,
{0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,500,501,502,503,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations,
{0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,500,501,502,503,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 ,
{0,0,0,0,0,241,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//Theater mode ON (3sec),
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,570,571,572,573,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8000K theater calibration,
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,570,571,572,573,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations,
{0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,570,571,572,573,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 ,
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,550,551,552,553,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//5600K theater calibration,
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,550,551,552,553,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations,
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,550,551,552,553,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 ,
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,530,531,532,533,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//4200K theater calibration,
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,530,531,532,533,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations,
{0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,530,531,532,533,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 ,
{0,0,0,0,0,240,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//Theater mode OFF (3sec),
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,540,541,542,543,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//5600K calibration,
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,540,541,542,543,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//SAVE calibrations,
{0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,0,0,0,0,0,540,541,542,543,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//back to 0 ,
{128,0,128,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}//end,,
}
,
{
{5,20,20,20,20,20},//number of steps, delay for each step
{0,0},
{121,0,128,0},
{254,0,255,0},
{122,0,128,0},
{0,0}
}
};



// color temperatures labels
const char *c_temp[] = {
	"2700        ",
	"3200        ",
	"4200        ",
	"4200 Theatre",
	"5600        ",
	"5600 Theatre",
	"8000        ",
	"8000 Theatre",
};

void dmxusb_mute_dmx(){

	buffer[0]=PACKET_START;
	buffer[1]=STOP_DMX_RDM;
	buffer[2]=0;
	buffer[3]=0;
	buffer[4]=buffer[0]+buffer[1]+buffer[2]+buffer[3];
	buffer[5]=buffer[4]+buffer[4];
#ifndef WIN32
	write(fd,buffer,6);
#else
	writeToSerialPort(h,buffer,6);
#endif
}

void dmxusb_open_port(char *serial_name){
#ifndef WIN32
	fd=open(serial_name,O_RDWR | O_NOCTTY);
	if (fd<0) {
		//chyba otevreni portu
		#ifdef DEBUG
			printf("chyba otevreni portu");
		#endif

	}

#else
	//printf("inside open port: %s",serial_name);
	h = openSerialPort(serial_name,B19200,one,off);
	//h = openSerialPort(serial_name,B9600,one,off);

	//Sleep(100);
#endif
#ifndef WIN32
		#ifdef DEBUG
			printf("port ready\n");
		#endif
		tcgetattr(fd,&oldtio);
        memset(&newtio,0,sizeof(newtio));
        newtio.c_iflag=0;
        newtio.c_oflag=~OPOST;
        newtio.c_cflag=CS8 | CSTOPB | CLOCAL | CREAD;
        newtio.c_lflag=0;
        newtio.c_cc[VTIME]=0;
        newtio.c_cc[VMIN]=0;
        if (tcsetattr(fd,TCSANOW,&newtio)==-1) {
                close(fd);
				#ifdef DEBUG
					printf("port setting error\n");
				#endif
        }
        tcflush(fd,TCIFLUSH);
        tcflush(fd,TCOFLUSH);
		#ifdef DEBUG
			printf("port settings done\n");
		#endif
#endif	

}


int file_exist (char *filename)
{
  struct stat status;   
  return (stat(filename, &status) == 0);
}


int get_uid(){

	buffer[0]=PACKET_START;
	buffer[1]=RDM_UID;
	buffer[2]=0;
	buffer[3]=0;
	buffer[4]=buffer[0]+buffer[1]+buffer[2]+buffer[3];
	buffer[5]=buffer[4]+buffer[4];
	
	
	#ifndef WIN32
		write(fd,buffer,6+size);
		struct timespec tim, tim2;
		tim.tv_sec = 0;
		tim.tv_nsec = 100000000L;
		nanosleep(&tim,&tim2);
	#else
	    writeToSerialPort(h,buffer,size+6);
		Sleep(100);
	#endif
	


int success=0;
int bytes;

#ifndef WIN32
fd_set read_fds, write_fds, except_fds;
FD_ZERO(&read_fds);
FD_ZERO(&write_fds);
FD_ZERO(&except_fds);
FD_SET(fd, &read_fds);


struct timeval timeout;
timeout.tv_sec = 1;
timeout.tv_usec = 0;
if (select(fd + 1, &read_fds, &write_fds, &except_fds, &timeout)>0)
{
//printf("select OK\n");
if (FD_ISSET(fd, &read_fds)) {
	ioctl(fd, FIONREAD, &bytes);
				unsigned char data[bytes];
				//printf("bytes ready: %d\n",bytes);
				read(fd,data,bytes);
				//printf("read bytes: %d\n",read(fd,data,bytes));
				if (data[0]==0xa5){
					if (data[1]==0x25){
						success=1;
						if(data[7]==0x02){
							sprintf(serial_alias,"RUNIT WTX");
	
	}
						else if(data[7]==0x01){
							sprintf(serial_alias,"RUI");
						}
					}}


}

}
#else
			bytes=11;
			unsigned char data[bytes];
			//printf("reding bytes ready: %d\n",bytes);
			readFromSerialPort(h,data,bytes);
			//printf("read bytes: %d\n",read(fd,data,bytes));
			if (data[0]==0xa5){
				if (data[1]==0x25){
					success=1;
					if(data[7]==0x02){
							sprintf(serial_alias,"RUNIT WTX");
					}
					else if(data[7]==0x01){
							sprintf(serial_alias,"RUI");
					}
				}}




#endif

    // fd is ready for reading

else
{
    // timeout or error
	//printf("error\n");
}
return success;
}



void init_artnet()

{

  char *ip_addr = NULL;
  int optc, subnet_addr = 0, port_addr = 0;
  int bcast_limit = 0;
  int an_sd;
  /* set up artnet node */
  node = artnet_new(ip_addr, verbose);;
  
  if(node == NULL) {
	printf ("Unable to set up artnet node: %s\n", artnet_strerror() );
 //return 1;
  }

  // set names and node type
  artnet_set_short_name(node, "TRAINER");
  artnet_set_long_name(node, "The calibRAtIoN submittER");
  artnet_set_node_type(node, ARTNET_SRV);

  artnet_set_subnet_addr(node, subnet_addr);

  // enable the first input port (1 universe only)
  artnet_set_port_type(node, 0, ARTNET_ENABLE_INPUT, ARTNET_PORT_DMX);
  artnet_set_port_addr(node, 0, ARTNET_INPUT_PORT, port_addr);
  artnet_set_bcast_limit(node, bcast_limit);

  //start the node
  //artnet_start(node);
  if (artnet_start(node) != ARTNET_EOK) {
    printf("Failed to start: %s\n", artnet_strerror() );
    //goto error_destroy;
//	return 1;
  }

  // store the sds
  an_sd = artnet_get_sd(node); //all as per examples
}


void find_port(){

static int i=0; //port iterator


#ifdef WIN32
	#define ITER 100
#else
	#define ITER 1
#endif

if ((i<ITER) && (!uid)){

#ifdef WIN32
	if (i<10){
		sprintf(serial_name,"COM%d",i);
	}else{
		sprintf(serial_name,"\\\\.\\COM%d",i);
	}

		printf(".");
		//printf("opening port %s\n",serial_name);
		dmxusb_open_port(serial_name);
		uid=get_uid();
		
		if (uid){ //>0
			#ifdef DEBUG
				printf("\n");
				printf("found port: %s\n", serial_name);
				printf("found device: %s\n",serial_alias);
			#endif
			mvwprintw(w,14,1,"                                         ");
			mvwprintw(w,14,1,"%s %s",uid ? serial_name:"", serial_alias);
	//		break;
		
		}else{
			//printf("UID failed");
		
		}
#else
	
	glob_t glob_result;
	#ifdef __APPLE__
		glob("/dev/cu.usbserial*",GLOB_TILDE,NULL,&glob_result);
	#else
		glob("/dev/ttyUSB*",GLOB_TILDE,NULL,&glob_result);
	#endif

printf("number of serial ports: %d\n",glob_result.gl_pathc);
unsigned int j=0; 
	while (j<glob_result.gl_pathc){ //iterate fast on UNIX as we have stat and can fail quickly
	sprintf(serial_name,"%s",glob_result.gl_pathv[j]);
	j++;
	if (file_exist(serial_name))
		{
		//printf("opening port %s\n",serial_name);
		dmxusb_open_port(serial_name);
		uid=get_uid();
		
		if (uid){ //>0
			#ifdef DEBUG
				printf("\n");
				printf("found port: %s\n", serial_name);
				printf("found device: %s\n",serial_alias);
			#endif
			mvwprintw(w,14,1,"                                         ");
			mvwprintw(w,14,1,"%s %s",uid ? serial_name:"", serial_alias);
			break;
		
		}else{
			//printf("UID failed");
		
		}

		}
	}

#endif
i++;	
	}else{
	if (!uid){ //uid should be 0 if no serial
		#ifdef ARTNET
			sprintf(serial_alias,"Art-Net Output enabled");
			mvwprintw(w,14,1,"                                         ");
			mvwprintw(w,14,1,"%s",serial_alias);
			init_artnet();
			uid=1;
		#else
			sprintf(serial_alias,"No DMX device found and Art-Net disabled");			
			mvwprintw(w,14,1,"                                         ");
			mvwprintw(w,14,1,"%s",serial_alias);
		#endif
	}
}

}


void dmxusb_send_dmx(){
	int i;
	int CRC=0;
	size=MAXCHANNELS;

	buffer[0]=PACKET_START;
	buffer[1]=SEND_DMX;
	buffer[2]=(size) & 0x00ff;
	buffer[3]=((size) >> 8) & 0x00ff;	
	buffer[4]=buffer[0]+buffer[1]+buffer[2]+buffer[3];
	memcpy(&buffer[5],dmx,size);
	for (i=0;i<5+size;i++) CRC+=buffer[i];
	  buffer[5+size]=(CRC & 0x00ff);
	#ifdef DEBUG
		printf("dmx out writted: %d\n",write(fd,buffer,6+size));
	#else
		write(fd,buffer,6+size);
	#endif
	#ifndef WIN32
		write(fd,buffer,6+size);
	#else
	    writeToSerialPort(h,buffer,size+6);
	#endif

}
void conf(){
//set correct fixture type
fixture++;
if (fixture>3){
fixture=1;
}

}

void fill_dmx()
{
//change DMX values - use values from our program and where needed, fill in calibration values from calib array

for(a=0; a<512; a++)
	{
		if (a<70) //really only analyze first 70 channels of each DMX packet
		{	
		int cur_val=program[current_program][program_step][a]; //our program values. re-write them by calibration values where value is 500 - indicating the slot
		if (cur_val>499){
			if (cur_val < 510)
			{
				dmx[a]=calib[0][cur_val-500];
			}else if (cur_val < 520)

			{
			
				dmx[a]=calib[1][cur_val-510];
			
			}else if (cur_val < 530)

			{
			
				dmx[a]=calib[2][cur_val-520];
		
			}else if (cur_val < 540)

			{
			
				dmx[a]=calib[3][cur_val-530];
		

			}else if (cur_val < 550)

			{
			
				dmx[a]=calib[4][cur_val-540];
			}else if (cur_val < 560)

			{
			
				dmx[a]=calib[5][cur_val-550];
			}else if (cur_val < 570)

			{
			
				dmx[a]=calib[6][cur_val-560];
			}else if (cur_val < 580)

			{
			
				dmx[a]=calib[7][cur_val-570];


			}else if (cur_val < 590)

			{
			
				dmx[a]=calib[8][cur_val-580];
				}

		}else{

				dmx[a]=program[current_program][current_step+1][a]; //for other DMX slots, use the program values

					}
			
		}
		else
		{
			//dmx[a]=0x00; //above 70, we don't care about DMX values. Should be 0 by default.
		}
	}
}
void init_calib(){
//initialize all calibration values to 128
//int sample=0; //test sequence to initialize calibrations by sequence of numbers
	for(a=0; a<8; a++)
		{
		for (b=0;b<4;b++)
		{
		calib[a][b]=128;
//		calib[a][b]=sample;
//		sample++;
			}
			}
}



void get_input(int r,int c,char * str,int msg) //get user input for each calibration value
{
	nodelay(w,FALSE); //reenable getch
	echo(); // print characters as typed
	curs_set(2); // show cursor
	mvwprintw(w,r+3,c+19+(c*4),"%s","      ");
	move(r+3,c+21+(c*4));
	getnstr(str,3); //get 3 characters
	int delka=0;
	int num;
	num = atoi(str);
	delka=strlen(str);
	 switch (delka){
		case 0:					   	   //no user input
			calib[menu_r][menu_c]=msg; //keep original value
			menu_c=menu_c+1;
			if(menu_c>3){
				menu_c=0;
				menu_r=menu_r+1;
				if(menu_r>7){
					menu_r=0;
				}
			}
		break;

		default:
		if (num >=0 && num<256)      //check for range
		{
			calib[menu_r][menu_c]=num; //new value
		
			menu_c=menu_c+1;
			if(menu_c>3){
				menu_c=0;
				menu_r=menu_r+1;
				if(menu_r>7){
					menu_r=0;
				}
				}
		}
		else
		{
			//not a valid input, repeat - stay in editable prompt
			get_input(menu_r,menu_c,str,calib[menu_r][menu_c]);
		}

		break;
	 
	 }

	noecho();   //edit is over, set terminal back to non edit mode - catch single char in main loop
	curs_set(0);
	nodelay(w,TRUE); //reenable getch

}

void msleep(long time) {
#ifndef WIN32
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 1000000*time;
  nanosleep(&ts, NULL);
 #else
  Sleep(time);
 #endif
}

// returns the time in milliseconds
unsigned long timeGetTime() {
#ifndef WIN32
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (unsigned long)tv.tv_sec*1000UL + (unsigned long)tv.tv_usec/1000;
#else
  SYSTEMTIME st;
  GetSystemTime(&st);
  return (unsigned long)st.wSecond*1000UL + (unsigned long)st.wMilliseconds;
#endif
}


int do_step() { //loop for DMX sending

if (current_program != 0) { //send DMX only if any program is active

	
  if (program_step!=current_step){ //new step
  
			float fadetime = (float)program[current_program][0][program_step];
			current_step=program_step;

			tstart=timeGetTime();
			tend=tstart+(int)(fadetime*100.0);
			t=tstart;

			sum = 0;
			for (int i = program_step; i < program_length+1; i++) {
			   sum += program[current_program][0][i];
				 }

			tallend=tstart+(sum*100.0);
			
			if (current_step==1)
				tsum=(int)sum;

	}

  if (t<=tend) { //not a new step, keep sending until time is up
    t=timeGetTime();

    int finalm = (tallend-timeGetTime())/6;
    int final = (tallend-timeGetTime())/100;
	
	if (final < 0)
	{
		final = 0;
	}

	//print program progress

//	move(14,0);
//	attron(COLOR_PAIR(1));
	//int percent=((tsum/6)-final-(tend-timeGetTime())/60)*100/(int)(tsum/6);
	//int percent=((tsum/6)-final)*100/(int)(tsum/6);
	int percent=((tsum)-final)*100/tsum;
	//int percent=((tsum)-final);

	//mvwprintw(w,14,1,"%s, Step %02d of %02d (Time %03d of %03d) %03i", fixtures_print[fixture],current_step,program_length, (tend-timeGetTime())/60,final,percent);
	//mvwprintw(w,14,1,"%s, Step %02d of %02d (Time %03d of %03d) %03i", fixtures_print[fixture],current_step,program_length, tsum,final,percent);
	mvwprintw(w,14,1,"%s, Step %02d of %02d                   %03i%%", fixtures_print[fixture],current_step,program_length,percent);


	move(14,0.42*percent);
	chgat(42, COLOR_PAIR(1), 0, NULL);
		
	  fill_dmx(); //fill in 
	  msleep(40); //sleep why? (as per example from artnet libs)
	  
	  //send DMX
	  #ifdef ARTNET
		if (artnet_send_dmx(node,0,MAXCHANNELS, dmx)){
			printf("failed to send: %s\n", artnet_strerror() );
		}
	  #else
		if(uid){
		dmxusb_send_dmx(dmx,MAXCHANNELS);
		}
	  #endif
	  msleep(40); //sleep dtto


    t=timeGetTime(); // get current time, because the last time is too old (due to the sleep)
  } else{

	//next step
  
	program_step++;
	if (program_step > program_length){ //end of program
		current_program=0;
		program_step=0;
		current_step=0;
		//nodelay(w,FALSE);
	    mvwprintw(w,14,1,"Finished                                 ");
		#ifndef ARTNET
			dmxusb_mute_dmx();
		#endif

	}
  
  }
}
  return 0;
}

void draw_screen() { //draw user interface
	int i=0,x,y,z;

	// color schemas
	init_pair(1, 174, 232);
	init_pair(2, COLOR_RED, COLOR_GREEN);
	init_pair(3, COLOR_RED, COLOR_WHITE);
	init_pair(4, COLOR_GREEN, COLOR_WHITE);
	init_pair(5, COLOR_BLUE, COLOR_WHITE);
	init_pair(6, COLOR_BLACK, COLOR_WHITE);
	init_pair(7, 244, COLOR_WHITE);
	init_pair(8, 174,238) ;

	attron(COLOR_PAIR(1));
	mvwprintw(w,0,1,"%s", "  TRAINER     The calibRAtIoN submittER  ");
	attron(COLOR_PAIR(8));
	mvwprintw(w,1,1,      "                                         ");
	mvwprintw(w,1,(45-strlen(fixtures_print[fixture]))/2, "%s", fixtures_print[fixture]);
	mvwprintw(w,2,1,"%s", " Color Temperature  Red Green Blue White ");
	  
	for(y=0; y<sizeof(c_temp)/sizeof(*c_temp); y+=1) //calibration values array UI
		{

		  move(y+3,1);
			attron(COLOR_PAIR(7));
			addstr(" ");
			addstr(c_temp[y]);
			addstr("      ");
			move(y+3,20);

		for(z=0;z<4;z++)
			{
			attron(COLOR_PAIR(z+3));
			if (z==menu_c && y==menu_r){
				attron(COLOR_PAIR(2));
			}
			printw(" %03d ",calib[y][z]);
				}
			addstr("  ");
			}
	
	// bottom buttons
		
	attron(COLOR_PAIR(8));
	mvwprintw(w,12,1,"%s", "Entr: Edit");
//	attron(COLOR_PAIR(8));
    //mvprintw(12,10,"%s", "");
//	attron(COLOR_PAIR(8));
	mvwprintw(w,12,12,"%s", "r: Run");
	mvwprintw(w,12,19,"%s", "t: Test");
	mvwprintw(w,12,27,"%s", "s: Stop");
//	attron(COLOR_PAIR(8));
	mvwprintw(w,12,35,"%s", "q: Quit");
		}



void cleanup() { //on exit
	if(w) {
		resetty();
		endwin();
	}
	#ifdef ARTNET
	  artnet_stop(node);
	#else
	 dmxusb_mute_dmx();
		#ifndef WIN32
			tcsetattr(fd,TCSANOW,&oldtio);
		#else
			closeSerialPort(h);
		#endif
	#endif
}


int main()
{




	find_port();
	//dmxusb_open_port("\\\\.\\COM35");
	//get_uid();
	//mvwprintw(w,18,1,"hledam port");


	int c = 0;
	char strr[3];
	atexit(cleanup);

	/* init curses */
	w = initscr();
	if (!w) {
		printf ("unable to open main-screen\n");
		return 1;
	}
	curs_set(0);
	savetty();
	start_color();
	//attron(COLOR_PAIR(8));
	noecho();
	raw();
	keypad(w, TRUE);
	mousemask(ALL_MOUSE_EVENTS, NULL); //accept mouse events

	MEVENT event;
	init_calib(); //fill calibrations with 128s
	draw_screen();//draw UI
	char e=' ';

	nodelay(w,TRUE);
	mvwprintw(w,14,1,"                                         ");
	mvwprintw(w,14,1,"%s %s",uid ? serial_name:"", serial_alias);
		/* main loop */
		c=0;
		while (c!='q') {
					c=wgetch(w);
					switch (c) { //user input, single character

			case KEY_MOUSE:
			if(getmouse(&event) == OK)
			{	/* When the user clicks left mouse button */
				if(event.bstate & BUTTON1_CLICKED)
				{	
					#ifdef DEBUG
						mvwprintw(w,20,1,"x: %02d, y: %02d",event.x,event.y);
					#endif

					if ((event.y-3)>=0 && (event.y-3)<8) //rows of the calibration table
						{
						if ((event.x-21) >=0 && (event.x-21)<18){//coluns of the calib table
							
							if (((event.x-21)%5)<3){ //is it in valid columns
								menu_c=(event.x-21)/5;	//happy, go, lucky
								menu_r=event.y-3; 		
								draw_screen(); //redraw the screen to draw cursor at new position
								get_input(menu_r,menu_c,strr,calib[menu_r][menu_c]);
							}

						}
						}
				else if (event.y==12){ //row of command buttons
						if ((event.x>0) && (event.x<11)){
								get_input(menu_r,menu_c,strr,calib[menu_r][menu_c]);
						} else if ((event.x>11) && (event.x<18)){ //run
							if (fixture){
								//nodelay(w,TRUE);
								current_program=fixture;
								program_step=1;
								current_step=-1;
								program_length=program[current_program][0][0];
							}else{ //fixture not selected, show error
							
								mvwprintw(w,14,(45-strlen(fixtures_print[fixture]))/2, "%s", fixtures_print[fixture]);
							}
						} else if ((event.x>18) && (event.x<26)){ //test
							if (fixture){
								//nodelay(w,TRUE);
								current_program=4;
								current_step=-1;
								program_step=1;
								program_length=program[current_program][0][0];
							}else{ //fixture not selected, show error
							
								mvwprintw(w,14,(45-strlen(fixtures_print[fixture]))/2, "%s", fixtures_print[fixture]);
							}
						} else if ((event.x>26) && (event.x<34)){ //stop
							current_program=0;
							current_step=-1;
							program_step=1;
							//nodelay(w,FALSE); //reenable getch
							//mvwprintw(w,14,1,"Stopped                                  ");
						} else if ((event.x>34) && (event.x<42)){
							return 0;
						}


					}
				}
			}		
			
			break;


					case KEY_HOME:
					  menu_r=0;
					  menu_c=0;
					  break;
					case KEY_RIGHT:
						menu_c++;
						if(menu_c > 3) {
							menu_c=0;
							menu_r=menu_r+1;
							if (menu_r>7){
								menu_r=0;
							}
						}
					  break;
					case KEY_LEFT:
						menu_c--;
						if(menu_c < 0) {
							menu_c=3;
							menu_r=menu_r-1;
							if (menu_r<0){
								menu_r=7;						
							}
						}
					  break;

					case KEY_DOWN:
					menu_r++;
					if(menu_r>7){
						menu_r=0;
						menu_c=menu_c+1;
						if(menu_c>3){
							menu_c=0;
						}
					}
					break;

			case KEY_UP:
					menu_r--;
					if(menu_r<0){
						menu_r=7;
						menu_c=menu_c-1;
						if(menu_c<0){
							menu_c=3;
						}
					}
			  break;

			  case 'c': //enter
					if (program_step==0){
					conf();
						}
					mvwprintw(w,14,1,"                                         ");
					break;
			  case '\n': //enter

					get_input(menu_r,menu_c,strr,calib[menu_r][menu_c]);
					break;

			  case 's': //stop program
					//setall();
					current_program=0;
					current_step=-1;
					program_step=1;
					//nodelay(w,FALSE); //reenable getch
	    			//mvwprintw(w,14,1,"Stopped                                  ");
					break;
			  case 't': //run test program
					if (fixture){
					//nodelay(w,TRUE);//disable getch while running
					current_program=4;
					program_step=1;
					current_step=-1;
					program_length=program[current_program][0][0];
					}else{ //fixture not selected, show error
						mvwprintw(w,14,(45-strlen(fixtures_print[fixture]))/2, "%s", fixtures_print[fixture]);
					}
					break;
			  case 'r': //run
					if (fixture){
					//nodelay(w,TRUE);
					current_program=fixture;
					program_step=1;
					current_step=-1;
					program_length=program[current_program][0][0];
					}else{ //fixture not selected, show error
					
						mvwprintw(w,14,(45-strlen(fixtures_print[fixture]))/2, "%s", fixtures_print[fixture]);
					}
					break;

					default:
						break;

					}
			find_port();				
			draw_screen(); //draw screen UI each loop after user input
			do_step(); //send DMX loop
			refresh();     //refresh screen
		}
	 return 0;
	}
