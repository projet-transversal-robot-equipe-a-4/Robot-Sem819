#include "c8051F020.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
sfr16 RCAP4 = 0xE4;
sfr16 RCAP2 = 0xCA;
bit epreuve = 0;
char vitesse1[8]= "20";
char vitesse2[8]= "20";

unsigned int i;
int condition_timer4 = 0;
unsigned int valeur_micro = 0;
float valeur_autoreload = 62771;

sbit out_servo = P1^6;
sfr16 TMR3 = 0x94;
sbit TRIG = P1^7;

unsigned int temps_ecoule;
int distance;

sfr16 ADC0 = 0xBE;
float Mesure_courant;
float Courant;

void init(){
	WDTCN = 0xDE;   // Dévalidation du watchdog 
	WDTCN = 0xAD;
	EA=1;
	XBR0 |= 0x04; //assigne UART0
	XBR2 |= 0x44; // enable crossbar et assigne UART1	
	
	EIE2 |= 0x04; //Enable interruption 16 timer 4 overflow
	EIE1 = 0x30; //Comparateur 0
	ES0 = 1;	//Enable interruption 4 UART0
	P1MDOUT |= 0xC0; //P1.6 et P1.7 en sortie
	
	out_servo = 0;
	
	REF0CN |= 0x03; // enable Vref	
}

void Config_ADC(){
	AMX0CF = 0x00; // Configuration register AIN0 et AIN1 independantes + AIN6 et AIN7 inde
	AMX0SL = 0x00; // select AIN0
	ADC0CF = 0x80; // gain de 1 et config horloge SAR avec oscillo externe
	AD0TM = 0; // track ing continous
	AD0CM0 = 0; // start conversion mode select
	AD0CM1 = 0; // start conversion mode select
	AD0LJST = 0; // left justified
	AD0EN = 1; // enable ADC0
	AD0INT = 0; // flag set to 0
	AD0WINT = 0; //   ADC 0 WINDOW COMPARE INTERRUPT FLAG
}

void oscillo(){
	OSCXCN &=0x00; // reset de EXT Oscillator
	OSCXCN |= 0x67;	//EXT Oscillator	(Bit4: divisé 2)
	while((OSCXCN & 0x80) != 0x80) {}
	OSCICN |= 0x08; // enable externate oscillator
	OSCICN &= 0xFB; // disable internal oscillator
}

void config_UART0(){
	ES0 = 1; // interrupt UART0 enable...
	SCON0 = 0x50; 
	P0MDOUT|=0x14;
}	

void config_UART1(){
	EIE2 |= 0x40;
	SCON1 |= 0x50;
}	

void config_Timer_1(){
	CKCON|=0x10;
	PCON |= 0x90;
	TH1 = 184;
	TMOD |= 0x20;       //  Timer1 CLK = system clock
	TMOD &= 0xaf;	
	TR1 = 1;
	
}
void config_Comparator(){
	CPT0CN = 0x80;
}
void config_Timer_2(){
	CKCON &= ~0x20;
	TR2 = 1;
}
void config_Timer_3(){
	TMR3CN = 0x02;
}
void config_Timer_4()
{
	T4CON &= ~0xC3;
	RCAP4 = 62771; //De base position au centre (0°)
	T4CON |= 0x04;
}

void Delay_milli(unsigned int millis)
{
    unsigned int i;
    RCAP2 = (65536 - (22118400 / (12 * 1000)));
		TF2 = 0;
    for (i = 0; i < millis; i++)
    {
        while(TF2 == 0){}
				TF2 = 0;
    }
}

void Delay_Micro(unsigned int micros)
{
    unsigned int g;
    CKCON &= ~0x40;
    T4CON = 0;
    RCAP4 = (65536 - (22118400 / (12 * 1000000)));
    TL4 = RCAP4L;
    TH4 = RCAP4H;
    
    T4CON |= 0x04;
    
    for (g = 0; g < micros; g++)
    {
        while((T4CON & 0x80) == 0)
        {
            T4CON &= ~0x80;
        }
    }
}

void AnalogEntree(){
	AMX0SL = 0x00; // select AIN0
	AD0BUSY = 1; // start conversion
	while (AD0BUSY == 1) {}
	Courant = Mesure_courant;
}

void Putchar0(char c)
{
	SBUF0 = c;
	while(TI0 == 0){}
	TI0 = 0;
}

void Putchar1(char c)
{
	SBUF1 = c;
	while((SCON1 & 0x02) == 0){} //Flag passe à 1 à la fin de transmission
	SCON1 &= ~0x02; //Flag de transmission Mis à 0
}

void send_string0(char* mot)
{
	int i = 0;
	for(i = 0; i < strlen(mot); i++){
			Putchar0(mot[i]);
	}
}

void send_string1(char* mot)
{
	int i = 0;
	for(i = 0; i < strlen(mot); i++){
			Putchar1(mot[i]);
	}
}

void deplacement(char* vit1,char* vit2)
{
	char string_envoi[20];
	strcpy(string_envoi,"mogo 1:");
	strcat(string_envoi,vit1);
	strcat(string_envoi," 2:");
	strcat(string_envoi,vit2);
	strcat(string_envoi,"\r");
	send_string1(string_envoi);
}

void servomoteur_H(int angle)
{
	valeur_micro = ((10 * angle) + 1500);
	valeur_autoreload = (unsigned int)((-1.8433 * valeur_micro) + 65536);
}

void trig()
{
	condition_timer4 = 0;
	TRIG = 1;
	Delay_Micro(10);
	TRIG = 0;
}

void Encodage_uart0(){
	char * str_putty;
	int cur_len;
	char char_putty;
	char * temp;
	char commande[40];
	char type[8];
	char angle[8];
	char commande_valeur[8];
	int test_valeur;
	char vitesse1_tmp[8];
	char vitesse2_tmp[8];
	char sens_rotation;
	char X[8];
	char Y[8];
	
	
	char_putty = SBUF0;
	cur_len = strlen(str_putty);
	temp = str_putty;
	str_putty = (char *) malloc(2 + cur_len);
	strcpy(str_putty,temp);
	str_putty[cur_len] = char_putty;
	str_putty[cur_len+1] = '\0';

	if (SBUF0 == 0x0D){
		cur_len = strlen(str_putty);
		str_putty[cur_len] = '\0';
		if (strcmp(str_putty,"D 1\r") == 0 && !epreuve) {
			send_string0("debut de l'epreuve\r\n>");
			epreuve = 1;
		} 
		else if (strcmp(str_putty, "E\r") == 0 && epreuve){
			send_string0("Fin de l'epreuve\r\n>");
			epreuve=0;
		}
		// Deplacemements 
		else if (strcmp(str_putty, "A\r") == 0 && epreuve){
			deplacement(vitesse1,vitesse2);
		}
		else if (strcmp(str_putty, "S\r") == 0 && epreuve){
			send_string1("stop\r");
		}
		else if (str_putty[0] == 'T' && str_putty[1] == 'V') {
			sscanf(str_putty, "%s %s",commande,commande_valeur);
			test_valeur = atoi(commande_valeur);
			if (test_valeur >= 100) {
				send_string0("\r\n#\r\n>");
			}
			else if (strcmp(commande_valeur,"") != 0) {
				strcpy(vitesse1,commande_valeur);
				strcpy(vitesse2,commande_valeur);
				send_string0("\r\n>");
			}
		}
		else if (strcmp(str_putty, "B\r") == 0 && epreuve){
			strcpy(vitesse1_tmp,"-");
			strcat(vitesse1_tmp,vitesse1);
			strcpy(vitesse2_tmp,"-");
			strcat(vitesse2_tmp,vitesse2);
			deplacement(vitesse1_tmp,vitesse2_tmp);
		}
		else if (str_putty[0] == 'A' && epreuve){
			sscanf(str_putty, "%s %s",commande,commande_valeur);
			test_valeur = atoi(commande_valeur);
			if (test_valeur >= 100) {
				send_string0("\r\n#\r\n>");
			}
			else if (strcmp(commande_valeur,"") != 0) {
				deplacement(commande_valeur,commande_valeur);
			}
		}
		else if (str_putty[0] == 'B' && epreuve){
			sscanf(str_putty, "%s %s",commande,commande_valeur);
			test_valeur = atoi(commande_valeur);
			if (test_valeur >= 100) {
				send_string0("\r\n#\r\n>");
			}
			else if (strcmp(commande_valeur,"") != 0) {
				strcpy(vitesse1_tmp,"-");
				strcat(vitesse1_tmp,commande_valeur);
				strcpy(vitesse2_tmp,"-");
				strcat(vitesse2_tmp,commande_valeur);
				deplacement(vitesse1_tmp,vitesse2_tmp);
			}
		}
		else if (strcmp(str_putty,"RD\r") == 0 && epreuve){
			strcpy(vitesse1_tmp,"-15");
			strcpy(vitesse2_tmp,"15");
			deplacement(vitesse1_tmp,vitesse2_tmp);
			Delay_milli(745);
			send_string1("stop\r");
		}
		else if (strcmp(str_putty, "RG\r") == 0 && epreuve){
				strcpy(vitesse1_tmp,"15");
				strcpy(vitesse2_tmp,"-15");
				deplacement(vitesse1_tmp,vitesse2_tmp);
				Delay_milli(745);
				send_string1("stop\r");
		}
		else if (strcmp(str_putty, "RCG\r") == 0 && epreuve){
				strcpy(vitesse1_tmp,"15");
				strcpy(vitesse2_tmp,"-15");
				deplacement(vitesse1_tmp,vitesse2_tmp);
				Delay_milli(1350);
				send_string1("stop\r");
		}
		else if (strcmp(str_putty, "RCD\r") == 0 && epreuve){
				strcpy(vitesse1_tmp,"-15");
				strcpy(vitesse2_tmp,"15");
				deplacement(vitesse1_tmp,vitesse2_tmp);
				Delay_milli(1350);
				send_string1("stop\r");
		}
		else if (str_putty[0] == 'R' && str_putty[1] == 'A' && epreuve){
				sscanf(str_putty, "%s %c:%s",commande,sens_rotation,commande_valeur);
				test_valeur = atoi(commande_valeur);
				if (sens_rotation == 'G') {
					strcpy(vitesse1_tmp,"15");
					strcpy(vitesse2_tmp,"-15");
				} else if (sens_rotation == 'D') {
					strcpy(vitesse1_tmp,"-15");
					strcpy(vitesse2_tmp,"15");
				} else {
					send_string0("\r\n#\r\n>");
				}
				deplacement(vitesse1_tmp,vitesse2_tmp);
				Delay_milli(test_valeur*8);
				send_string1("stop\r");
				}
		else if (str_putty[0] == 'G' && epreuve) {
			sscanf(str_putty, "%s X:%s Y:%s A:%s",commande,X,Y,angle);
			strcpy(commande,"digo 1 : ");
			strcat(commande,X);
			strcat(commande," : ");
			strcat(commande,vitesse1);
			strcat(commande," 2 : ");
			strcat(commande,X);
			strcat(commande," : ");
			strcat(commande,vitesse2);
			strcat(commande,"\r");
			send_string1(commande);
		}
			
	// servomoteur 
		else if (str_putty[0] == 'C' && str_putty[1] == 'S' && epreuve){
					sscanf(str_putty, "%s %s %s:%s",commande,type,angle, commande_valeur);
					if(strstr(commande_valeur, "-")) {
						sscanf(commande_valeur,"-%s",commande_valeur);
						test_valeur = - atoi(commande_valeur);
					} else {
						test_valeur = atoi(commande_valeur);
					}
					servomoteur_H(test_valeur);
					send_string0("\r\nAS H\r\n>");
		}
	
	// telemetre
		else if (str_putty[0] == 'M' && str_putty[1] == 'O' & str_putty[2] == 'U' && epreuve){
			trig();
		}
	// mesure courant
		else if (str_putty[0] == 'M' && str_putty[1] == 'I' && epreuve){
			AnalogEntree();
			sprintf(commande_valeur,"%.3f",Courant);
			send_string0("Courant instantane : ");
			send_string0(commande_valeur);
			send_string0("\r\n>");
		}
			
	else {
			send_string1(str_putty);
		}
	str_putty = "";
	}
}

void Encodage_uart1(){
	char * str_putty;
	int cur_len;
	char char_putty;
	char * temp;
	
	char_putty = SBUF1;
	cur_len = strlen(str_putty);
	temp = str_putty;
	str_putty = (char *) malloc(2 + cur_len);
	strcpy(str_putty,temp);
	str_putty[cur_len] = char_putty;
	str_putty[cur_len+1] = '\0';

	if (SBUF1 == 0x3E) {
		if ( strcmp(str_putty,"\r\nNACK\r\n>") == 0 ) {
				send_string0("\r\n#\r\n>");
		} else {
			send_string0(str_putty);
		}
		str_putty="";
	}
}
void interrupt_reception0(void) interrupt 4{
	if(RI0==1){
		RI0 = 0;
		//test si reception est fini
		Encodage_uart0();
		
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
	char commande_valeur[8];
	TMR3CN &= ~0x04; //Stop timer 3
	CPT0CN &= 0x80; //RAZ Drapeau comparateur
	temps_ecoule = (unsigned int)(TMR3) * 13 / 24;
	distance = temps_ecoule / 58;
	sprintf(commande_valeur,"%d",distance);
	strcat(commande_valeur,"\r\n>");
	send_string0("Distance : ");
	send_string0(commande_valeur);
}

void interrupt_reception1(void) interrupt 20{
	if (SCON1 & 0x01 == 1) {
		SCON1 &= ~0x01;
		//test si reception est fini
		Encodage_uart1();
		
	}
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
		RCAP4 = 55000;
		condition_timer4 = 0;
		out_servo = 0;
	}
	T4CON &=~0x80;
	TL4 = RCAP4L;
  TH4 = RCAP4H;
}

void finConversion() interrupt 15 {
	AD0INT = 0; // flag set 0
	Mesure_courant = (ADC0/4096.0)*10.192;
}

void main(){	
	init();
	oscillo();
	config_Timer_1();
	config_Timer_2();
	config_Timer_3();
	config_Timer_4();
	config_Comparator();
	config_UART0();
	config_UART1();
	Config_ADC();
	while(1){}
}