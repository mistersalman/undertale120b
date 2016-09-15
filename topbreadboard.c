
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "io.c"
#include "usart_ATmega1284.h"

void A2D_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}
void Set_A2D_Pin(unsigned char pinNum) {
	ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
	// Allow channel to stabilize
	static unsigned char i = 0;
	for ( i=0; i<15; i++ ) { asm("nop"); }
}

unsigned long numin;

void transmit_data(unsigned long data){
	for (int i = 31; i >= 0; i--){
		PORTC = 0x08; PORTC |= ((data >> i) & 0x01);
		PORTC |= 0x04;
	}
	PORTC |= 0x02;
	PORTC = 0x00;
}

void light_up(unsigned char a, unsigned char r, unsigned char g, unsigned char b){
	unsigned char negr = ~r;
	unsigned char negg = ~g;
	unsigned char negb = ~b;
	numin = negb;
	numin = (numin << 8) + negg;
	numin = (numin << 8) + negr;
	numin = (numin << 8) + a;
	transmit_data(numin);
}

unsigned char faceset;
//0 - start
//1 - papyrus
//2 - papyrus combat
//3 - undyne
//4 - undyne combat
//5 - sans
//6 - sans combat
//7 - defeat
//8 - victory

enum FacesGrid{begin, startline1, startline2, startline3, startline4, startline5, startline6,
	startline7, startline8, papyrusline1, papyrusline2, papyrusline3, papyrusline3_war,
	papyrusline4, papyrusline5, papyrusline6, papyrusline6_war, papyrusline7,
	papyrusline7_war, papyrusline8, undyneline1, undyneline2, undyneline3,
	undyneline3_war, undyneline4, undyneline5, undyneline6, undyneline6_war,
	undyneline7, undyneline7_war, undyneline8, sansline1, sansline2, sansline3,
	sansline3_war, sansline4, sansline5, sansline6, sansline6_war, sansline7,
	sansline7_war, sansline8, defeatline1, defeatline2, defeatline3, defeatline4,
	defeatline5, defeatline6, defeatline7, defeatline8, victoryline1, victoryline2,
	victoryline3, victoryline4, victoryline5, victoryline6, victoryline7,
victoryline8} faceGrid;

void TickGrid(){
	switch(faceGrid){
		case begin:
		if(USART_HasReceived(1)){
			faceset = USART_Receive(1);
		}
		if(faceset == 0){faceGrid = startline1;}
		if(faceset == 1 || faceset == 2){faceGrid = papyrusline1;}
		if(faceset == 3 || faceset == 4){faceGrid = undyneline1;}
		if(faceset == 5 || faceset == 6){faceGrid = sansline1;}
		if(faceset == 7){faceGrid = defeatline1;}
		if(faceset == 8){faceGrid = victoryline1;}
		break;
		
		case startline1:
		faceGrid = startline2;
		break;

		case startline2:
		faceGrid = startline3;
		break;

		case startline3:
		faceGrid = startline4;
		break;

		case startline4:
		faceGrid = startline5;
		break;

		case startline5:
		faceGrid = startline6;
		break;

		case startline6:
		faceGrid = startline7;
		break;

		case startline7:
		faceGrid = startline8;
		break;

		case startline8:
		faceGrid = begin;
		break;

		case papyrusline1:
		faceGrid = papyrusline2;
		break;

		case papyrusline2:
		if(faceset == 1){faceGrid = papyrusline3;}
		if(faceset == 2){faceGrid = papyrusline3_war;}
		break;

		case papyrusline3:
		faceGrid = papyrusline4;
		break;

		case papyrusline3_war:
		faceGrid = papyrusline4;
		break;

		case papyrusline4:
		faceGrid = papyrusline5;
		break;

		case papyrusline5:
		if(faceset == 1){faceGrid = papyrusline6;}
		if(faceset == 2){faceGrid = papyrusline6_war;}
		break;

		case papyrusline6:
		faceGrid = papyrusline7;
		break;

		case papyrusline6_war:
		faceGrid = papyrusline7_war;
		break;

		case papyrusline7:
		faceGrid = papyrusline8;
		break;

		case papyrusline7_war:
		faceGrid = papyrusline8;
		break;

		case papyrusline8:
		faceGrid = begin;
		break;

		case undyneline1:
		faceGrid = undyneline2;
		break;

		case undyneline2:
		if(faceset == 3){faceGrid = undyneline3;}
		if(faceset == 4){faceGrid = undyneline3_war;}
		break;

		case undyneline3:
		faceGrid = undyneline4;
		break;

		case undyneline3_war:
		faceGrid = undyneline4;
		break;

		case undyneline4:
		faceGrid = undyneline5;
		break;

		case undyneline5:
		if(faceset == 3){faceGrid = undyneline6;}
		if(faceset == 4){faceGrid = undyneline6_war;}
		break;

		case undyneline6:
		faceGrid = undyneline7;
		break;

		case undyneline6_war:
		faceGrid = undyneline7_war;
		break;

		case undyneline7:
		faceGrid = undyneline8;
		break;

		case undyneline7_war:
		faceGrid = undyneline8;
		break;

		case undyneline8:
		faceGrid = begin;
		break;

		case sansline1:
		faceGrid = sansline2;
		break;

		case sansline2:
		if(faceset == 5){faceGrid = sansline3;}
		if(faceset == 6){faceGrid = sansline3_war;}
		break;

		case sansline3:
		faceGrid = sansline4;
		break;

		case sansline3_war:
		faceGrid = sansline4;
		break;

		case sansline4:
		faceGrid = sansline5;
		break;

		case sansline5:
		if(faceset == 5){faceGrid = sansline6;}
		if(faceset == 6){faceGrid = sansline6_war;}
		break;

		case sansline6:
		faceGrid = sansline7;
		break;

		case sansline6_war:
		faceGrid = sansline7_war;
		break;

		case sansline7:
		faceGrid = sansline8;
		break;

		case sansline7_war:
		faceGrid = sansline8;
		break;

		case sansline8:
		faceGrid = begin;
		break;

		case defeatline1:
		faceGrid = defeatline2;
		break;

		case defeatline2:
		faceGrid = defeatline3;
		break;

		case defeatline3:
		faceGrid = defeatline4;
		break;

		case defeatline4:
		faceGrid = defeatline5;
		break;

		case defeatline5:
		faceGrid = defeatline6;
		break;

		case defeatline6:
		faceGrid = defeatline7;
		break;

		case defeatline7:
		faceGrid = defeatline8;
		break;

		case defeatline8:
		faceGrid = begin;
		break;

		case victoryline1:
		faceGrid = victoryline2;
		break;

		case victoryline2:
		faceGrid = victoryline3;
		break;

		case victoryline3:
		faceGrid = victoryline4;
		break;

		case victoryline4:
		faceGrid = victoryline5;
		break;

		case victoryline5:
		faceGrid = victoryline6;
		break;

		case victoryline6:
		faceGrid = victoryline7;
		break;

		case victoryline7:
		faceGrid = victoryline8;
		break;

		case victoryline8:
		faceGrid = begin;
		break;
	}

	switch(faceGrid){
		case begin:
		break;
		
		case startline1:
		light_up(128, 0, 3, 192);
		break;

		case startline2:
		light_up(64, 0, 3, 192);
		break;

		case startline3:
		light_up(32, 0, 3, 192);
		break;

		case startline4:
		light_up(16, 0, 3, 192);
		break;

		case startline5:
		light_up(8, 0, 3, 192);
		break;

		case startline6:
		light_up(4, 0, 3, 192);
		break;

		case startline7:
		light_up(2, 255, 3, 192);
		break;

		case startline8:
		light_up(1, 255, 3, 192);
		break;

		case papyrusline1:
		light_up(128, 0, 60, 0);
		break;

		case papyrusline2:
		light_up(64, 0, 126, 0);
		break;

		case papyrusline3:
		light_up(32, 0, 106, 0);
		break;

		case papyrusline3_war:
		light_up(32, 0, 106, 20);
		break;

		case papyrusline4:
		light_up(16, 0, 62, 0);
		break;

		case papyrusline5:
		light_up(8, 0, 62, 0);
		break;

		case papyrusline6:
		light_up(4, 0, 32, 0);
		break;

		case papyrusline6_war:
		light_up(4, 28, 32, 0);
		break;

		case papyrusline7:
		light_up(2, 0, 16, 0);
		break;

		case papyrusline7_war:
		light_up(2, 12, 16, 0);
		break;

		case papyrusline8:
		light_up(1, 0, 12, 0);
		break;

		case undyneline1:
		light_up(128, 0, 0, 126);
		break;

		case undyneline2:
		light_up(64, 0, 0, 255);
		break;

		case undyneline3:
		light_up(32, 0, 0, 90);
		break;

		case undyneline3_war:
		light_up(32, 4, 32, 94);
		break;

		case undyneline4:
		light_up(16, 0, 0, 255);
		break;

		case undyneline5:
		light_up(8, 0, 0, 126);
		break;

		case undyneline6:
		light_up(4, 0, 0, 195);
		break;

		case undyneline6_war:
		light_up(4, 60, 0, 195);
		break;

		case undyneline7:
		light_up(2, 0, 0, 102);
		break;

		case undyneline7_war:
		light_up(2, 24, 0, 102);
		break;

		case undyneline8:
		light_up(1, 0, 0, 60);
		break;

		case sansline1:
		light_up(128, 60, 0, 0);
		break;

		case sansline2:
		light_up(64, 126, 0, 0);
		break;

		case sansline3:
		light_up(32, 90, 0, 0);
		break;

		case sansline3_war:
		light_up(32, 90, 0, 32);
		break;

		case sansline4:
		light_up(16, 126, 0, 0);
		break;

		case sansline5:
		light_up(8, 126, 0, 0);
		break;

		case sansline6:
		light_up(4, 126, 0, 0);
		break;

		case sansline6_war:
		light_up(4, 66, 0, 60);
		break;

		case sansline7:
		light_up(2, 126, 0, 0);
		break;

		case sansline7_war:
		light_up(2, 102, 0, 24);
		break;

		case sansline8:
		light_up(1, 60, 0, 0);
		break;

		case defeatline1:
		light_up(128, 0, 0, 102);
		break;

		case defeatline2:
		light_up(64, 0, 0, 102);
		break;

		case defeatline3:
		light_up(32, 0, 0, 0);
		break;

		case defeatline4:
		light_up(16, 0, 0, 66);
		break;

		case defeatline5:
		light_up(8, 0, 0, 0);
		break;

		case defeatline6:
		light_up(4, 0, 0, 255);
		break;

		case defeatline7:
		light_up(2, 0, 0, 129);
		break;

		case defeatline8:
		light_up(1, 0, 0, 129);
		break;

		case victoryline1:
		light_up(128, 102, 0, 0);
		break;

		case victoryline2:
		light_up(64, 102, 0, 0);
		break;

		case victoryline3:
		light_up(32, 0, 0, 0);
		break;

		case victoryline4:
		light_up(16, 195, 0, 0);
		break;

		case victoryline5:
		light_up(8, 195, 0, 0);
		break;

		case victoryline6:
		light_up(4, 102, 0, 0);
		break;

		case victoryline7:
		light_up(2, 60, 0, 0);
		break;

		case victoryline8:
		light_up(1, 24, 0, 0);
		break;
	}
}

unsigned char request = 0;
unsigned char sendVal = 0;
unsigned short myVal;

enum IRReader{IR_check} IRState;

void IRTick(){
	switch(IRState){
		case IR_check:
		IRState = IR_check;
		break;
	}
	
	switch(IRState){
		case IR_check:
		myVal = ADC;
		if(myVal <= 970){sendVal = 0; PORTB = 0x02;}
		if(myVal >= 970){sendVal = 1; PORTB = 0x01;}
		if(USART_IsSendReady(0)){
			USART_Send(sendVal, 0);
		}
		if(USART_HasReceived(0)){
			request = USART_Receive(0);
		}
		break;
	}
}

int main(void){
		//
		DDRA = 0x00; PORTA = 0xFF;
		//Audio
		DDRB = 0xFF; PORTB = 0x00;
		//Grid
		DDRC = 0xFF; PORTC = 0x00;
		//
		DDRD = 0xFF; PORTC = 0x00;
		
		A2D_init();
		Set_A2D_Pin(0x00);
		
		unsigned long gridtimepass = 0;
		
		TimerSet(1);
		TimerOn();
		
		initUSART(1);
		USART_Flush(1);
		initUSART(0);
		USART_Flush(0);
		
		faceGrid = begin;
		IRState = IR_check;
		
		while(1){

// 				myVal = ADC;
// 				if(myVal >= 970){PORTB = 0x01;}
// 				if(myVal < 970){PORTB = 0x02;}

				if(gridtimepass >= 1){
					TickGrid();
					IRTick();
					gridtimepass = 0;
				}
				while(!TimerFlag);
				TimerFlag = 0;
				gridtimepass++;
		}
}
