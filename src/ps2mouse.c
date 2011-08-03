#include<avr/io.h>
#include<util/delay.h>
#include "pstwo.h"
#include "ps2mouse.h"

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

bool ps2_init_mouse(void) {
	printf("\n Resetting TP");
	tp_reset();
	//return;
	printf("\n Done. ");

	if ( ! send_packet(0xff) )
		return false;
	read_packet(); //Ack
	read_packet(); //Bat
	read_packet(); //dev ID
	//Enable Data reporting
	if ( !send_packet(0xf4) )
		return false;
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

	//Set remote mode
	if( ! send_packet(0xf0) )
		return false;
	read_packet(); //Ack

	return true;
}


/*
 *
 */
void ps2_read_mouse(int *dx, int *dy, uint8_t *BTNS )
{

	uint8_t ack ,LMB,MMB,RMB;
	int mouseinf;
	{
		send_packet(0xeb);
		ack=read_packet(); //Ack
		if(ack==0xfa) 
		{
			mouseinf=read_packet();
			*dx= read_packet();
			*dy= read_packet();

			// raw *dx is of 0xXX: If
			int x = *dx;
			int y = *dy;

			if(mouseinf&0x10)
				*dx-=0x100; // Add sign bit to dx
			if(mouseinf&0x20)
				*dy-=0x100; // Add sign bit to dy

			/*
			if( x!=0 ){
				//printf("\n%4x %4x | %4x %4x", x,*dx,y,*dy);
				printf("\nX %4x %4x: ", x,*dx);
				if(*dx&0xFF00)
					printf("-%d" , 256-(*dx+0x100));
				else
					printf("+%d" , *dx);
			}
			*/

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
		}
	}
}
