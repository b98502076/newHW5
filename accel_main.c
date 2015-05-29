#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro
#include "accel.h"
#include "i2c_master_int.h"
#include "i2c_display.h"
#define CYCLES 10000000
// DEVCFGs here
// DEVCFG0
#pragma config DEBUG = OFF// no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // not boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 40MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz  DIVID BY 4
#pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV   times 20
#pragma config UPLLIDIV = DIV_1 // divide clock after FPLLMUL     divid by 1
#pragma config UPLLEN = ON // USB clock on
#pragma config FPLLODIV = DIV_2 // divide clock by 1 to output on pin

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid
#pragma config PMDL1WAY = 1 // not multiple reconfiguration, check this
#pragma config IOL1WAY = 1 // not multimple reconfiguration, check this
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // controlled by USB module
//	Function Prototypes


void bar(int len);
int main(void) {
    // startup
	__builtin_disable_interrupts();

	// set the CP0 CONFIG register to indicate that
	// kseg0 is cacheable (0x3) or uncacheable (0x2)
	// see Chapter 2 "CPU for Devices with M4K Core"
	// of the PIC32 reference manual
	__builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

	// no cache on this chip!

	// 0 data RAM access wait states
	BMXCONbits.BMXWSDRM = 0x0;

	// enable multi vector interrupts
	INTCONbits.MVEC = 0x1;

	// disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
	DDPCONbits.JTAGEN = 0;

	__builtin_enable_interrupts();

        acc_setup();
        acc_write_register(CTRL2, 0x00);                    //  Set sensitivity 2g
        display_init();
        int length=28;
        while(1){           
            bar(length);
            display_draw();
            
            display_clear();
            }
            
            
        return(0);
}


void bar(int len){
     int start_row=30, start_col=60, col_x, row_y, x, y;
     short accels[3]; // accelerations for the 3 axes
     float slope_x, slope_y;
     int i;
     acc_read_register(OUT_X_L_A, (unsigned char *) accels, 6);
     slope_x= ((float) accels[0])/ ((float) 16000);
     slope_y= ((float) accels[1])/ ((float) 16000);
     col_x=4+slope_x* ((float) len);
     row_y=4+slope_y* ((float) len);
     if (col_x>=0){
     for (x=0; x<=col_x; x++){
          for(i=1; i < 5 ; i++){
             display_pixel_set(start_row+i, start_col+x, 1);
             }
         }
     }
     else{
         for (x=0; x>=col_x; x--){
          for(i=1; i < 5 ; i++){
             display_pixel_set(start_row+i, start_col+x, 1);
             }
         }
     }
     if (row_y>=0){
          for (y=0; y<=row_y; y++){
          for(i=1; i < 5 ; i++){
             display_pixel_set(start_row+y, start_col+i, 1);
             }
         }
     }
     else{
         for (y=0; y>=row_y; y--){
          for(i=1; i < 5 ; i++){
             display_pixel_set(start_row+y, start_col+i, 1);
             }
         }
     }

}