//------------------------------------------------------------------------------------
// pointeur_code.c
//------------------------------------------------------------------------------------
// DATE: 24/03/19
//
// Target: C8051F020
// Tool chain: KEIL Microvision 4
//
//  NOM: THIEBAUT
//
//
//------------------------------------------------------------------------------------
#include "c8051F020.h"
//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------

sfr16 RCAP4 = 0xE4;

sbit pointeur = P1^6;

int a = 0;
int b = 0;

float nb_cycles = 0;
float nombre_boucles = 0;

float t_on = 0;
float t_on_pwm = 0;
float t_off = 0;
float t_off_pwm = 0;

float rapport_cyclique = 0;
float alpha = 0;
float frequence_pwm = 100;
float t_pwm = 0;

void config_Oscillateur(){
	OSCXCN = OSCXCN&0x00;
	OSCXCN = OSCXCN|0x67;  //configuration oscillateur externe freq max
	while((OSCXCN & 0x80)==0) 
	{
		//delay passage de XTVLD a 1
	}
	
	OSCICN=OSCICN|0x08;
}
void config_ISR(){
	
}
void config_GPIO(){
	XBR0 = 0x84 ; //enable UART0 & CP0 
	XBR1 = 0x80 ; //enable SYSCLK
	XBR2 = 0x40 ; //enable Crossbar and CNVSTR
	P1MDOUT = 0x40;
}
void Delay_Millis(unsigned int millis){
	unsigned int i;
	CKCON &= ~0x40;
	
	T4CON = 0;
	
	RCAP4 = 65536 - (22118400 / (12 * 1000));
	
	TL4 = RCAP4L;
	TH4 = RCAP4H;
	
	T4CON |= 0x04;
	
	for (i = 0; i < millis; i++)
	{
		while((T4CON & 0x80) == 0);
		{
			T4CON &= ~0x80;
		}
	}
}
void commande_pointeur(nb_cycles, t_on, t_off){
	for(a = 0; a < nb_cycles; a++){
		pointeur = 1;
		Delay_Millis(t_on);
		pointeur = 0;
		Delay_Millis(t_off);		
	}
}
void commande_pointeur_PWM(float nb_cycles, float t_on, float t_off, float rapport_cyclique){
	alpha = (rapport_cyclique / 100); //calculs différents paramètres
	t_pwm = (1 / frequence_pwm);
	t_on_pwm = (int)(alpha * t_pwm * 1000);
	t_off_pwm = (int)((1 - alpha) * t_pwm * 1000);
	
	nombre_boucles = (int)(t_on	/ (t_on_pwm + t_off_pwm));
	
	for(b = 0; b < nb_cycles; b++){
		commande_pointeur(nombre_boucles, t_on_pwm, t_off_pwm); //partie correspondant à t_on
		Delay_Millis(t_off); //partie correspondant à t_off
	}
}
void main(void){
	EA = 0;
	
	WDTCN = 0xDE; 
	WDTCN = 0xAD;
		
	config_Oscillateur();
	config_GPIO();	
	config_ISR();
	
	EA = 1;
	
	commande_pointeur(4, 1000, 200); //4 cycles, 1s ON, 200ms OFF	
	Delay_Millis(5000);	
	commande_pointeur_PWM(10, 4000, 2000, 75); //10 cycles, 4s ON, 2s OFF à 25%
	
	while(1)
	{				
											
	}
}