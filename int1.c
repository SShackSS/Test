#include <stdio.h>
#include <ctype.h>
#include <dos.h>

/*
 *  MSCDEX Monitor,
 *
 *  Operation:
 *	This is a TSR that inserts itself before the real MSCDEX interrupt
 *	service routines. Basically, it appends to the file C:\MSCDEX.LOG a
 *	register dump of the interrupt. This allows us to spy on applications
 *	to figure out which MSCDEX functions are being used.  The logfile 
 *	contains the time of the interrupt (in HH:MM:SS format) plus the
 *	ax,bx,cx and dx register contents.
 *
 *  How to load:
 *	After MSCDEX is loaded, you can load this either by running it, or
 *	load it into high memory using loadhigh. Eg: loadhigh mscmon
 *
 *  Compiler Used:
 *	Borland C++, 3.1
 *
 *  Compile: bcc -1 -ml mscmon.c
 *
 *  Test History:
 *	Works okay on 486's and 386's. I haven't tried it out on any 286's yet.
 *
 *  Author: 	Colin Ian King, Templeman Library, University of Kent.
 *
 *  Modification Record:
 *
 *   23/02/93	Original Code.
 *   24/02/93	Tidy up and release to public domain.
 *   24/02/93   For a compact TSR, I have hard coded the name of the log file.
 *		The original had the filename passed as a command line argument
 *		but it makes the code a bit bigger.
 *		
 */


#define INT2F		0x2f
#define LOGFILE		"c:\\mscdex.log"	

/*
 *  Reduce heap and stack to make program more compact.
 */
extern unsigned _heaplen = 512; 
extern unsigned _stklen  = 256;

void	exit(int);

/*
 *  Pointer to MSCDEX service routine.
 */
static void interrupt (*oldfunc)(unsigned bp, unsigned di, unsigned si,
			         unsigned ds, unsigned es, unsigned dx,	
			         unsigned cx, unsigned bx, unsigned ax);


void interrupt  monitor(unsigned bp, unsigned di, unsigned si,
			  unsigned ds, unsigned es, unsigned dx,	
			  unsigned cx, unsigned bx, unsigned ax)
{
	/*
	 *  If AH = 15, then it is a MSCDEX service, so write to the
	 *  logfile the time and a short register dump. Crude but
	 *  effective.
	 */
	if ((ax & 0xff00) == 0x1500) { 
		FILE *fp;
		if ((fp = fopen(LOGFILE,"a")) != NULL) {
			struct dostime_t t;
			_dos_gettime(&t);
			fprintf(fp,"%2d:%2d:%2d int 2f ax=%04x bx=%04x cx=%04x dx=%04x\n",t.hour,t.minute,t.second,ax,bx,cx,dx);
			fclose(fp);
		}
	} 
	/*
	 *  Now execute the REAL MSCDEX Service routine.
	 */
	_chain_intr(oldfunc);
}


/* 
 *  Wedge in the monitor function into int 2f, and
 *  keep the code resident
 */
int
main()
{
	fprintf(stdout,"MSCDEX Monitor\n");

	oldfunc = _dos_getvect(INT2F);
	_dos_setvect(INT2F,monitor);
	/*
	 *  If this does not work, alter the + 10 to something bigger. It
	 *  is a magic size of the program when resident in memory.
	 */
	_dos_keep(0, (_SS + 10 + (_SP/16) - _psp));

	return 0;
}
