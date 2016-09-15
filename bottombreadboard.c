
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"
#include "timer.h"
#include "usart_ATmega1284.h"
// Ensure DDRC is setup as output

unsigned long num_in;
void transmit_data(unsigned long data) {
	int k;
	for(k = 31; k >= 0; --k) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x08;
		// set SER = next bit of data to be sent.
		PORTC |= ((data >> k) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x04;
	}
	// set RCLK = 1. Rising edge copies data from the “Shift” register to the
	PORTC |= 0x02;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}
void light_up(unsigned char a, unsigned char r, unsigned char g, unsigned char light_val){
	unsigned char negr = ~r;
	unsigned char negg = ~g;
	num_in = light_val;
	num_in = (num_in << 8) + negr;
	num_in = (num_in << 8) + negg;
	num_in = (num_in << 8) + a;
	transmit_data(num_in);
}

//Functions;
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}

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
void display_enh_p(signed char enh_val){
		LCD_ClearScreen();
		LCD_DisplayString(1, "Papyrus     /050Royal Cadet     ");
		LCD_Cursor(10); LCD_WriteData((enh_val / 100) + '0');
		LCD_Cursor(11); LCD_WriteData(((enh_val % 100)/10) + '0');
		LCD_Cursor(12); LCD_WriteData((enh_val % 10) + '0');
}
void display_enh_U(signed char enh_val){
		LCD_ClearScreen();
		LCD_DisplayString(1, "Undyne      /100the Undying     ");
		LCD_Cursor(10); LCD_WriteData((enh_val / 100) + '0');
		LCD_Cursor(11); LCD_WriteData(((enh_val % 100)/10) + '0');
		LCD_Cursor(12); LCD_WriteData((enh_val % 10) + '0');
}
void display_enh_s(signed char enh_val)
{
		LCD_ClearScreen();
		LCD_DisplayString(1, "Sans        /250                ");
		LCD_Cursor(10); LCD_WriteData((enh_val / 100) + '0');
		LCD_Cursor(11); LCD_WriteData(((enh_val % 100)/10) + '0');
		LCD_Cursor(12); LCD_WriteData((enh_val % 10) + '0');
	
}
//end of functions
//--------Shared Variables----------------------------------------------------
unsigned char SM2_output = 0x00;
unsigned char SM3_output = 0x00;
unsigned char Map1_array[] = {231,227,227,231,207,207,159,63,63,159,207,231,243,243,231,243,243,243,243};
unsigned char Map2_array[] = {231,227,227,231,207,207,207};
unsigned char Map3_array[] = {16,16,16,31,248,8,8,8,129,66,38,24,24,36,66,129};
unsigned char pause = 0;
int cnt = 0;
unsigned char dam;
int continue_map = 0;
int bonus_flag = 0;
unsigned char n = 1;
int j = 0;
int p = 0;
int t = 0;
int w = 0;
int map_counter = 0;
unsigned char enh;
unsigned char enh_temp;
int update= 0;
int scroll_count = 0;
unsigned char faceset = 0;
int ready = 0;
unsigned char row_led = 0x08;
unsigned char col_led = 0x08;
unsigned char out_val;
unsigned short LR_val;
unsigned short UD_val;
unsigned char player_health = 0xFF;
int attack = 0;
int hit = 0;
unsigned char attack_val = 0x00;
unsigned char mic_val;
int user_select = 2;
int set = 0;
int need_movement = 1;
int count = 0;
int on_hit = 0;
unsigned char player_att = 5;
int set2= 0;
int delay = 0;
unsigned char inf_toggle;
unsigned char button;
unsigned char max_call = 128;
int clock_counter = 0; 
int script_select = 0;
unsigned char chosen;
//--------End Shared Variables------------------------------------------------
typedef struct _task {
	/*Tasks should have members that include: state, period,
	a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

//user defined FSM
//--------------------------------------------------------------------- 
//ADC SELECT
enum SM1_state{LR, UD, switch_set,wait_movement};
int SMTick1(int state){
	switch(state){
		
		/*changes PINA ADC PORTS*/
		case wait_movement:
		if(need_movement == 0){	Set_A2D_Pin(0x02);mic_val = ADC;if(mic_val > 860){light_up(1,0,128,player_health); bonus_flag == 1;}state = wait_movement;break;}
			else if((need_movement == 1)|| (script_select == 1)){state = LR; break;}
				
		case LR:
		/*Set_A2D_Pin(0x00);*/
		if(need_movement == 0 && script_select == 0){state = wait_movement;break;}
		LR_val = ADC;
		state = switch_set;
		break;
		
		
		case switch_set:
		if ((set == 1) && (need_movement == 1)){
			set = 0;
			state = LR;
			
		}
		else if(set == 0 &&(need_movement == 1)){
			set = 1;
			state = UD;
		}
		break;
		
		case UD:
		UD_val = ADC;
		state = switch_set;
		/*light_up(UD_val,0,1,0);*/
		break;

		
		
	}

	switch(state){
		case wait_movement:
		break;
		
		case LR:
		LR_val = ADC;
		/*light_up(LR_val,128,0,0);*/
		break;
		
		case switch_set:                                                                                                                                      
		if(((set == 0 && (need_movement == 1))))
		{
			Set_A2D_Pin(0x01);
		}
		else if(((set == 1) && (need_movement == 1) )){
			Set_A2D_Pin(0x00);
		}
		break;
		
		case UD:
		UD_val = ADC;
		break;
	}
	return state;
}; 
enum SM2_state{display};
int SMTick2(int state){
	switch(state){
		case display:
		if(need_movement == 1){			
		light_up(col_led,0,row_led,player_health);
		break;
		}
	}
	return state;
	};
enum SM3_state{rest};
int SMTick3(int state){
	switch(state){
		case rest:
		if(need_movement == 1 && script_select != 1){
		if(LR_val == 96){break;}
			else if (LR_val < 80 && (col_led != 128)){col_led = col_led << 1; break;}
				else if(LR_val > 106 && (col_led != 1)){col_led  = col_led >> 1; break;}
			break;
				}
				break;
	}
	return state;
};	

enum SM4_state{rest_UD};
int SMTick4(int state){
	switch(state){
		case rest_UD:
		if(need_movement == 1 && script_select != 1){
		if(UD_val == 88){break;}
		else if (UD_val > 98 && (row_led != 128)){row_led = row_led << 1; break;}
		else if(UD_val < 78 && (row_led != 1)){row_led = row_led >> 1; break;}
		}
		break;
		
	}
	return state;
};

enum SM5_state{begin, intro12, intro34, intro56, intro78, intro910, intro1112,
	menu12, menupa, menuun, menusa, papyrus12, papyrus34, papyrus56, papyrus78,
	papyruscombat, papyruswin, undyne12, undyne34, undyne56, undyne78, undyne910,
	undyne1112, undyne1314, undynecombat, undynewin, sans12, sans34, sans56, sans78,
	sans910, sans1112, sans1314, sans1516, sans1718, sans1920, sans2122, sanscombat,
	sanswin, defeat,
				start_map,map1,map2,map3,map4,map5,map6,map7,map8,wait_display, //map1_display
				char_2_map,char_2_map1,char_2_map2,char_2_map3,char_2_map4,char_2_map5,char_2_map6,char_2_map7,
				char_3_map,char_3_map1,char_3_map2,char_3_map3,char_3_map4,char_3_map5,char_3_map6,char_3_map7,
						attack_state,attack_map1, attack_map2, attack_map3,attack_map4,
						player_dead,victory_state}; // Game states
						
						
						
int SMTick5(int state){
	switch(state){

			case begin:if(scroll_count++ == 800){faceset = 0;LCD_ClearScreen();LCD_DisplayString(1, "Long ago, Man & Monster Ruled   ");scroll_count = 0;state = intro12;
					if(USART_IsSendReady(1)){
						USART_Send(faceset,1);
					} break;}
			break;

			case intro12:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "the Earth. One  day, Man turned ");scroll_count = 0;state = intro34;break;}
			break;

			case intro34:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "on Monster. They sealed them    ");state = intro56;scroll_count =0;break;}
			
			break;

			case intro56:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "underground. You are a child who");state = intro78;scroll_count = 0; break;}
			
			break;

			case intro78:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "fell under. Killor BE KILLED!!!!");state = intro910;scroll_count = 0; break;}	
			break;

			case intro910:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "   Undertale:   Selected Fights ");state = intro1112;scroll_count = 0; break;}

			break;

			case intro1112:if(LR_val != 96 &&scroll_count++ == 1000){LCD_ClearScreen();LCD_DisplayString(1, "Please select a character:      ");state = menu12;scroll_count = 0;break;}
			
			break;

			case menu12:
			script_select = 1;
			chosen = ~PIND & 0x20;
			if(LR_val != 96 && (scroll_count++ % 500 == 0)) {faceset = 1;LCD_ClearScreen();LCD_DisplayString(1, "Papyrus         Difficulty: Easy");chosen = ~PIND & 0x20;	if(USART_IsSendReady(1)){
				USART_Send(faceset,1);
			}state = menupa;break;}
			else {state = menu12;break;}
			break;

			case menupa:
			
			chosen = ~PIND & 0x20;
			if(chosen){enh = 50;LCD_ClearScreen();LCD_DisplayString(1, "Hey friend! Whatare we playing  ");state = papyrus12; script_select = 0;scroll_count = 0;break;}
			if(LR_val > 106 ){faceset = 3;if(USART_IsSendReady(1)){
				USART_Send(faceset,1);
			}LCD_ClearScreen();LCD_DisplayString(1, "Undyne          Difficulty: Med ");state = menuun;break;}
			state = menupa;
			break;

			case menuun:
			chosen = ~PIND & 0x20;
			if(chosen){LCD_ClearScreen();enh = 100;LCD_DisplayString(1, "Stop right therehuman! You      ");state = undyne12;script_select = 0;scroll_count = 0;break;}
			if(LR_val <  86 && (scroll_count++ % 300 == 0)){faceset = 1;if(USART_IsSendReady(1)){
				USART_Send(faceset,1);
			}state = menupa;LCD_ClearScreen();LCD_DisplayString(1, "Papyrus         Difficulty: Easy");break;}
			if(LR_val > 106 &&  (scroll_count++ % 300 == 0)){faceset = 5;if(USART_IsSendReady(1)){
				USART_Send(faceset,1);
			}state = menusa;LCD_ClearScreen();LCD_DisplayString(1, "Sans            Difficulty: Hard");break;}
			state = menuun;
			break;

			case menusa:
			chosen = ~PIND & 0x20;
			if(chosen){LCD_ClearScreen();enh = 250;LCD_DisplayString(1, "So...you've     killed everyone ");state = sans12;script_select = 0;scroll_count = 0;break;}
			if(LR_val < 86){faceset = 3;if(USART_IsSendReady(1)){
				USART_Send(faceset,1);}
				 LCD_ClearScreen();LCD_DisplayString(1, "Undyne          Difficulty: Med "); state = menuun;break;}
			state = menusa;
			break;

			case papyrus12:if(scroll_count++ == 800){LCD_ClearScreen();	LCD_DisplayString(1, "today?..........Um, what's with ");state = papyrus34; scroll_count =  0;break;}
			break;

			case papyrus34:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "that look?......Oh. Please don't");state = papyrus56; scroll_count =  0;break;}
			break;

			case papyrus56:if(scroll_count++ == 800){faceset = 2;if(USART_IsSendReady(1)){
				USART_Send(faceset,1);
			}LCD_ClearScreen();LCD_DisplayString(1, "kill me.........................");state = papyrus78; scroll_count =  0;break;}
			break;

			case papyrus78:
			LCD_ClearScreen();
			LCD_DisplayString(1, "Papyrus     /050Royal Cadet     ");
			LCD_Cursor(10); LCD_WriteData(0 + '0');
			LCD_Cursor(11); LCD_WriteData(5 + '0');
			LCD_Cursor(12); LCD_WriteData(0 + '0');
			user_select = 1;need_movement = 1;state = start_map;break;
			

			case papyruscombat:
			break;

			case papyruswin:
			state = menu12;
			break;

			case undyne12:if(scroll_count++ == 800 ){LCD_ClearScreen();LCD_DisplayString(1, "killed Papyrus  and massacred   ");scroll_count = 0;state = undyne34;break;}
			faceset = 3;
			break;

			case undyne34:if(scroll_count++ == 800 ){	LCD_ClearScreen();LCD_DisplayString(1, "the Underground!I can't sit by  ");scroll_count = 0;state = undyne56;break;}
			break;

			case undyne56:if(scroll_count++ == 800 ){LCD_ClearScreen();LCD_DisplayString(1, "and watch any   longer!         ");scroll_count = 0;state = undyne78;break;}
			break;

			case undyne78:faceset = 4; if(scroll_count++ == 800 ){	LCD_ClearScreen();LCD_DisplayString(1, "I, Undyne the   Undying, shall  ");scroll_count = 0;state = undyne910;break;}
			break;

			case undyne910:if(scroll_count++ == 800 ){LCD_ClearScreen();LCD_DisplayString(1, "strike you down,even if it costs");scroll_count = 0;state = undyne1112;break;}
					break;

			case undyne1112:if(scroll_count++ == 800 ){LCD_ClearScreen();LCD_DisplayString(1, "me my LIFE!!                    ");scroll_count = 0;state = undyne1314;break;}
					break;

			case undyne1314:
			user_select = 2;
			need_movement = 1;
			LCD_ClearScreen();
			LCD_DisplayString(1, "Undyne      /100the Undying     ");
			LCD_Cursor(10); LCD_WriteData(1 + '0');
			LCD_Cursor(11); LCD_WriteData(0 + '0');
			LCD_Cursor(12); LCD_WriteData(0 + '0');
			state = start_map;
			break;

			case undynecombat:

			faceset = 4;
			while(enh != 0){state = undynecombat;}
			if(enh == 0){state = undynewin;}
			if(player_health == 0){state = defeat;}
			break;

			case undynewin:
			faceset = 8;
			state = menu12;
			break;

			case sans12:if(scroll_count++ == 800){	LCD_ClearScreen();LCD_DisplayString(1, "in the          underground. All");	scroll_count = 0;state = sans34;break;}
			faceset = 5;
			break;

			case sans34:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "the monsters aredead. I warned  ");	scroll_count = 0;state = sans56;break;}
			break;

			case sans56:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "you about doing this, but I     ");	scroll_count = 0;state = sans78;;break;}
			break;

			case sans78:if(scroll_count++ == 800){	LCD_ClearScreen();LCD_DisplayString(1, "guess it's in   your nature.    ");	scroll_count = 0;state = sans910;break;}
			break;

			case sans910:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "But look on the bright side,    ");scroll_count = 0;state = sans1112;break;}
			break;

			case sans1112:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "It's a beaufitulday, birds are  ");scroll_count = 0;state = sans1314;break;}
			break;

			case sans1314:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "singing,        flowers are     ");scroll_count = 0;state = sans1516;break;}
			break;

			case sans1516:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "blooming, on    days like these,");scroll_count = 0;state = sans1718;break;}
				break;

			case sans1718:if(scroll_count++ == 800){LCD_ClearScreen();LCD_DisplayString(1, "kids like you,..................");scroll_count = 0;state = sans1920;break;}
			break;

			case sans1920:faceset = 6; if(scroll_count++ == 800){user_select = 3;if(USART_IsSendReady(1)){
				USART_Send(faceset,1);
			}LCD_ClearScreen();LCD_DisplayString(1, "SHOULD BE       BURNING IN HELL.");scroll_count = 0;state = sans2122;break;}
			
			break;

			case sans2122:
				LCD_ClearScreen();
				LCD_DisplayString(1, "Sans        /250                ");
				LCD_Cursor(10); LCD_WriteData(2 + '0');
				LCD_Cursor(11); LCD_WriteData(5 + '0');
				LCD_Cursor(12); LCD_WriteData(0 + '0');
				user_select = 3;need_movement = 1;
				state = start_map;
				break;
			break;
		
			

			case sanscombat:
			faceset = 6;
			while(enh != 0){state = sanscombat;}
			if(enh == 0){state = sanswin;}
			if(player_health == 0){state = defeat;}
			break;

			case sanswin:
			faceset = 8;
			state = menu12;
			break;

			case defeat:
			faceset = 7;
			state = menu12;
			break;





	case start_map:
	if(user_select == 1){state = map1;col_led = 1; row_led = 8;j = 0;break;}
	else if (user_select == 2){	state = char_2_map; p = 0;col_led = 8; row_led = 1; break;}
	else if(user_select == 3){state = char_3_map; t = 0; col_led = 1, row_led = 1; break;}
		else {state = start_map; break;}
	
	
	
	
	
	case wait_display:
	if(user_select == 1){state = map1;need_movement = 1;col_led = 1; row_led = 8;j = 0; count = 0;break;}
	else if(user_select == 2){
		state = char_2_map;need_movement = 1;p = 0;col_led = 8; row_led = 1;break;}
		else if(user_select == 3){
			state = char_3_map;need_movement = 1;t = 0; col_led = 1, row_led = 1;break;}
	break;
	
	
	
	
	case map1:state = map2;break;
	case map2:state = map3;break; 
	case map3:state = map4;break;
	case map4:state = map5;break;
	case map5:state = map6;	break;
	case map6:state = map7;	break;
	case map7:state = map8;break;
	case map8:state = map1;break;
	

	
	case attack_state:state = attack_map1;break;
	case attack_map1: state = attack_map2;break;
	case attack_map2: state = attack_map3; break;
	case attack_map3: state = attack_map4; break;
	case attack_map4: state = attack_state;break; 
	
	
	
	case char_2_map:state = char_2_map1;break;
	case char_2_map1: state = char_2_map2; break;
	case char_2_map2:state = char_2_map3;break;
	case char_2_map3:state = char_2_map4;break;
	case char_2_map4:state = char_2_map5;break;
	case char_2_map5:state = char_2_map6;break;
	case char_2_map6:state = char_2_map7;break;
	case char_2_map7:state = char_2_map;break;
	
		
		case char_3_map:state = char_3_map1;break;
		case char_3_map1: state = char_3_map2; break;
		case char_3_map2:state = char_3_map3;break;
		case char_3_map3:state = char_3_map4;break;
		case char_3_map4:state = char_3_map5;break;
		case char_3_map5:state = char_3_map6;break;
		case char_3_map6:state = char_3_map7;break;
		case char_3_map7:state = char_3_map;break;

	case player_dead:
	if(scroll_count++ % 500 == 0){LCD_ClearScreen();
		LCD_DisplayString(1, "You have died.  Redeem yourself.");
		p=t=j= 0;count = 0; on_hit = 0;need_movement = 1; state = menu12;user_select = 0; player_health = 0xFF;break;}
	

	}
	switch(state){
		
			case begin:
			break;


			case intro12:break;

			case intro34:
			break;

			case intro56:
			
			break;

			case intro78:
			
			break;

			case intro910:
		
			break;

			case intro1112:
			
			break;

			case menu12:
			break;

			case menupa:
			break;

			case menuun:
			break;

			case menusa:
			break;

			case papyrus12:
			break;

			case papyrus34:
			
			break;

			case papyrus56:
		
			break;

			case papyrus78:
			break;

			case papyruscombat:
			break;

			case papyruswin:
			break;

			case undyne12:
			break;

			case undyne34:
			break;

			case undyne56:
			break;

			case undyne78:
			break;

			case undyne910:

			break;

			case undyne1112:
			break;

			case undyne1314:

	
			break;

			case undynecombat:

			break;

			case undynewin:
	
			break;

			case sans12:

			break;

			case sans34:

			break;

			case sans56:

			break;

			case sans78:
	
			break;

			case sans910:
		
			break;

			case sans1112:

				break;

			case sans1314:

			break;

			case sans1516:

			break;

			case sans1718:

				break;

			case sans1920:
			break;

			case sans2122:
			break;

			case sanscombat:

			case sanswin:
			break;

			case defeat:
			break;


		
		
		case map1:
		/*if(enh == 25){light_up(1,0,12,player_health);}*/
		if( j == 25){
			need_movement = 0;
			continue_map = 1;
			state = attack_state;
			Set_A2D_Pin(0x02);
			j = 0;
		}
		light_up(1, Map1_array[j],0,player_health);
		if((col_led == 1)&&(row_led & Map1_array[j]) !=0)
		{	
			player_health = player_health >> 1;
			need_movement = 0;
			state = attack_state;Set_A2D_Pin(0x02);
		}
		break;
		
		case map2:
		light_up(2, Map1_array[j+1],0, player_health);
		if((col_led == 2)&&(row_led & Map1_array[j+1]) != 0)
		{
			player_health = player_health >> 1;
			//Set_A2D_Pin(0x02);
			need_movement = 0;
			state = attack_state;Set_A2D_Pin(0x02);
		}
		break;
		
		case map3:
		light_up(4, Map1_array[j+2],0, player_health);
		if((col_led == 4)&&(row_led & Map1_array[j+2]) != 0)
		{
			player_health = player_health >> 1;need_movement = 0;
			//Set_A2D_Pin(0x03);
			state = attack_state;Set_A2D_Pin(0x02);
		}
		break;
		
		case map4:
		light_up(8, Map1_array[j+3],0, player_health);
		if((col_led == 8)&&(row_led & Map1_array[j+3]) != 0)
		{
			player_health = player_health >> 1;need_movement = 0;
			state = attack_state;Set_A2D_Pin(0x02);
		}
		break;
		
		case map5:
		light_up(16, Map1_array[j+4],0, player_health);
		if((col_led == 16)&&(row_led & Map1_array[j+4]) != 0)
		{
			player_health = player_health >> 1;need_movement = 0;
			state = attack_state;Set_A2D_Pin(0x02);
		}
		break;
		
		case map6:
		light_up(32, Map1_array[j+5],0, player_health);
		if((col_led == 32)&&(row_led & Map1_array[j+5]) != 0)
		{
			player_health = player_health >> 1;need_movement = 0;
		
			state = attack_state;Set_A2D_Pin(0x02);
		}
		break;
		
		case map7:
		light_up(64, Map1_array[j+6],0, player_health);;
		if((col_led == 64)&&(row_led & Map1_array[j+6]) != 0)
		{
			player_health = player_health >> 1;need_movement = 0;
			
			state = attack_state;Set_A2D_Pin(0x02);
		}
		break;
		
		case map8:
		light_up(128, Map1_array[j+7],0, player_health);
		if((col_led == 128)&&(row_led & Map1_array[j+7]) != 0)
		{

			player_health = player_health >> 1;need_movement = 0;
			state = attack_state;need_movement = 0;Set_A2D_Pin(0x02);
		}
		break;
		
		
		case char_2_map:
		if(p >= 30){
			need_movement = 0;
			continue_map = 1;
			state = attack_state;Set_A2D_Pin(0x02);
		}
		light_up(Map2_array[p],1,0,player_health);
		if((row_led == 1)&&(col_led & Map2_array[p]) !=0)
		{
			player_health = player_health >> 1;
			need_movement = 0;
			state = attack_state;Set_A2D_Pin(0x02);
			break;
		}
		break;
		
		
		case char_2_map1:
		light_up(Map2_array[p+1],2, 0,player_health);
		if((row_led == 2)&&(col_led & Map2_array[p+1]) !=0)
		{
			player_health = player_health >> 1;
			need_movement = 0;
			state = attack_state;Set_A2D_Pin(0x02);
				break;
		}
		break;
	
		case char_2_map2:
		light_up(Map2_array[p+2],4,0,player_health);
		if((row_led == 4)&&(col_led & Map2_array[p+2]) !=0)
		{
			player_health = player_health >> 1;
			need_movement = 0;
			state = attack_state;Set_A2D_Pin(0x02);
			break;
		}
		break;
		case char_2_map3:
		light_up(Map2_array[p+3],8,0,player_health);
		if((row_led == 8)&&(col_led & Map2_array[p+3]) !=0)
		{
			player_health = player_health >> 1;
			need_movement = 0;
			state = attack_state;Set_A2D_Pin(0x02);
			break;
		}
			break;
		case char_2_map4:
		light_up(Map2_array[p+4],16 ,0,player_health);
		if((row_led == 16)&&(col_led & Map2_array[p+4]) !=0)
		{
			player_health = player_health >> 1;
			need_movement = 0;
			state = attack_state;Set_A2D_Pin(0x02);
			break;
		}
		break;
		
		case char_2_map5:
		light_up(Map2_array[p+5],32 ,0,player_health);
		if((row_led == 32)&&(col_led & Map2_array[p+5]) !=0)
		{
			player_health = player_health >> 1;
			need_movement = 0;Set_A2D_Pin(0x02);
			state = attack_state;break;
		}
		break;
		case char_2_map6:
		light_up(Map2_array[p+6],64 ,0,player_health);
		if((row_led == 64)&&(col_led & Map2_array[p+6]) !=0)
		{
			player_health = player_health >> 1;
			need_movement = 0;
			state = attack_state;Set_A2D_Pin(0x02);
			break;
		}
			break;
		case char_2_map7:
		light_up(Map2_array[p+7],128 ,0,player_health);
		if((row_led == 128)&&(col_led & Map2_array[p+7]) !=0)
		{
			player_health = player_health >> 1;
			need_movement = 0;
			state = attack_state;Set_A2D_Pin(0x02);
			break;
		}
		break;
			
			
			case char_3_map:
			w++;
			if((w  == 100) && (t != 8)){t = t + 8; w = 0;}
			if((w == 100) && (t >= 8)){t = 0;w = 0;}
			if(map_counter++ % 800 == 0){need_movement = 0; map_counter =0;Set_A2D_Pin(0x02);state = attack_state;break;}
// 			if( clock_counter >= 20){
// 				
// 			}
// 			if(clock_counter % 10 == 0){t++;}
			if((col_led == 128)&&(row_led & Map3_array[t]) !=0)
			{
				player_health = player_health >> 1;
				need_movement = 0;
				state = attack_state;Set_A2D_Pin(0x02);
				break;
			}
			light_up(128,Map3_array[t], 0, player_health);
			break;
			
			
			case char_3_map1:
			light_up(64,Map3_array[t+1], 0, player_health);
			if((col_led == 64)&&(row_led & Map3_array[t+1]) !=0)
			{
				player_health = player_health >> 1;
				need_movement = 0;
				state = attack_state;Set_A2D_Pin(0x02);
				break;
			}
			break;
			
			case char_3_map2:
			light_up(32,Map3_array[t+2], 0, player_health);
			if((col_led == 32)&&(row_led & Map3_array[t+2]) !=0)
			{
				player_health = player_health >> 1;
				need_movement = 0;
				state = attack_state;Set_A2D_Pin(0x02);
				break;
			}
			break;
			case char_3_map3:
			light_up(16,Map3_array[t+3], 0, player_health);
			if((col_led == 16)&&(row_led & Map3_array[t+3]) !=0)
			{
				player_health = player_health >> 1;
				need_movement = 0;
				state = attack_state;Set_A2D_Pin(0x02);
				break;
			}
			break;
			case char_3_map4:
			light_up(8,Map3_array[t+4], 0, player_health);
			if((col_led == 8)&&(row_led & Map3_array[t+4]) !=0)
			{
				player_health = player_health >> 1;
				need_movement = 0;
				state = attack_state;Set_A2D_Pin(0x02);break;
				
			}
			break;
			case char_3_map5:
			light_up(4,Map3_array[t+5], 0, player_health);
			if((col_led == 4)&&(row_led & Map3_array[t+5]) !=0)
			{
				player_health = player_health >> 1;
				need_movement = 0;
				state = attack_state;Set_A2D_Pin(0x02);
				break;
			}
			break;
			case char_3_map6:
			light_up(2,Map3_array[t+6], 0, player_health);
			if((col_led == 2)&&(row_led & Map3_array[t+6]) !=0)
			{
				player_health = player_health >> 1;
				need_movement = 0;
				state = attack_state;Set_A2D_Pin(0x02);
				break;
			}
			break;
			case char_3_map7:
			light_up(1,Map3_array[t+7], 0, player_health);
			if((col_led == 1)&&(row_led & Map3_array[t+7]) !=0)
			{
				player_health = player_health >> 1;
				need_movement = 0;
				state = attack_state;Set_A2D_Pin(0x02);
				break;
			}
			break;
			
		
		
		
		
		case attack_state:
		count++;
		if(player_health == 0){
			light_up(0,0,0,player_health);
			need_movement = 0;
			j = p = t = 0;
			faceset = 7;
			if(USART_IsSendReady(1)){USART_Send(faceset, 1);}
			state = player_dead;
			break;
		}
		if(enh <= 0 && user_select == 1 ){p=t=j= 0;count = 0; on_hit = 0;need_movement = 1;user_select = 0; player_health = 0xFF;faceset = 8; if(USART_IsSendReady(1)){USART_Send(faceset,1);}	LCD_ClearScreen();LCD_DisplayString(1, "I...I thought wewere friends....");state = intro1112;break;}
		if(enh <= 0 && user_select == 2){p=t=j= 0;count = 0; on_hit = 0;need_movement = 1;user_select = 0; player_health = 0xFF;faceset = 8; if(USART_IsSendReady(1)){USART_Send(faceset,1);}	LCD_ClearScreen();LCD_DisplayString(1, "I've failed.    Forgive me Sans.");state = intro1112;break;}
		if(enh == 0 && user_select == 3){p=t=j= 0;count = 0; on_hit = 0;need_movement = 1;user_select = 0; player_health = 0xFF;faceset = 8; if(USART_IsSendReady(1)){USART_Send(faceset,1);}	LCD_ClearScreen();LCD_DisplayString(1, "Well, guess I'velost. hahahaha..");state = intro1112;break;}
		else if(count == 1000){state = wait_display;need_movement = 1; count = 0;
			j = 0;
			p =0;
			t = 0;	
			break;}
		else if(need_movement == 1){
			state = wait_display;
			j = 0;
			p =0;
			t = 0;update = 1;
			break;
		}
		Set_A2D_Pin(0x02);mic_val = ADC;
		if(mic_val > 860){light_up(1,0,128,player_health); bonus_flag == 1; LCD_WriteData(1 +'0');}
		light_up(255,8,0, player_health);
		break;
		
		case attack_map1:
		break;
		
		case attack_map2:
		light_up(255,16,0, player_health);
		break;
		case attack_map3:
		light_up(255,64,0, player_health);
		break;
		case attack_map4:
		light_up(255,2,0, player_health);
		break;

		case wait_display: 
		break;
				
				
				
		case player_dead:
		break;
		
		
	}
	return state;
}
enum  SMTick_state{blink};
  int SMTick6(int state){
  	switch(state){
		  case blink:
		  if(user_select == 1 && (clock_counter++ % 1000) == 0){ j++;}
		  if (user_select == 2 && (clock_counter++ % 500) == 0){p++;}
			  if(user_select == 3 && clock_counter++ % 100 == 0){map_counter++;}
		 
		  break;

	}
	return state;
}

enum SM8_state{start_hit, attack_swipe_wait,calc_hit,attack_reader,wait_1_sec};
int SMTick8(int state){
	
	switch(state){
	case start_hit:
		if(USART_HasReceived(0)){
			inf_toggle = USART_Receive(0);
		}
		//bu
	if(need_movement == 0 && (player_health != 0)){state = attack_swipe_wait;Set_A2D_Pin(0x02);}
	else {state = start_hit; }break;
		
	case attack_swipe_wait:
	if(USART_HasReceived(0)){
		inf_toggle = USART_Receive(0);
	}
	//button = ~PIND & 0x20;//read inf here;
	if(inf_toggle == 1){ state = calc_hit; break;}
	else if(need_movement == 0){state = attack_reader; break;}
	break;
	
	case attack_reader:
	Set_A2D_Pin(0x02);mic_val = ADC;
	if(mic_val > 860){bonus_flag == 1;}
	/*button = ~PIND & 0x20;*/
	if(USART_HasReceived(0)){
		inf_toggle = USART_Receive(0);
	}
	if(inf_toggle != 1 && (cnt % 200 == 0) && (need_movement == 0)){max_call = max_call >> 1;cnt = 0; state = attack_reader;break;}
	if(inf_toggle == 1 && (need_movement == 0) || (max_call == 0)){state = calc_hit;cnt = 0;break;}
	break;		
	
	case calc_hit:
	Set_A2D_Pin(0x02);mic_val = ADC;
	if(mic_val > 860){light_up(1,0,128,player_health); bonus_flag == 1;}
	if(max_call == 1 || max_call == 128){
		dam = 0;state = wait_1_sec;if(dam > enh){enh_temp = 0;}if(dam <= enh){enh_temp = enh - dam;}
		enh = enh_temp;	break;}
	if(max_call == 2 || max_call == 64){
		dam = 10;	
		if(bonus_flag == 1){dam = dam*2;}
		enh_temp = enh - dam;state = wait_1_sec;enh = enh_temp;break;}
	if(max_call == 4 || max_call == 32){dam = 0;state = wait_1_sec; if(dam > enh){enh_temp = 0;}if(dam <= enh){enh_temp = enh - dam;}
		enh = enh_temp;	break;}
	if(max_call == 8 || max_call == 16){
		dam = 25;
		if(bonus_flag == 1){dam = dam*2;}state = wait_1_sec; if(dam > enh){enh_temp = 0;}if(dam <= enh){enh_temp = enh - dam;} enh = enh_temp;break;}
	if(max_call == 0){state = wait_1_sec;	dam = 0; if(dam > enh){enh_temp = 0;}if(dam <= enh){enh_temp = enh - dam;}
		enh = enh_temp;break;}
	state = wait_1_sec;	
	break;
	
	case wait_1_sec:
	light_up(255,0,max_call,player_health);
	if(cnt == 1000){
		need_movement = 1;
		max_call = 128;
		cnt = 0;
		enh = enh;update = 1;
		state = start_hit;}
	break;
		
		}
		switch(state){
			case attack_reader:
			enh = enh;
			light_up(255,0,max_call,player_health);
			cnt++;
			/*Set_A2D_Pin(0x02);*/
			/*if(ADC < 889){bonus_flag = 1;}*/
			break;
			
			case calc_hit:	
			
			break;
			
			case wait_1_sec:
			Set_A2D_Pin(0x02);mic_val = ADC;
			if(mic_val > 860){bonus_flag == 1;}
			enh = enh;
			cnt++;
			break;
			
			case start_hit: enh = enh;break;
		}
	return state;
	};












enum SM7_state{update_enh};
int SMTick7(int state)
{
	switch(state){
		case update_enh: 
		if(update == 1 && user_select == 1){display_enh_p(enh_temp);update = 0;break;}
			else if(update == 1 && user_select == 2){display_enh_U(enh_temp);update = 0;break;}
				else if(update == 1 && user_select == 3){display_enh_s(enh_temp);update = 0;break;}
			break;
			
	
	}
	return state;
}

int main(void)
{	
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xFF; PORTC = 0x00; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xDF; PORTD = 0x20;
	//periods:
	unsigned long int SMTick1_calc = 1;
	unsigned long int SMTick2_calc = 1;
	unsigned long int  SMTick3_calc = 180;
	unsigned long int SMTick4_calc = 180;
	unsigned long int SMTick5_calc = 1;
	unsigned long int SMTick6_calc = 1;
	unsigned long int SMTick7_calc = 1;
	unsigned long int SMTick8_calc = 1;
	unsigned long int SMTick9_calc = 500;
	A2D_init();
	LCD_init();
	//Finding GCD
	//unsigned long tmpGCD = 1;
	//tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
	//GREATED COMMNON DIVIOR FOR ALL TASK OR
	unsigned long int GCD = 1;
	
	unsigned long int SM1Tick1_period = SMTick1_calc;
	unsigned long int SM1Tick2_period = SMTick2_calc;
	unsigned long int SM1Tick3_period = SMTick3_calc;	
	unsigned long int SM1Tick4_period = SMTick4_calc;	
	unsigned long int SM1Tick5_period = SMTick5_calc;
	unsigned long int SM1Tick6_period = SMTick6_calc;
	unsigned long int SM1Tick7_period = SMTick7_calc;
	unsigned long int SM1Tick8_period = SMTick8_calc;
	unsigned long int SM1Tick9_period = SMTick9_calc;
	
	
	//ARRAY OF TASKS
	static task task1, task2, task3, task4,task5,task6,task8,task7;
	task *tasks[] = {&task1, &task2, &task3,&task4,&task5,&task6,&task8,&task7};
	const unsigned short numTasks = 8;
	// 	//task7
	task7.state = update_enh;
	task7.period = SM1Tick7_period;
	task7.elapsedTime = SM1Tick7_period;
	task7.TickFct = &SMTick7;

	//for task 1
	task1.state = wait_movement;
	task1.period = SM1Tick1_period;
	task1.elapsedTime = SM1Tick1_period;
	task1.TickFct = &SMTick1;
	//task 2
	task2.state = display;
	task2.period = SM1Tick2_period;
	task2.elapsedTime = SM1Tick2_period;
	task2.TickFct = &SMTick2;
	//task 3
	task3.state = rest;
	task3.period = SM1Tick3_period;
	task3.elapsedTime = SM1Tick3_period;
	task3.TickFct = &SMTick3;
	//task4
	task4.state = rest_UD;
	task4.period = SM1Tick4_period;
	task4.elapsedTime = SM1Tick4_period;
	task4.TickFct = &SMTick4;
	//TASK5
	task5.state = begin;
	task5.period = SM1Tick5_period;
	task5.elapsedTime = SM1Tick5_period;
	task5.TickFct = &SMTick5;
	//task6
	task6.state = blink;
	task6.period = SM1Tick6_period;
	task6.elapsedTime = SM1Tick6_period;
	task6.TickFct = &SMTick6;
	
	//task8
	task8.state = start_hit;
	task8.period = SM1Tick8_period;
	task8.elapsedTime = SM1Tick8_period;
	task8.TickFct = &SMTick8;
	
		
	// 	//task9
// 	task9.state = begin;
// 	task9.period = SM1Tick9_period;
// 	task9.elapsedTime = SM1Tick9_period;
// 	task9.TickFct = &SMTick9;
// 				
	
	TimerSet(GCD);
	TimerOn();
	initUSART(1);
	initUSART(0);
	USART_Flush(1);
	USART_Flush(0);
	
		
	unsigned short i; 
	while(1) {
		
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	return 0;
}
	

