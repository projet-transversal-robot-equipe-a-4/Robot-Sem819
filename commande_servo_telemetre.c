//------------------------------------------------------------------------------------
// commande_servo_telemetre.c
//------------------------------------------------------------------------------------
// DATE: 21/03/19
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
unsigned int i;
unsigned int valeur_micro = 0;
unsigned int temps_ecoule;
unsigned int distance;
float valeur_autoreload = 0;
int condition_timer4 = 0;

sfr16 RCAP4 = 0xE4;
sfr16 TMR3 = 0x94;
sbit out_servo = P1^6;
sbit TRIG = P1^7;

void config_INT(){
	EIE2 |= 0x04; //Enable interruption 16 timer 4 overflow
	EIE1 = 0x30; //INT RISING FALLING COMP0
}
void config_Oscillateur()
{
	OSCXCN = OSCXCN & 0x00;
	OSCXCN = OSCXCN | 0x67;  //configuration oscillateur externe freq max
	while((OSCXCN & 0x80)==0) 
	{
		//delay passage de XTVLD a 1
	}
	
	OSCICN = OSCICN | 0x08;
}
void config_Comparator(){
	CPT0CN = 0x80;
}
void config_GPIO(){
	XBR0 = 0x04 ; //enable UART 
	XBR1 = 0x80 ; //enable SYSCLK
	XBR2 = 0x40 ; //enable Crossbar
	P1MDOUT |= 0xC0; //P1.6 et P1.7 en sortie
	out_servo = 0;
}
void config_Timer_3(){
	TMR3CN = 0x02;
}
void config_Timer_4(){
	T4CON &= ~0xC3;
	RCAP4 = 62771; //De base position au centre (0°)
	TL4 = RCAP4L;
	TH4 = RCAP4H;
	T4CON |= 0x04;
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
void Delay_Micro(unsigned int micros){
	unsigned int i;
	CKCON &= ~0x40;
	
	T4CON = 0;
	
	RCAP4 = 65536 - (22118400 / (12 * 1000000));
	
	TL4 = RCAP4L;
	TH4 = RCAP4H;
	
	T4CON |= 0x04;
	
	for (i = 0; i < micros; i++)
	{
		while((T4CON & 0x80) == 0);
		{
			T4CON &= ~0x80;
		}
	}
}
void RISING(void) interrupt 11 //Sur front montant de COMPARATOR --> Mettre 2.5V sur C27 (echo connecté au comparateur)
{
	TMR3 = 0x0000;
	TMR3CN = 0x00;
	TMR3CN |= 0x04; //Lance timer 3
	CPT0CN &= 0x80; //RAZ Drapeau comparateur
}
void FALLING(void) interrupt 10 //Sur front descendant pin ECHO (front descendant de comparator)
{
	TMR3CN &= ~0x04; //Stop timer 3
	CPT0CN &= 0x80; //RAZ Drapeau comparateur
	temps_ecoule = (unsigned int)(TMR3) * 13 / 24;
	distance = temps_ecoule / 58;
}
void ISR_TIMER4(void) interrupt 16 //Routine interruption overflow timer 4
{ 
	if (condition_timer4 == 0){
		RCAP4 = valeur_autoreload;
		condition_timer4 = 1;
		out_servo = 1;
	}
	
	else 
	{
		RCAP4 = 58000;
		condition_timer4 = 0;
		out_servo = 0;
	}
	
	TL4 = RCAP4L;
	TH4 = RCAP4H;
	T4CON &=~0x80;
}
void trig()
{
	TRIG = 1;
	Delay_Micro(11);
	TRIG = 0;
}
void configuration()
{
	EA = 0;
	config_INT();	
	config_GPIO();
	config_Oscillateur();
	config_Timer_3();
	config_Timer_4();	
	config_Comparator();
	EA = 1;
}
void servomoteur_H(int angle){
	valeur_micro = ((10 * angle) + 1500);
	valeur_autoreload = (unsigned int)((-1.8433 * valeur_micro) + 65536);
}
void main (void){
	WDTCN = 0xDE;
	WDTCN = 0xAD;
	
	configuration();
	
	servomoteur_H(63); //Position servo
	Delay_Millis(1000);
	trig(); //calcul distance
	
	while(1)
	{

	}
}