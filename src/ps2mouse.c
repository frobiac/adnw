#include<avr/io.h>
#include<util/delay.h>
#include "pstwo.h"

//#define DELX (dapack[1]-(((dapack[0] & (1 << 4)) >> 4) << 8))
//#define DELY (dapack[2]-(((dapack[0] & (1 << 5)) >> 5) << 8))


void led(uint8_t n) {
	DDRD |= (1 << 6);
	if(n==0)
		PORTD &= ~(1 << 6);
	else if(n==1)
		PORTD |= (1 << 6);
	return;
}

void ps2_init_mouse(void) {

	send_packet(0xff);
	read_packet(); //Ack
	read_packet(); //Bat
	read_packet(); //dev ID
	////
	send_packet(0xf4); //Enable Data reporting
	read_packet();	// Ack
	////
	//send_packet(0xe8); //Set Resolution
	//read_packet(); //Ack
	//send_packet(0x01); //8counts/mm
	//read_packet(); //Ack
	////
	//send_packet(0xf3); //SetSample rate
	//read_packet(); //Ack
	//send_packet(0x64); //200 smaples a second
	send_packet(0xf0); //Set remote mode
	read_packet(); //Ack
}
void ps2_read_mouse(int *dx, int *dy, uint8_t *BTNS ) {


	//init_mouse();
	uint8_t i, d, dapack[3];
	uint8_t ack ,LMB,MMB,RMB;
	int mouseinf,posx,posy;

//	while(1) 
	{
		send_packet(0xeb);
		ack=read_packet(); //Ack
		if(ack==0xfa) 
		{
			mouseinf=read_packet();
			*dy= read_packet();
			*dx= read_packet();
			
	
			if(mouseinf&0x10)
				*dx-=0x100; // Add sign bit to dx
			if(mouseinf&0x20)
				*dy-=0x100; // Add sign bit to dy
			
			
			posx += dx;   // Absolute X position
			posy += dy;   // Absolute Y position

	
		        LMB=0;MMB=0;RMB=0;
/*	
			if(mouseinf&0x01) { //0x09
				LMB=1;         // Get leftbutton status
			}
			else 
				LMB=0;


			if(mouseinf&0x02) {
				RMB=1;        // Get rightbutton status
			}
			else 
				RMB=0;
	
			//if(mouseinf == 0x0B ) {
			if(mouseinf& 0x04 ) {
				MMB=1;       // Get middlebutton status
			}
			else 
				MMB=0;

			// emulate 3 buttons
			if( RMB & LMB ) {
				MMB=1;
				RMB=LMB=0;
			}
*/			
			*BTNS = (LMB<<3) | (MMB<<2) | (RMB << 1);
			/*
			pdec(LMB);
			pdec(MMB);
			pdec(RMB);
			
			print(" ");
			pdec16(posx);print(",");pdec16(posy);print(" ");
			phex(mouseinf);print("\t");pdec(dx);print("\t");pdec16(dy);print("  "); 
			*/
		}
	}


	/*
	   while(1) 
	   {
	   send_packet(0xeb);
	   d=read_packet(); //Ack
	   pdec16(d);

	   for(i=0;i<3;i++)
	   {
	   d=read_packet();
	   print(" ");
	   pdec16(d);
	   dapack[i]=d;
	   }
	   print("\n");
	   led(dapack[0] & (1 << 0));
	   }
	 */
}
/*
int ps2main(void) {

	// set for 16 MHz clock, and make sure the LED is off
	CPU_PRESCALE(0);

	// initialize the USB, but don't want for the host to
	// configure.  The first several messages sent will be
	// lost because the PC hasn't configured the USB yet,
	// but we care more about blinking than debug messages!
	usb_init();
	_delay_ms(1000);
	led(1);

	print("ON ");


	init_mouse();
	uint8_t i, d, dapack[3];
	uint8_t ack , LMB,MMB,RMB ;
	int dx,dy,mouseinf,posx,posy;

	while(1) 
	{
		send_packet(0xeb);
		ack=read_packet(); //Ack
		if(ack==0xfa) 
		{
			print("\n");

			mouseinf=read_packet();
			dx=read_packet();
			dy=read_packet();
			
	
			if(mouseinf&0x10)
				dx-=0x100; // Add sign bit to dx
			if(mouseinf&0x20)
				dy-=0x100; // Add sign bit to dy
			
			
			posx += dx;   // Absolute X position
			posy += dy;   // Absolute Y position

	
			
			if(mouseinf&0x01) { //0x09
				LMB=1;         // Get leftbutton status
			}
			else 
				LMB=0;


			if(mouseinf&0x02) {
				RMB=1;        // Get rightbutton status
			}
			else 
				RMB=0;
	
			//if(mouseinf == 0x0B ) {
			if(mouseinf& 0x04 ) {
				MMB=1;       // Get middlebutton status
			}
			else 
				MMB=0;

			// emulate 3 buttons
			if( RMB & LMB ) {
				MMB=1;
				RMB=LMB=0;
			}
	
			pdec(LMB);
			pdec(MMB);
			pdec(RMB);
			
			print(" ");
			pdec16(posx);print(",");pdec16(posy);print(" ");
			phex(mouseinf);print("\t");pdec(dx);print("\t");pdec16(dy);print("  "); 
		}
	}


}

*/
